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

#ifndef PNR_CELLTYPE_HH
#define PNR_CELLTYPE_HH

enum class CellType : int {
  LOGIC, IO, GB, RAM, WARMBOOT, PLL,
};

std::string cell_type_name(CellType ct);

constexpr int cell_type_idx(CellType type)
{
  return static_cast<int>(type);
}

static const int n_cell_types = cell_type_idx(CellType::PLL) + 1;

inline obstream &operator<<(obstream &obs, CellType t)
{
  return obs << static_cast<int>(t);
}

inline ibstream &operator>>(ibstream &ibs, CellType &t)
{
  int x;
  ibs >> x;
  t = static_cast<CellType>(x);
  return ibs;
}

#endif
