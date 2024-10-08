#include "spline.h"

#include <TDatabasePDG.h>
#include <TF1.h>
#include <TFile.h>
#include <TGraph.h>
#include <TSpline.h>

#include <iostream>
#include <ranges>

namespace {
constexpr double min{0.1}, max{100.};

std::vector<std::string> get_channels(const std::string &channel_name) {
  const std::map<std::string, std::vector<std::string>> channel_set_map{
      {"qel_cc", {"qel_cc_p", "qel_cc_n"}},
      {"res_cc", {"res_cc_p", "res_cc_p"}},
      {"total_comparable",
       {"mec_cc", "qel_cc_p", "qel_cc_n", "res_cc_p", "res_cc_n", "dis_cc"}}};
  if (auto it = channel_set_map.find(channel_name);
      it != channel_set_map.end()) {
    return it->second;
  }
  return {channel_name};
}

std::string get_nucleus_name(int id) {
  switch (id) {
  case 2212:
  case 1000010010:
    return "H1";
  case 1000060120:
    return "C12";
  case 1000180400:
    return "Ar40";
  case 1000260560:
    return "Fe56";
  default:
    throw std::runtime_error("Unknown nucleus id :" + std::to_string(id));
  }
}

std::string get_neutrino_name(int id) {
  switch (id) {
  case 12:
    return "nu_e";
  case 14:
    return "nu_mu";
  case 16:
    return "nu_tau";
  case -12:
    return "nu_e_bar";
  case -14:
    return "nu_mu_bar";
  case -16:
    return "nu_tau_bar";
  default:
    throw std::runtime_error("Unknown neutrino id :" + std::to_string(id));
  }
}

TF1 get_spline_sum(TFile *file, const std::vector<std::string> &channels,
                   std::string interaction) {
  return {
      interaction.c_str(),
      [spline_vec =
           channels | std::views::transform([&](auto &&channel) {
             // IIRC the TFile instance will still hold the ownership of those
             //  TGraph objects, so no need to worry about memory leak.
             return file->Get<TGraph>((interaction + "/" + channel).c_str());
           }) |
           std::views::filter([](auto &&graph) -> bool { return graph; }) |
           std::views::transform(
               [](auto &&graph) { return TSpline3{"", graph}; }) |
           std::ranges::to<std::vector>()](const double *x, const double *) {
        return std::ranges::fold_left(
            spline_vec | std::views::transform(
                             [&](auto &&spline) { return spline.Eval(x[0]); }),
            0.0, std::plus<double>{});
      },
      min, max, 0};
}
} // namespace

spline_reader::spline_reader(std::string filename)
    : file(std::make_unique<TFile>(filename.data())) {}

spline_reader::~spline_reader() = default;

double spline_reader::get_cross_section(int neutrino_id, int target_id,
                                        double energy,
                                        const std::string &channel_name) {
  if (energy < min || energy > max) {
    std::cerr << std::format("Energy {} is out of range [{}, {}].\t Are sure "
                             "this is what you wanted? But proceeding anyway.",
                             energy, min, max)
              << std::endl;
  }
  auto key = std::make_tuple(neutrino_id, target_id, channel_name);
  auto it = cross_section_objects.find(key);
  if (it == cross_section_objects.end()) {
    auto neutrino_name = get_neutrino_name(neutrino_id);
    auto nucleus_name = get_nucleus_name(target_id);
    auto channels = get_channels(channel_name);
    auto interaction = neutrino_name + "_" + nucleus_name;
    it = cross_section_objects
             .emplace(key, get_spline_sum(file.get(), channels, interaction))
             .first; // emplace returns a pair<iterator, bool>
  }
  return it->second.Eval(energy);
}
