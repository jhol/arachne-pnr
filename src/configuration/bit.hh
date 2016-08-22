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

#ifndef PNR_CONFIGURATION_BIT_HH
#define PNR_CONFIGURATION_BIT_HH

#include "bstream.hh"

#include <ostream>

namespace pnr {
namespace configuration {

class Bit
{
public:
  friend std::ostream &operator<<(std::ostream &s, const Bit &cbit);
  friend pnr::obstream &operator<<(pnr::obstream &obs, const Bit &cbit);
  friend pnr::ibstream &operator>>(pnr::ibstream &ibs, Bit &cbit);
  template<typename T> friend struct std::hash;
  
  int tile;
  int row;
  int col;
  
public:
  Bit()
    : tile(0), row(0), col(0)
  {}
  Bit(int tile_, int r, int c)
    : tile(tile_), row(r), col(c)
  {}
  
  Bit with_tile(int new_t) const { return Bit(new_t, row, col); }
  
  bool operator==(const Bit &rhs) const;
  bool operator!=(const Bit &rhs) const { return !operator==(rhs); }
  
  bool operator<(const Bit &rhs) const;
};

inline pnr::obstream &operator<<(pnr::obstream &obs, const Bit &cbit)
{
  return obs << cbit.tile << cbit.row << cbit.col;
}

inline pnr::ibstream &operator>>(pnr::ibstream &ibs, Bit &cbit)
{
  return ibs >> cbit.tile >> cbit.row >> cbit.col;
}

}
}

namespace std {

template<>
struct hash<pnr::configuration::Bit>
{
public:
  size_t operator() (const pnr::configuration::Bit &cbit) const
  {
    std::hash<int> hasher;
    size_t h = hasher(cbit.tile);
    h = hash_combine(h, hasher(cbit.row));
    return hash_combine(h, hasher(cbit.col));
  }
};

}

#endif
