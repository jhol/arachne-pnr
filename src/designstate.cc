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

#include "designstate.hh"

#include "netlist/design.hh"

namespace pnr {

DesignState::DesignState(const chipdb::ChipDB &chipdb_,
  const chipdb::Package &package_, netlist::Design *d_)
  : chipdb(chipdb_),
    package(package_),
    d(d_),
    models(d_),
    top(d_->top())
{
}

bool
DesignState::is_dual_pll(netlist::Instance *inst) const
{
  assert(models.is_pllX(inst));
  if (inst->instance_of()->name() == "SB_PLL40_2F_CORE"
      || inst->instance_of()->name() == "SB_PLL40_2_PAD"
      || inst->instance_of()->name() == "SB_PLL40_2F_PAD")
    return true;
  else
    {
      assert(inst->instance_of()->name() == "SB_PLL40_PAD"
             || inst->instance_of()->name() == "SB_PLL40_CORE");
      return false;
    }
}

std::vector<int>
DesignState::pll_out_io_cells(netlist::Instance *inst, int cell) const
{
  assert(models.is_pllX(inst)
         && chipdb.cell_type[cell] == chipdb::CellType::PLL);
  
  bool dual = is_dual_pll(inst);
  
  const auto &p_a = chipdb.cell_mfvs.at(cell).at("PLLOUT_A");
  chipdb::Location io_loc_a(p_a.first, std::stoi(p_a.second));
  int io_cell_a = chipdb.loc_cell(io_loc_a);
  
  std::vector<int> r;
  r.push_back(io_cell_a);
  
  if (dual)
    {
      const auto &p_b = chipdb.cell_mfvs.at(cell).at("PLLOUT_B");
      chipdb::Location io_loc_b(p_b.first, std::stoi(p_b.second));
      int io_cell_b = chipdb.loc_cell(io_loc_b);
      r.push_back(io_cell_b);
    }
  
  return r;
}

}
