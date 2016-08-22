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

#ifndef PNR_CHIPDBPARSER_HH
#define PNR_CHIPDBPARSER_HH

#include "cbit.hh"
#include "line_parser.hh"

#include <string>

class ChipDB;

class ChipDBParser : public LineParser
{
  ChipDB *chipdb;
  
  CBit parse_cbit(int tile, const std::string &s);
  
  void parse_cmd_device();
  void parse_cmd_pins();
  void parse_cmd_gbufpin();
  void parse_cmd_tile();
  void parse_cmd_tile_bits();
  void parse_cmd_net();
  void parse_cmd_buffer_routing();
  void parse_cmd_colbuf();
  void parse_cmd_gbufin();
  void parse_cmd_iolatch();
  void parse_cmd_ieren();
  void parse_cmd_extra_bits();
  void parse_cmd_extra_cell();
  
public:
  ChipDBParser(const std::string &f, std::istream &s_)
    : LineParser(f, s_), chipdb(nullptr)
  {}
  
  ChipDB *parse();
};

#endif
