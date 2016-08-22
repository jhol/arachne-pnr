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

#ifndef PNR_CONFIGURATION_BITVAL_HH
#define PNR_CONFIGURATION_BITVAL_HH

#include "bit.hh"

#include <ostream>
#include <map>
#include <set>

namespace pnr {
namespace configuration {

class BitVal
{
public:
  friend std::ostream &operator<<(std::ostream &s, const BitVal &cbits);
  
  std::map<Bit, bool> cbit_val;
  
public:
  BitVal() {}
  BitVal(const std::map<Bit, bool> &cbv)
    : cbit_val(cbv)
  {}
  
  std::set<Bit> cbits() const;
};

}
}

#endif
