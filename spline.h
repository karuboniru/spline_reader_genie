#pragma once

#include <map>
#include <memory>
#include <string>
#include <tuple>

class TFile;
class TF1;

class spline_reader {
public:
  spline_reader(std::string filename);
  spline_reader(spline_reader &&) = default;
  spline_reader(const spline_reader &) = delete;
  spline_reader &operator=(const spline_reader &) = delete;
  spline_reader &operator=(spline_reader &&) = default;
  ~spline_reader();

  double
  get_cross_section(int neutrino_id, int target_id, double energy,
                    const std::string &channel_name = "total_comparable");

private:
  std::unique_ptr<TFile> file;
  std::map<
      std::tuple<int, int, std::string> /* neutrino_id, target_id, channel */,
      TF1>
      cross_section_objects;
};