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

#include "cbitval.hh"

namespace pnr {

std::ostream &
operator<<(std::ostream &s, const CBitVal &cv)
{
  for (const auto &p : cv.cbit_val)
    {
      if (p.second)
        s << "1";
      else
        s << "0";
    }
  for (const auto &p : cv.cbit_val)
    s << " " << p.first;
  return s;
}

std::set<CBit>
CBitVal::cbits() const
{
  return keys(cbit_val);
}

}
