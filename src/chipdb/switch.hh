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

#ifndef PNR_CHIPDB_SWITCH_HH
#define PNR_CHIPDB_SWITCH_HH

#include "configuration/bit.hh"

#include <map>
#include <vector>

namespace pnr {

class ibstream;
class obstream;

namespace chipdb {

class Switch
{
public:
  friend pnr::obstream &operator<<(pnr::obstream &obs, const Switch &sw);
  friend pnr::ibstream &operator>>(pnr::ibstream &ibs, Switch &sw);
  
  bool bidir; // routing
  int tile;
  int out;
  std::map<int, unsigned> in_val;
  std::vector<configuration::Bit> cbits;
  
public:
  Switch() {}
  Switch(bool bi,
         int t,
         int o,
         const std::map<int, unsigned> &iv,
         const std::vector<configuration::Bit> &cb)
    : bidir(bi),
      tile(t),
      out(o),
      in_val(iv),
      cbits(cb)
  {}
};

pnr::obstream &operator<<(pnr::obstream &obs, const chipdb::Switch &sw);
pnr::ibstream &operator>>(pnr::ibstream &ibs, chipdb::Switch &sw);

}
}

#endif
