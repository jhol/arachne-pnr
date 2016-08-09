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

#include "chipdb.hh"

#include "chipdbparser.hh"
#include "util.hh"

#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>

namespace pnr {
namespace chipdb {

ChipDB::ChipDB()
  : width(0), height(0), n_tiles(0), n_nets(0), n_global_nets(8),
    n_cells(0),
    cell_type_cells(n_cell_types),
    bank_cells(4)
{
}

int
ChipDB::add_cell(CellType type, const Location &loc)
{
  int cell = ++n_cells;
  cell_type.push_back(type);
  cell_location.push_back(loc);
  cell_type_cells[cell_type_idx(type)].push_back(cell);
  return cell;
}

int
ChipDB::tile_bank(int t) const
{
  assert(tile_type[t] == TileType::IO);
  int x = tile_x(t),
    y = tile_y(t);
  if (x == 0)
    return 3;
  else if (y == 0)
    return 2;
  else if (x == width - 1)
    return 1;
  else
    {
      assert(y == height - 1);
      return 0;
    }
}

void
ChipDB::dump(std::ostream &s) const
{
  s << ".device " << device << "\n\n";
  
  for (const auto &p : packages)
    {
      s << ".pins " << p.first << "\n";
      for (const auto &p2 : p.second.pin_loc)
        {
          int t = p2.second.tile();
          s << p2.first
            << " " << tile_x(t)
            << " " << tile_y(t)
            << " " << p2.second.pos() << "\n";
        }
      s << "\n";
    }
  
  s << ".colbuf\n";
  for (const auto &p : tile_colbuf_tile)
    s << tile_x(p.second) << " " << tile_y(p.second) << " "
      << tile_x(p.first) << " " << tile_y(p.first) << "\n";
  s << "\n";
  
  for (int i = 0; i < width; i ++)
    for (int j = 0; j < height; j ++)
      {
        int t = tile(i, j);
        switch(tile_type[t])
          {
          case TileType::EMPTY:
            break;
          case TileType::IO:
            s << ".io_tile " << i << " " << j << "\n";
            break;
          case TileType::LOGIC:
            s << ".logic_tile " << i << " " << j << "\n";
            break;
          case TileType::RAMB:
            s << ".ramb_tile " << i << " " << j << "\n";
            break;
          case TileType::RAMT:
            s << ".ramt_tile " << i << " " << j << "\n";
            break;
          }
        
        for (const auto &p : tile_nonrouting_cbits.at(tile_type[t]))
          {
            s << p.first;
            for (const auto &cbit : p.second)
              s << " " << cbit;
            s << "\n";
          }
        s << "\n";
      }
  
  std::vector<std::vector<std::pair<int, std::string>>> net_tile_names(n_nets);
  for (int i = 0; i < n_tiles; ++i)
    for (const auto &p : tile_nets[i])
      net_tile_names[p.second].push_back(std::make_pair(i, p.first));
  
  for (int i = 0; i < n_nets; ++i)
    {
      s << ".net " << i << "\n";
      for (const auto &p : net_tile_names[i])
        s << tile_x(p.first) << " " << tile_y(p.first) << " " << p.second << "\n";
      s << "\n";
    }
  
  for (unsigned i = 0; i < switches.size(); ++i)
    {
      const Switch &sw = switches[i];
      
      s << (sw.bidir ? ".routing" : ".buffer")
        << " " << tile_x(sw.tile) << " " << tile_y(sw.tile) << " " << sw.out;
      for (const configuration::Bit &cb : sw.cbits)
        s << " B" << cb.row << "[" << cb.col << "]";
      s << "\n";
      
      for (const auto &p : sw.in_val)
        {
          for (int j = 0; j < (int)sw.cbits.size(); ++j)
            {
              if (p.second & (1 << j))
                s << "1";
              else
                s << "0";
            }     
          s << " " << p.first << "\n";
        }
      s << "\n";
    }
}

void
ChipDB::set_device(const std::string &d,
                   int w, int h,
                   int n_nets_)
{
  device = d;
  width = w;
  height = h;
  n_tiles = width * height;
  n_nets = n_nets_;
  
  tile_type.resize(n_tiles, TileType::EMPTY);
  tile_nets.resize(n_tiles);
  
  net_tile_name.resize(n_nets);
  out_switches.resize(n_nets);
  in_switches.resize(n_nets);
}

void
ChipDB::finalize()
{
  int t1c1 = tile(1, 1);
  for (const auto &p : tile_nets[t1c1])
    {
      if (is_prefix("glb_netwk_", p.first))
        {
          int n = std::stoi(&p.first[10]);
          extend(net_global, p.second, n);
        }
    }
  
  for (int i = 1; i <= n_cells; ++i)
    {
      int t = cell_location[i].tile();
      if (tile_type[t] != TileType::IO)
        continue;
      
      int b = tile_bank(t);
      bank_cells[b].push_back(i);
    }
  
  tile_pos_cell.resize(n_tiles);
  for (int i = 0; i < n_tiles; ++i)
    {
      switch(tile_type[i])
        {
        case TileType::LOGIC:
          tile_pos_cell[i].resize(8, 0);
          break;
        case TileType::IO:
          tile_pos_cell[i].resize(4, 0);
          break;
        case TileType::RAMT:
          tile_pos_cell[i].resize(1, 0);
          break;
        default:
          break;
        }
    }
  for (int i = 1; i <= n_cells; ++i)
    {
      const Location &loc = cell_location[i];
      // *logs << i << " " << loc << "\n";
      int t = loc.tile();
      int pos = loc.pos();
      if ((int)tile_pos_cell[t].size() <= pos)
        tile_pos_cell[t].resize(pos + 1, 0);
      assert(tile_pos_cell[t][pos] == 0);
      tile_pos_cell[t][pos] = i;
    }
  
  in_switches.resize(n_nets);
  out_switches.resize(n_nets);
  for (size_t s = 0;  s < switches.size(); ++s)
    {
      int out = switches[s].out;
      extend(out_switches[out], s);
      for (const auto &p : switches[s].in_val)
        extend(in_switches[p.first], s);
    }
}

int
ChipDB::find_switch(int in, int out) const
{
  std::vector<int> t;
  std::set_intersection(out_switches[out].begin(),
                        out_switches[out].end(),
                        in_switches[in].begin(),
                        in_switches[in].end(),
                        std::back_insert_iterator<std::vector<int>>(t));
  assert(t.size() == 1);
  int s = t[0];
  assert(switches[s].out == out);
  assert(contains_key(switches[s].in_val, in));
  return s;
}

void
ChipDB::bwrite(pnr::obstream &obs) const
{
  std::vector<std::string> net_names;
  std::map<std::string, int> net_name_idx;
  
  std::vector<std::map<int, int>> tile_nets_idx(n_tiles);
  for (int t = 0; t < n_tiles; ++t)
    {
      for (const auto &p : tile_nets[t])
        {
          int ni;
          auto i = net_name_idx.find(p.first);
          if (i == net_name_idx.end())
            {
              ni = net_name_idx.size();
              net_names.push_back(p.first);
              net_name_idx.insert(std::make_pair(p.first, ni));
            }
          else
            ni = i->second;
          extend(tile_nets_idx[t], ni, p.second);
        }
    }
  
  obs << device
      << width
      << height
    // n_tiles = width * height
      << n_nets
    // n_global_nets = 8
      << packages
      << loc_pin_glb_num
      << iolatch
      << ieren
      << extra_bits
      << gbufin
      << tile_colbuf_tile
      << tile_type
    // net_tile_name
      << net_names
      << tile_nets_idx // tile_nets
      << tile_nonrouting_cbits
      << n_cells
      << cell_type
      << cell_location
      << cell_mfvs
      << cell_type_cells
    // bank_cells
      << switches
    // in_switches, out_switches
      << tile_cbits_block_size;
}

void
ChipDB::bread(pnr::ibstream &ibs)
{
  std::vector<std::string> net_names;
  std::vector<std::map<int, int>> tile_nets_idx;
  
  ibs >> device
      >> width
      >> height
    // n_tiles = width * height
      >> n_nets
    // n_global_nets = 8
      >> packages
      >> loc_pin_glb_num
      >> iolatch
      >> ieren
      >> extra_bits
      >> gbufin
      >> tile_colbuf_tile
      >> tile_type
    // net_tile_name
      >> net_names
      >> tile_nets_idx // tile_nets
      >> tile_nonrouting_cbits
      >> n_cells
      >> cell_type
      >> cell_location
      >> cell_mfvs
      >> cell_type_cells
    // bank_cells
      >> switches
    // in_switches, out_switches
      >> tile_cbits_block_size;
  
  n_tiles = width * height;
  
  tile_nets_idx.resize(n_tiles);
  tile_nets.resize(n_tiles);
  for (int i = 0; i < n_tiles; ++i)
    {
      for (const auto &p : tile_nets_idx[i])
        extend(tile_nets[i], net_names[p.first], p.second);
    }
  
  finalize();
}

ChipDB *
read_chipdb(const std::string &filename)
{
  std::string expanded = expand_filename(filename);
  std::ifstream ifs(expanded, std::ifstream::in | std::ifstream::binary);
  if (ifs.fail())
    fatal(fmt("read_chipdb: failed to open `" << expanded << "': "
              << strerror(errno)));
  ChipDB *chipdb;
  if (is_suffix(expanded, ".bin"))
    {
      chipdb = new ChipDB;
      pnr::ibstream ibs(ifs);
      chipdb->bread(ibs);
    }
  else
    {
      ChipDBParser parser(filename, ifs);
      chipdb = parser.parse();
    }
  return chipdb;
}

std::string
cell_type_name(CellType ct)
{
  switch(ct)
    {
    case CellType::LOGIC:  return "LC";
    case CellType::IO:  return "IO";
    case CellType::GB:  return "GB";
    case CellType::RAM:  return "RAM";
    case CellType::WARMBOOT:  return "WARMBOOT";
    case CellType::PLL:  return "PLL";
    default:  abort();
    }
}

configuration::Bit
ChipDB::extra_cell_cbit(int c, const std::string &name) const
{
  const auto &p = cell_mfvs.at(c).at(name);
  const auto &cbits = tile_nonrouting_cbits.at(tile_type[p.first]).at(std::string("PLL.") + p.second);
  assert(cbits.size() == 1);
  const configuration::Bit &cbit0 = cbits[0];
  return cbit0.with_tile(p.first);
}

}
}
