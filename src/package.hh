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

#ifndef PNR_PACKAGE_HH
#define PNR_PACKAGE_HH

#include "bstream.hh"
#include "location.hh"

#include <ostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <cassert>

namespace pnr {

class Package
{
  friend obstream &operator<<(obstream &obs, const Package &pkg);
  friend ibstream &operator>>(ibstream &ibs, Package &pkg);
  
public:
  std::string name;
  
  std::map<std::string, Location> pin_loc;
  std::map<Location, std::string> loc_pin;
};

inline obstream &operator<<(obstream &obs, const Package &pkg)
{
  return obs << pkg.name << pkg.pin_loc;
}

inline ibstream &operator>>(ibstream &ibs, Package &pkg)
{
  ibs >> pkg.name >> pkg.pin_loc;
  for (const auto &p : pkg.pin_loc)
    extend(pkg.loc_pin, p.second, p.first);
  return ibs;
}

}

#endif
