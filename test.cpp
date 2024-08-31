#include "spline.h"
#include <iostream>

int main() {
  // Initialize the class with the path to the spline file (Generated by GENIE)
  spline_reader sp{
      "/var/home/yan/neutrino/spline/full/genie_spline_3_0_2/G18_02a_02_11b/"
      "3.04.02-routine_validation_01-xsec_vA/total_xsec.root"};
  // Calculate the cross section for (neutrino_id, nucleus_id, energy)
  // By default, the cross section is defined by qel+res+dis+mec
  std::cout << "Cross section: \t" << sp.get_cross_section(14, 1000060120, 1.0)
            << std::endl;
  std::cout << "Cross section: \t" << sp.get_cross_section(14, 1000060120, 2.0)
            << std::endl;
  // can also specify the channel name:
  std::cout << "QEL section: \t"
            << sp.get_cross_section(14, 1000060120, 1.0, "qel_cc") << std::endl;
  // Or GENIE's tot_cc (incl. all charm channel)
  std::cout << "Cross section: \t"
            << sp.get_cross_section(14, 1000060120, 1.0, "tot_cc") << std::endl;
  // Or GENIE's specific channel
  std::cout << "Cross section: \t"
            << sp.get_cross_section(14, 1000060120, 50.0, "imd_cc")
            << std::endl;
  // All the values are in
  // 1e-38 cm^2 per nucleus
  return 0;
}