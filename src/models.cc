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

#include "models.hh"

#include "design.hh"

namespace pnr {

Models::Models(const Design *d)
{
  lut4 = d->find_model("SB_LUT4");
  carry = d->find_model("SB_CARRY");
  lc = d->find_model("ICESTORM_LC");
  io = d->find_model("SB_IO");
  gb = d->find_model("SB_GB");
  gb_io = d->find_model("SB_GB_IO");
  ram = d->find_model("SB_RAM40_4K");
  ramnr = d->find_model("SB_RAM40_4KNR");
  ramnw = d->find_model("SB_RAM40_4KNW");
  ramnrnw = d->find_model("SB_RAM40_4KNRNW");
  warmboot = d->find_model("SB_WARMBOOT");
  tbuf = d->find_model("$_TBUF_");
}

}
