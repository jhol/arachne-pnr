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

#ifndef PNR_CHIPDB_TILETYPE_HH
#define PNR_CHIPDB_TILETYPE_HH

#include "bstream.hh"

#include <functional>

namespace pnr {
namespace chipdb {

enum class TileType : int {
  EMPTY, IO, LOGIC, RAMB, RAMT,
};

inline pnr::obstream &operator<<(pnr::obstream &obs, chipdb::TileType t)
{
  return obs << static_cast<int>(t);
}

inline pnr::ibstream &operator>>(pnr::ibstream &ibs, chipdb::TileType &t)
{
  int x;
  ibs >> x;
  t = static_cast<chipdb::TileType>(x);
  return ibs;
}

std::string tile_type_name(TileType t);

}
}

namespace std {

template<>
struct hash<pnr::chipdb::TileType>
{
public:
  size_t operator() (pnr::chipdb::TileType x) const
  {
    std::hash<int> hasher;
    return hasher(static_cast<int>(x));
  }
};

}

#endif