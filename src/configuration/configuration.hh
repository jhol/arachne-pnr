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

#ifndef PNR_CONFIGURATION_CONFIGURATION_HH
#define PNR_CONFIGURATION_CONFIGURATION_HH

#include "bit.hh"
#include "chipdb/chipdb.hh"

#include <ostream>

namespace pnr {

namespace netlist {
class Design;
class Instance;
class Net;
class IdLess;
}

namespace configuration {

class Configuration
{
private:
  std::map<Bit, bool> cbits;
  std::set<std::tuple<int, int, int>> extra_cbits;
  
public:
  Configuration();
  
  void set_cbit(const Bit &cbit, bool value);
  void set_cbits(const std::vector<Bit> &value_cbits,
                 unsigned value);
  void set_extra_cbit(const std::tuple<int, int, int> &t);
  
  void write_txt(std::ostream &s,
                 const chipdb::ChipDB *chipdb,
                 netlist::Design *d,
                 const std::map<netlist::Instance *, int, netlist::IdLess>
		   &placement,
                 const std::vector<netlist::Net *> &cnet_net);
};

}
}

#endif
