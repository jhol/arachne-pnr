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

#include "tiletype.hh"

namespace pnr {

std::string
tile_type_name(TileType t)
{
  assert(t != TileType::EMPTY);
  switch(t)
    {
    case TileType::IO:
      return "io_tile";
    case TileType::LOGIC:
      return "logic_tile";
    case TileType::RAMB:
      return "ramb_tile";
    case TileType::RAMT:
      return "ramt_tile";
    case TileType::EMPTY:
      abort();
    }    
  return std::string();
}

}
