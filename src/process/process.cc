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

#include "process.hh"

#include "carry.hh"
#include "casting.hh"
#include "chipdb/chipdb.hh"
#include "configuration/configuration.hh"
#include "designstate.hh"
#include "netlist/design.hh"
#include "netlist/port.hh"
#include "parse/blif.hh"
#include "parse/pcf.hh"
#include "process/constant.hh"
#include "process/global.hh"
#include "process/io.hh"
#include "process/pack.hh"
#include "process/place.hh"
#include "process/route.hh"

#include "config.h"

#include <fstream>
#include <memory>

namespace pnr {
namespace process {

void process(const std::string &input_file,
             const std::string &pcf_file,
             const std::string &output_file,
             const std::string &post_pack_blif,
             const std::string &post_pack_verilog,
             const std::string &post_place_pcf,
             const std::string &post_place_blif,
             const std::string &device,
             const std::string &package_name,
             unsigned int seed,
	     int max_passes,
             bool route_only,
	     bool promote_globals)
{
  random_generator rg(seed);
  
  *logs << "device: " << device << "\n";
  const chipdb::ChipDB chipdb(device);
  
  *logs << "  supported packages: ";
  bool first = true;
  for (const auto &p : chipdb.packages)
    {
      if (first)
        first = false;
      else
        *logs << ", ";
      *logs << p.first;
    }
  *logs << "\n";
  
  // chipdb.dump(std::cout);
  
  auto package_i = chipdb.packages.find(package_name);
  if (package_i == chipdb.packages.end())
    fatal(fmt("unknown package `" << package_name << "'"));
  const chipdb::Package &package = package_i->second;
  
#ifdef __AFL_HAVE_MANUAL_CONTROL
  __AFL_INIT();
#endif
  
  /*
  while (__AFL_LOOP(1000)) {
  */
  
  *logs << "read_blif " << (input_file.empty() ? "<stdin>" : input_file)
    << "...\n";
  netlist::Design d = input_file.empty() ?
    parse::read_blif("<stdin>", std::cin) :
    parse::read_blif(input_file);

  // d.dump();
  
  *logs << "prune...\n";
  d.prune();
#ifndef NDEBUG
  d.check();
#endif
  // d.dump();
  
  DesignState ds(chipdb, package, d);
  
  if (route_only)
    {
      for (netlist::Instance *inst : ds.top->instances())
        {
          const std::string &loc_attr = inst->get_attr("loc").as_string();
          int cell;
          if (sscanf(loc_attr.c_str(), "%d", &cell) != 1)
            fatal("parse error in loc attribute");
          extend(ds.placement, inst, cell);
        }
    }
  else
    {
      if (!pcf_file.empty())
        {
          *logs << "read_pcf " << pcf_file << "...\n";
	    parse::read_pcf(pcf_file, ds);
        }
      
      *logs << "instantiate_io...\n";
      instantiate_io(d);
#ifndef NDEBUG
      d.check();
#endif
      // d.dump();
      
      *logs << "pack...\n";
      pack(ds);
#ifndef NDEBUG
      d.check();
#endif
      // d.dump();
      
      if (!post_pack_blif.empty())
        {
          *logs << "write_blif " << post_pack_blif << "\n";
          std::string expanded = expand_filename(post_pack_blif);
          std::ofstream fs(expanded);
          if (fs.fail())
            fatal(fmt("write_blif: failed to open `" << expanded << "': "
                      << strerror(errno)));
          fs << "# " << PACKAGE_NAME " " PNR_PACKAGE_VERSION_STRING << "\n";
          d.write_blif(fs);
        }
      if (!post_pack_verilog.empty())
        {
          *logs << "write_verilog " << post_pack_verilog << "\n";
          std::string expanded = expand_filename(post_pack_verilog);
          std::ofstream fs(expanded);
          if (fs.fail())
            fatal(fmt("write_verilog: failed to open `" << expanded << "': "
                      << strerror(errno)));
          fs << "/* " << PACKAGE_NAME " " PNR_PACKAGE_VERSION_STRING << " */\n";
          d.write_verilog(fs);
        }
      
      *logs << "place_constraints...\n";
      place_constraints(ds);
#ifndef NDEBUG
      d.check();
#endif
      
      // d.dump();
      
      *logs << "promote_globals...\n";
      process::promote_globals(ds, promote_globals);
#ifndef NDEBUG
      d.check();
#endif
      // d.dump();
      
      *logs << "realize_constants...\n";
      realize_constants(chipdb, d);
#ifndef NDEBUG
      d.check();
#endif
	
      *logs << "place...\n";
      // d.dump();
      place(rg, ds);
#ifndef NDEBUG
      d.check();
#endif
      // d.dump();
      
      if (!post_place_pcf.empty())
        {
          *logs << "write_pcf " << post_place_pcf << "...\n";
          std::string expanded = expand_filename(post_place_pcf);
          std::ofstream fs(expanded);
          if (fs.fail())
            fatal(fmt("write_pcf: failed to open `" << expanded << "': "
                      << strerror(errno)));
          fs << "# " << PACKAGE_NAME " " PNR_PACKAGE_VERSION_STRING << "\n";
          for (const auto &p : ds.placement)
            {
              if (ds.models.is_io(p.first))
                {
                  const chipdb::Location &loc = chipdb.cell_location[p.second];
                  std::string pin = package.loc_pin.at(loc);
		    netlist::Port *top_port = p.first->find_port(
                    "PACKAGE_PIN")->connection_other_port();
                  assert(isa<netlist::Model>(top_port->node())
                         && cast<netlist::Model>(top_port->node()) == ds.top);
                  
                  fs << "set_io " << top_port->name() << " " << pin << "\n";
                }
            }
        }
      
      if (!post_place_blif.empty())
        {
          for (const auto &p : ds.placement)
            {
              // p.first->set_attr("loc", fmt(p.second));
              const chipdb::Location &loc = chipdb.cell_location[p.second];
              int t = loc.tile();
              int pos = loc.pos();
              p.first->set_attr("loc",
                                fmt(chipdb.tile_x(t)
                                    << "," << chipdb.tile_y(t)
                                    << "/" << pos));
            }
          
          *logs << "write_blif " << post_place_blif << "\n";
          std::string expanded = expand_filename(post_place_blif);
          std::ofstream fs(expanded);
          if (fs.fail())
            fatal(fmt("write_blif: failed to open `" << expanded << "': "
                      << strerror(errno)));
          fs << "# " << PACKAGE_NAME " " PNR_PACKAGE_VERSION_STRING << "\n";
          d.write_blif(fs);
        }
    }
  
  // d.dump();
  
  *logs << "route...\n";
  route(ds, max_passes);
#ifndef NDEBUG
  d.check();
#endif
  
  if (!output_file.empty())
    {
      *logs << "write_txt " << output_file << "...\n";
      std::string expanded = expand_filename(output_file);
      std::ofstream fs(expanded);
      if (fs.fail())
        fatal(fmt("write_txt: failed to open `" << expanded << "': "
                  << strerror(errno)));
      ds.conf.write_txt(fs, chipdb, d,
                        ds.placement, ds.cnet_net);
    }
  else
    {
      *logs << "write_txt <stdout>...\n";
      ds.conf.write_txt(std::cout, chipdb, d,
                        ds.placement, ds.cnet_net);
    }
  /*
  }
  */
}

}
}
