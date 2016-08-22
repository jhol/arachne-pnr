/* Copyright (C) 2015 Cotton Seed
   
   This file is part of arachne-pnr.  Arachne-pnr is free software;
   you can redistribute it and/or modify it under the terms of the GNU
   General Public License version 2 as published by the Free Software
   Foundation.
   
   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program. If not, see <http://www.gnu.org/licenses/>. */

#ifndef PNR_PROCESS_HH
#define PNR_PROCESS_HH

#include <string>

namespace pnr {
namespace process {

void process(const std::string &input_file,
             const std::string &pcf_file,
             const std::string &output_file,
             const std::string &post_pack_blif,
             const std::string &post_pack_verilog,
             const std::string &post_place_pcf,
             const std::string &post_place_blif,
             const std::string &device,
             const std::string &package_name,
             unsigned int seed,
	     int max_passes,
             bool route_only,
             bool promote_globals);

}
}

#endif
