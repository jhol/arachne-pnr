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

#include "switch.hh"
#include "bstream.hh"

namespace pnr {
namespace chipdb {

pnr::obstream &operator<<(pnr::obstream &obs, const chipdb::Switch &sw)
{
  obs << sw.bidir
      << sw.tile
      << sw.out
      << sw.cbits.size();
  for (const configuration::Bit &cbit : sw.cbits)
    {
      assert(cbit.tile == sw.tile);
      obs << cbit.row << cbit.col;
    }
  obs << sw.in_val;
  return obs;
}

pnr::ibstream &operator>>(pnr::ibstream &ibs, chipdb::Switch &sw)
{
  size_t n_cbits;
  ibs >> sw.bidir
      >> sw.tile
      >> sw.out
      >> n_cbits;
  sw.cbits.resize(n_cbits);
  for (size_t i = 0; i < n_cbits; ++i)
    {
      int row, col;
      ibs >> row >> col;
      sw.cbits[i] = configuration::Bit(sw.tile, row, col);
    }
  ibs >> sw.in_val;
  return ibs;
}

}
}
