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

#ifndef PNR_PROCESS_CONSTANT_HH
#define PNR_PROCESS_CONSTANT_HH

namespace pnr {

namespace chipdb {
class ChipDB;
}

namespace netlist {
class Design;
}

namespace process {

void realize_constants(const chipdb::ChipDB *chipdb, netlist::Design *d);

}

}

#endif
