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

#ifndef PNR_CHIPDB_CHIPDB_HH
#define PNR_CHIPDB_CHIPDB_HH

#include "configuration/bit.hh"
#include "configuration/bitval.hh"
#include "switch.hh"
#include "celltype.hh"
#include "tiletype.hh"
#include "package.hh"
#include "location.hh"
#include "vector.hh"

#include <cassert>
#include <map>
#include <ostream>
#include <set>
#include <string>
#include <vector>

namespace pnr {
namespace chipdb {

class ChipDB
{
public:
  std::string device;
  
  int width;
  int height;
  
  int n_tiles;
  int n_nets;
  int n_global_nets;
  std::map<int, int> net_global;
  
  std::map<std::string, Package> packages;
  
  std::map<Location, int> loc_pin_glb_num;
  
  std::vector<int> iolatch;  // tiles
  std::map<Location, Location> ieren;
  std::map<std::string, std::tuple<int, int, int>> extra_bits;
  
  std::map<std::pair<int, int>, int> gbufin;
  
  std::map<int, int> tile_colbuf_tile;
  
  std::vector<TileType> tile_type;
  std::vector<std::pair<int, std::string>> net_tile_name;
  std::vector<std::map<std::string, int>> tile_nets;
  
  std::map<TileType,
          std::map<std::string, std::vector<configuration::Bit>>>
    tile_nonrouting_cbits;
  
  configuration::Bit extra_cell_cbit(int ec, const std::string &name) const;
  
  int n_cells;
  BasedVector<CellType, 1> cell_type;
  BasedVector<Location, 1> cell_location;
  std::map<int, std::map<std::string, std::pair<int, std::string>>>
    cell_mfvs;
  
  std::vector<std::vector<int>> tile_pos_cell;
  int loc_cell(const Location &loc) const 
  {
    return tile_pos_cell[loc.tile()][loc.pos()];
  }
  
  std::vector<std::vector<int>> cell_type_cells;
  
  std::vector<std::vector<int>> bank_cells;
  
  // buffers and routing
  std::vector<Switch> switches;
  
  std::vector<std::set<int>> out_switches;
  std::vector<std::set<int>> in_switches;
  
  std::map<TileType, std::pair<int, int>> tile_cbits_block_size;
  
  int add_cell(CellType type, const Location &loc);
  bool is_global_net(int i) const { return i < n_global_nets; }
  int find_switch(int in, int out) const;
  
  int tile(int x, int y) const
  {
    assert(x >= 0 && x < width);
    assert(y >= 0 && y < height);
  
    return x + width*y;
  }
  
  int tile_x(int t) const
  {
    assert(t >= 0 && t <= n_tiles);
    return t % width;
  }

  int tile_y(int t) const
  {
    assert(t >= 0 && t <= n_tiles);
    return t / width;
  }
  
  int tile_bank(int t) const;
  int ramt_ramb_tile(int ramt_t) const
  {
    assert(tile_type[ramt_t] == TileType::RAMT);
    
    int ramb_t = ramt_t - width;
    assert(ramb_t == tile(tile_x(ramt_t),
                          tile_y(ramt_t)-1));
    assert(tile_type[ramb_t] == TileType::RAMB);
    return ramb_t;
  }
  
  void set_device(const std::string &d, int w, int h, int n_nets_);
  void finalize();
  
public:
  explicit ChipDB();
  ChipDB(const std::string &device_);
  
  void dump(std::ostream &s) const;
  void bwrite(pnr::obstream &obs) const;
  void bread(pnr::ibstream &ibs);
};

}
}

#endif
