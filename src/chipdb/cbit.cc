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

#include "cbit.hh"

namespace pnr {
namespace chipdb {

bool
CBit::operator==(const CBit &rhs) const
{
  return tile == rhs.tile
    && row == rhs.row
    && col == rhs.col;
}

bool
CBit::operator<(const CBit &rhs) const
{
  if (tile < rhs.tile)
    return true;
  if (tile > rhs.tile)
    return false;

  if (row < rhs.row)
    return true;
  if (row > rhs.row)
    return false;
  
  return col < rhs.col;
}

}

std::ostream &
operator<<(std::ostream &s, const chipdb::CBit &cbit)
{
  return s << cbit.tile << " B" << cbit.row << "[" << cbit.col << "]";
}

}
