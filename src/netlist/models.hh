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

#ifndef PNR_NETLIST_MODELS_HH
#define PNR_NETLIST_MODELS_HH

#include "instance.hh"
#include "model.hh"

namespace pnr {
namespace netlist {

class Design;

class Models
{
public:
  Model *lut4,
    *carry;
  Model *lc,
    *io,
    *gb,
    *gb_io,
    *ram,
    *ramnr,
    *ramnw,
    *ramnrnw,
    *warmboot,
    *tbuf;
  
public:
  Models(const Design &d);
  
  bool is_dff(Instance *inst) const
  {
    return is_prefix("SB_DFF", inst->instance_of()->name());
  }
  bool is_lut4(Instance *inst) const { return inst->instance_of() == lut4; }
  bool is_carry(Instance *inst) const { return inst->instance_of() == carry; }
  
  bool is_lc(Instance *inst) const { return inst->instance_of() == lc; }
  bool is_io(Instance *inst) const { return inst->instance_of() == io; }
  bool is_gb(Instance *inst) const { return inst->instance_of() == gb; }
  bool is_gb_io(const Instance *inst) const { return inst->instance_of() == gb_io; }
  bool is_ioX(const Instance *inst) const
  {
    return inst->instance_of() == io
      || inst->instance_of() == gb_io;
  }
  bool is_gbX(const Instance *inst) const
  {
    return inst->instance_of() == gb
      || inst->instance_of() == gb_io;
  }
  
  bool is_ram(Instance *inst) const { return inst->instance_of() == ram; }
  bool is_ramnr(Instance *inst) const { return inst->instance_of() == ramnr; }
  bool is_ramnw(Instance *inst) const { return inst->instance_of() == ramnw; }
  bool is_ramnrnw(Instance *inst) const { return inst->instance_of() == ramnrnw; }
  bool is_warmboot(Instance *inst) const { return inst->instance_of() == warmboot; }
  bool is_tbuf(Instance *inst) const { return inst->instance_of() == tbuf; }
  
  bool is_ramX(Instance *inst) const
  { 
    return (inst->instance_of() == ram
            || inst->instance_of() == ramnr
            || inst->instance_of() == ramnw
            || inst->instance_of() == ramnrnw);
  }
  bool is_pllX(const Instance *inst) const
  {
    return is_prefix("SB_PLL40_", inst->instance_of()->name());
  }
};

}
}

#endif
