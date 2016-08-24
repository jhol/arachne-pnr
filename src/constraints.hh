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

#ifndef PNR_CONSTRAINTS_HH
#define PNR_CONSTRAINTS_HH

#include <map>
#include <string>

namespace pnr {

class DesignState;

class Constraints
{
public:
  std::map<std::string, chipdb::Location> net_pin_loc;
  
public:
  Constraints() {}
  Constraints(const std::map<std::string, chipdb::Location> &np)
    : net_pin_loc(np)
  {}
};

void place_constraints(DesignState &ds);

}

#endif
