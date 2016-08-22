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

#include "chipdbparser.hh"

#include "chipdb.hh"
#include "util.hh"

#include <cassert>
#include <cstring>

CBit
ChipDBParser::parse_cbit(int t, const std::string &s_)
{
  std::size_t lbr = s_.find('['),
    rbr = s_.find(']');
  
  if (s_[0] != 'B'
      || lbr == std::string::npos
      || rbr == std::string::npos)
    fatal("invalid cbit spec");
  
  std::string rows(&s_[1], &s_[lbr]),
    cols(&s_[lbr + 1], &s_[rbr]);
  
  int r = std::stoi(rows),
    c = std::stoi(cols);
  
  return CBit(t, r, c);
}

void
ChipDBParser::parse_cmd_device()
{
  if (words.size() != 5)
    fatal("wrong number of arguments");
  
  chipdb->set_device(words[1],
                     std::stoi(words[2]),
                     std::stoi(words[3]),
                     std::stoi(words[4]));
  
  // next command
  read_line();
}

void
ChipDBParser::parse_cmd_pins()
{
  if (words.size() != 2)
    fatal("wrong number of arguments");
  
  const std::string &package_name = words[1];
  Package &package = chipdb->packages[package_name];
  
  package.name = package_name;
  
  for (;;)
    {
      read_line();
      if (eof()
          || line[0] == '.')
        return;
      
      if (words.size() != 4)
        fatal("invalid .pins entry");
      
      const std::string &pin = words[0];
      int x = std::stoi(words[1]),
        y = std::stoi(words[2]),
        pos = std::stoi(words[3]);
      int t = chipdb->tile(x, y);
      Location loc(t, pos);
      extend(package.pin_loc, pin, loc);
      extend(package.loc_pin, loc, pin);
    }
}

void
ChipDBParser::parse_cmd_gbufpin()
{
  if (words.size() != 1)
    fatal("wrong number of arguments");
              
  for (;;)
    {
      read_line();
      if (eof()
          || line[0] == '.')
        return;
      
      if (words.size() != 4)
        fatal("invalid .gbufpin entry");
      
      int x = std::stoi(words[0]),
        y = std::stoi(words[1]),
        pos = std::stoi(words[2]),
        glb_num = std::stoi(words[3]);
      int t = chipdb->tile(x, y);
      Location loc(t, pos);
      extend(chipdb->loc_pin_glb_num, loc, glb_num);
      
      chipdb->add_cell(CellType::GB, Location(t, 2));
    }
}

void
ChipDBParser::parse_cmd_tile()
{
  if (words.size() != 3)
    fatal("wrong number of arguments");
  
  int x = std::stoi(words[1]),
    y = std::stoi(words[2]);
  if (x < 0 || x >= chipdb->width)
    fatal("tile x out of range");
  if (y < 0 || y >= chipdb->height)
    fatal("tile y out of range");
  
  int t = chipdb->tile(x, y);
  
  const std::string &cmd = words[0];
  if (cmd == ".io_tile")
    {
      chipdb->tile_type[t] = TileType::IO;
      for (int p = 0; p < 2; ++p)
        chipdb->add_cell(CellType::IO, Location(t, p));
    }
  else if (cmd == ".logic_tile")
    {
      chipdb->tile_type[t] = TileType::LOGIC;
                  
      for (int p = 0; p < 8; ++p)
        chipdb->add_cell(CellType::LOGIC, Location(t, p));
    }
  else if (cmd == ".ramb_tile")
    chipdb->tile_type[t] = TileType::RAMB;
  else
    {
      assert(cmd == ".ramt_tile");
      chipdb->tile_type[t] = TileType::RAMT;
                  
      chipdb->add_cell(CellType::RAM, Location(t, 0));
    }
  
  // next command
  read_line();
}

void
ChipDBParser::parse_cmd_tile_bits()
{
  if (words.size() != 3)
    fatal("wrong number of arguments");
  
  TileType ty;
  const std::string &cmd = words[0];
  if (cmd == ".io_tile_bits")
    ty = TileType::IO;
  else if (cmd == ".logic_tile_bits")
    ty = TileType::LOGIC;
  else if (cmd == ".ramb_tile_bits")
    ty = TileType::RAMB;
  else
    {
      assert(cmd == ".ramt_tile_bits");
      ty = TileType::RAMT;
    }
  
  int n_columns = std::stoi(words[1]),
    n_rows = std::stoi(words[2]);
  
  extend(chipdb->tile_cbits_block_size,
         ty,
         std::make_pair(n_columns, n_rows));
  
  for (;;)
    {
      read_line();
      if (eof()
          || line[0] == '.')
        return;
      
      if (words.size() < 2)
        fatal("invalid tile entry");
      
      const std::string &func = words[0];
      
      std::vector<CBit> cbits(words.size() - 1);
      for (unsigned i = 1; i < words.size(); ++i)
        cbits[i - 1] = parse_cbit(0, words[i]);
      
      extend(chipdb->tile_nonrouting_cbits[ty], func, cbits);
    }

}

void
ChipDBParser::parse_cmd_net()
{
  if (words.size() != 2)
    fatal("wrong number of arguments");
  
  int n = std::stoi(words[1]);
  if (n < 0)
    fatal("invalid net index");
  
  bool first = true;
  for (;;)
    {
      read_line();
      if (eof()
          || line[0] == '.')
        return;
      
      if (words.size() != 3)
        fatal("invalid .net entry");
      
      int x = std::stoi(words[0]),
        y = std::stoi(words[1]);
      if (x < 0 || x >= chipdb->width)
        fatal("tile x out of range");
      if (y < 0 || y >= chipdb->height)
        fatal("tile y out of range");
      int t = chipdb->tile(x, y);
      
      if (first)
        {
          chipdb->net_tile_name[n] = std::make_pair(t, words[2]);
          first = false;
        }
      extend(chipdb->tile_nets[t], words[2], n);
    }
}

void
ChipDBParser::parse_cmd_buffer_routing()
{
  if (words.size() < 5)
    fatal("too few arguments");
  
  bool bidir = words[0] == ".routing";
  
  int x = std::stoi(words[1]),
    y = std::stoi(words[2]);
  if (x < 0 || x >= chipdb->width)
    fatal("tile x out of range");
  if (y < 0 || y >= chipdb->height)
    fatal("tile y out of range");
  int t = chipdb->tile(x, y);
  
  int n = std::stoi(words[3]);
  if (n < 0)
    fatal("invalid net index");
  
  std::vector<CBit> cbits(words.size() - 4);
  for (unsigned i = 4; i < words.size(); i ++)
    cbits[i - 4] = parse_cbit(t, words[i]);
  
  std::map<int, unsigned> in_val;             
  
  for (;;)
    {
      read_line();
      if (eof()
          || line[0] == '.')
        {
          chipdb->switches.push_back(Switch(bidir,
                                            t,
                                            n,
                                            in_val,
                                            cbits));
          return;
        }
      
      const std::string &sval = words[0];
      
      if (words.size() != 2
          || sval.size() != cbits.size())
        fatal("invalid .buffer/.routing entry");
      
      int n2 = std::stoi(words[1]);
      
      unsigned val = 0;
      for (unsigned i = 0; i < sval.size(); i ++)
        {
          if (sval[i] == '1')
            val |= (1 << i);
          else
            {
              if (sval[i] != '0')
                fatal("invalid binary string");
            }
        }
      
      extend(in_val, n2, val);
    }
}

void
ChipDBParser::parse_cmd_colbuf()
{
  for (;;)
    {
      read_line();
      if (eof()
          || line[0] == '.')
        return;
      
      if (words.size() != 4)
        fatal("invalid .colbuf entry");
      
      int src_x = std::stoi(words[0]);
      int src_y = std::stoi(words[1]);
      int dst_x = std::stoi(words[2]);
      int dst_y = std::stoi(words[3]);
      
      chipdb->tile_colbuf_tile[chipdb->tile(dst_x, dst_y)]
        = chipdb->tile(src_x, src_y);
    }
}

void
ChipDBParser::parse_cmd_gbufin()
{
  for (;;)
    {
      read_line();
      if (eof()
          || line[0] == '.')
        return;
      
      if (words.size() != 3)
        fatal("invalid .gbufin entry");
      
      int g = std::stoi(words[2]);
      assert(g < chipdb->n_global_nets);
      
      extend(chipdb->gbufin,
             std::make_pair(std::stoi(words[0]), std::stoi(words[1])),
             g);
    }
}

void
ChipDBParser::parse_cmd_iolatch()
{
  for (;;)
    {
      read_line();
      if (eof()
          || line[0] == '.')
        return;
      
      if (words.size() != 2)
        fatal("invalid .iolatch entry");
      
      int x = std::stoi(words[0]),
        y = std::stoi(words[1]);
      chipdb->iolatch.push_back(chipdb->tile(x, y));
    }
}

void
ChipDBParser::parse_cmd_ieren()
{
    for (;;)
      {
        read_line();
        if (eof()
            || line[0] == '.')
          return;
        
        if (words.size() != 6)
          fatal("invalid .ieren entry");
        
        int pio_t = chipdb->tile(std::stoi(words[0]),
                                 std::stoi(words[1])),
          ieren_t = chipdb->tile(std::stoi(words[3]),
                                 std::stoi(words[4]));
        
        Location pio(pio_t, std::stoi(words[2])),
          ieren(ieren_t, std::stoi(words[5]));
        extend(chipdb->ieren, pio, ieren);
      }
}

void
ChipDBParser::parse_cmd_extra_bits()
{
  for (;;)
    {
      read_line();
      if (eof()
          || line[0] == '.')
        return;
      
      if (words.size() != 4)
        fatal("invalid .extra_bits entry");
      
      int bank_num = std::stoi(words[1]),
        addr_x = std::stoi(words[2]),
        addr_y = std::stoi(words[3]);
      
      extend(chipdb->extra_bits,
             words[0],
             std::make_tuple(bank_num, addr_x, addr_y));
    }
}

void
ChipDBParser::parse_cmd_extra_cell()
{
  if (words.size() != 4)
    fatal("wrong number of arguments to .extra_cell");
              
  const std::string &cell_type = words[3];
  int x = std::stoi(words[1]),
    y = std::stoi(words[2]);
  int t = chipdb->tile(x, y);
  
  int c = 0;
  if (cell_type == "WARMBOOT")
    c = chipdb->add_cell(CellType::WARMBOOT, Location(t, 0));
  else if (cell_type == "PLL")
    c = chipdb->add_cell(CellType::PLL, Location(t, 3));
  else
    fatal(fmt("unknown extra cell type `" << cell_type << "'"));
  
  std::map<std::string, std::pair<int, std::string>> mfvs;
  for (;;)
    {
      read_line();
      if (eof()
          || line[0] == '.')
        {
          extend(chipdb->cell_mfvs, c, mfvs);
          return;
        }

      if (words.size() > 0 && words[0] == "LOCKED")
        continue;
      
      if (words.size() != 4)
        fatal("invalid .extra_cell entry");
      
      int mfv_t = chipdb->tile(std::stoi(words[1]),
                               std::stoi(words[2]));
      extend(mfvs, words[0],
             std::make_pair(mfv_t,
                            words[3]));
    }
}

ChipDB *
ChipDBParser::parse()
{
  chipdb = new ChipDB;
  
  read_line();
  for (;;)
    {
      if (eof())
        break;
      if (line[0] != '.')
        fatal(fmt("expected command, got '" << words[0] << "'"));
      
      const std::string &cmd = words[0];
      if (cmd == ".device")
        parse_cmd_device();
      else if (cmd == ".pins")
        parse_cmd_pins();
      else if (cmd == ".gbufpin")
        parse_cmd_gbufpin();
      else if (cmd == ".io_tile"
               || cmd == ".logic_tile"
               || cmd == ".ramb_tile"
               || cmd == ".ramt_tile")
        parse_cmd_tile();
      else if (cmd == ".io_tile_bits"
               || cmd == ".logic_tile_bits"
               || cmd == ".ramb_tile_bits"
               || cmd == ".ramt_tile_bits")
        parse_cmd_tile_bits();
      else if (cmd == ".net")
        parse_cmd_net();
      else if (cmd == ".buffer"
               || cmd == ".routing")
        parse_cmd_buffer_routing();
      else if (cmd == ".colbuf")
        parse_cmd_colbuf();
      else if (cmd == ".gbufin")
        parse_cmd_gbufin();
      else if (cmd == ".iolatch")
        parse_cmd_iolatch();
      else if (cmd == ".ieren")
        parse_cmd_ieren();
      else if (cmd == ".extra_bits")
        parse_cmd_extra_bits();
      else if (cmd == ".extra_cell")
        parse_cmd_extra_cell();
      else
        fatal(fmt("unknown directive '" << cmd << "'"));
    }
  
  chipdb->finalize();
  return chipdb;
}
