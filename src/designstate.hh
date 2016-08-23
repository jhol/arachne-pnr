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

#ifndef PNR_DESIGNSTATE_HH
#define PNR_DESIGNSTATE_HH

#include "carry.hh"
#include "configuration/configuration.hh"
#include "chipdb/chipdb.hh"
#include "constraints.hh"
#include "netlist/models.hh"

namespace pnr {

class DesignState
{
public:
  const chipdb::ChipDB &chipdb;
  const chipdb::Package &package;
  netlist::Design &d;
  const netlist::Models models;
  netlist::Model *const top;
  Constraints constraints;
  CarryChains chains;
  std::set<netlist::Instance *, netlist::IdLess> locked;
  std::map<netlist::Instance *, int, netlist::IdLess> placement;
  std::map<netlist::Instance *, uint8_t, netlist::IdLess> gb_inst_gc;
  std::vector<netlist::Net *> cnet_net;
  configuration::Configuration conf;
  
public:
  DesignState(const chipdb::ChipDB &chipdb_, const chipdb::Package &package_,
              netlist::Design &d_);
  
  bool is_dual_pll(netlist::Instance *inst) const;
  std::vector<int> pll_out_io_cells(netlist::Instance *inst, int cell) const;
};

}

#endif
