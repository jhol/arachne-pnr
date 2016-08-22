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

#include "process/process.hh"

#include "util.hh"

#include "config.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>

const char *program_name;

namespace pnr {

class null_streambuf : public std::streambuf
{
public:
  int overflow(int c) override { return c; }  
};

void
usage()
{
  std::cout <<
    "Usage:\n"
    "\n"
    "  " << program_name << " [options] [input-file]\n" <<
    "\n"
    "Place and route netlist.  Input file is in BLIF format.  Output is\n"
    "(text) bitstream.\n"
    "\n"
    "    -h, --help\n"
    "        Print this usage message.\n"
    "\n"
    "    -q, --quiet\n"
    "        Run quite.  Don't output progress messages.\n"
    "\n"
    "    -d <device>, --device <device>\n"
    "        Target device <device>.  Supported devices:\n"
    "          1k - Lattice Semiconductor iCE40LP/HX1K\n"
    "          8k - Lattice Semiconductor iCE40LP/HX8K\n"
    "        Default: 1k\n"
    "\n"
    "    -l, --no-promote-globals\n"
    "        Don't promote nets to globals.\n"
    "\n"
    "    -B <file>, --post-pack-blif <file>\n"
    "        Write post-pack netlist to <file> as BLIF.\n"
    "    -V <file>, --post-pack-verilog <file>\n"
    "        Write post-pack netlist to <file> as Verilog.\n"
    "\n"
    "    --post-place-blif <file>\n"
    "        Write post-place netlist to <file> as BLIF.\n"
    "\n"
    "    --route-only\n"
    "        Input must include placement.\n"
    "\n"
    "    -p <pcf-file>, --pcf-file <pcf-file>\n"
    "        Read physical constraints from <pcf-file>.\n"
    "\n"
    "    -P <package>, --package <package>\n"
    "        Target package <package>.\n"
    "        Default: tq144 for 1k, ct256 for 8k\n"
    "\n"
    "    -r\n"
    "        Randomize seed.\n"
    "\n"
    "    -m <int>, --max-passes <int>\n"
    "        Maximum number of routing passes.\n"
    "        Default: 200\n"
    "\n"
    "    -s <int>, --seed <int>\n"
    "        Set seed for random generator to <int>.\n"
    "        Default: 1\n"
    "\n"
    "    -w <pcf-file>, --write-pcf <pcf-file>\n"
    "        Write pin assignments to <pcf-file> after placement.\n"
    "\n"
    "    -o <output-file>, --output-file <output-file>\n"
    "        Write output to <output-file>.\n"
    "\n"
    "    -v, --version\n"
    "        Print version and exit.\n";
}

struct null_ostream : public std::ostream
{
  null_ostream() : std::ostream(0) {}
};


}

int
main(int argc, const char **argv)
{
  using namespace pnr;
  using namespace pnr::process;
	
  program_name = argv[0];
  
  bool help = false,
    quiet = false,
    promote_globals = true,
    route_only = false,
    randomize_seed = false;
  std::string device = "1k";
  const char *input_file = nullptr,
    *package_name_cp = nullptr,
    *pcf_file = nullptr,
    *post_place_pcf = nullptr,
    *post_pack_blif = nullptr,
    *post_pack_verilog = nullptr,
    *post_place_blif = nullptr,
    *output_file = nullptr,
    *seed_str = nullptr,
    *max_passes_str = nullptr;
  
  for (int i = 1; i < argc; ++i)
    {
      if (argv[i][0] == '-')
        {
          if (!strcmp(argv[i], "-h")
              || !strcmp(argv[i], "--help"))
            help = true;
          else if (!strcmp(argv[i], "-q")
              || !strcmp(argv[i], "--quiet"))
            quiet = true;
          else if (!strcmp(argv[i], "-d")
              || !strcmp(argv[i], "--device"))
            {
              if (i + 1 >= argc)
                fatal(fmt(argv[i] << ": expected argument"));
              
              ++i;
              device = argv[i];
            }
          else if (!strcmp(argv[i], "-l")
                   || !strcmp(argv[i], "--no-promote-globals"))
            promote_globals = false;
          else if (!strcmp(argv[i], "-B")
              || !strcmp(argv[i], "--post-pack-blif"))
            {
              if (i + 1 >= argc)
                fatal(fmt(argv[i] << ": expected argument"));
              
              ++i;
              post_pack_blif = argv[i];
            }
          else if (!strcmp(argv[i], "-V")
              || !strcmp(argv[i], "--post-pack-verilog"))
            {
              if (i + 1 >= argc)
                fatal(fmt(argv[i] << ": expected argument"));
              
              ++i;
              post_pack_verilog = argv[i];
            }
          else if (!strcmp(argv[i], "--post-place-blif"))
            {
              if (i + 1 >= argc)
                fatal(fmt(argv[i] << ": expected argument"));
              
              ++i;
              post_place_blif = argv[i];
            }
          else if (!strcmp(argv[i], "--route-only"))
            route_only = true;
          else if (!strcmp(argv[i], "-p")
                   || !strcmp(argv[i], "--pcf-file"))
            {
              if (i + 1 >= argc)
                fatal(fmt(argv[i] << ": expected argument"));
              
              ++i;
              pcf_file = argv[i];
            }
          else if (!strcmp(argv[i], "-P")
                   || !strcmp(argv[i], "--package"))
            {
              if (i + 1 >= argc)
                fatal(fmt(argv[i] << ": expected argument"));
              
              ++i;
              package_name_cp = argv[i];
            }
          else if (!strcmp(argv[i], "-r"))
            randomize_seed = true;
          else if (!strcmp(argv[i], "-w")
                   || !strcmp(argv[i], "--write-pcf"))
            {
              if (i + 1 >= argc)
                fatal(fmt(argv[i] << ": expected argument"));
              
              ++i;
              post_place_pcf = argv[i];
            }
          else if (!strcmp(argv[i], "-s")
                   || !strcmp(argv[i], "--seed"))
            {
              if (i + 1 >= argc)
                fatal(fmt(argv[i] << ": expected argument"));
              
              ++i;
              seed_str = argv[i];
            }
          else if (!strcmp(argv[i], "-m")
                   || !strcmp(argv[i], "--max-passes"))
            {
              if (i + 1 >= argc)
                fatal(fmt(argv[i] << ": expected argument"));
              
              ++i;
              max_passes_str = argv[i];
            }
          else if (!strcmp(argv[i], "-o")
                   || !strcmp(argv[i], "--output-file"))
            {
              if (i + 1 >= argc)
                fatal(fmt(argv[i] << ": expected argument"));
              
              ++i;
              output_file = argv[i];
            }
          else if (!strcmp(argv[i], "-v")
                   || !strcmp(argv[i], "--version"))
            {
              std::cout << PACKAGE_NAME " " PNR_PACKAGE_VERSION_STRING << "\n";
              exit(EXIT_SUCCESS);
            }
          else
            fatal(fmt("unknown option `" << argv[i] << "'"));
        }
      else
        {
          if (input_file)
            fatal("too many command-line arguments");
          else
            input_file = argv[i];
        }
    }
  
  if (help)
    {
      usage();
      exit(EXIT_SUCCESS);
    }
  
  if (device != "1k"
      && device != "8k")
    fatal(fmt("unknown device: " << device));
  
  std::string package_name;
  if (package_name_cp)
    package_name = package_name_cp;
  else if (device == "1k")
    package_name = "tq144";
  else
    {
      assert(device == "8k");
      package_name = "ct256";
    }
  
  std::unique_ptr<std::ostream> null_ostream;
  if (quiet) {
    null_ostream.reset(new std::ostream(new null_streambuf));
    logs = null_ostream.get();
  } else
    logs = &std::cerr;
  
  unsigned seed = 0;
  if (seed_str)
    {
      std::string seed_s = seed_str;
      
      if (seed_s.empty())
        fatal("invalid empty seed");
      
      for (char ch : seed_s)
        {
          if (ch >= '0'
              && ch <= '9')
            seed = seed * 10 + (unsigned)(ch - '0');
          else
            fatal(fmt("invalid character `" 
                      << ch
                      << "' in unsigned integer literal in seed"));
        }
    }
  else
    seed = 1;

  int max_passes = 0;
  if (max_passes_str)
    {
      std::string max_passes_s = max_passes_str;
      
      if (max_passes_s.empty())
        fatal("invalid empty max-passes value");
      
      for (char ch : max_passes_s)
        {
          if (ch >= '0'
              && ch <= '9')
            max_passes = max_passes * 10 + (unsigned)(ch - '0');
          else
            fatal(fmt("invalid character `" 
                      << ch
                      << "' in unsigned integer literal in max-passes value"));
        }
    }
  else
    max_passes = 200;
  
  if (randomize_seed)
    {
      std::random_device rd;
      do {
        seed = rd();
      } while (seed == 0);
    }
  
  *logs << "seed: " << seed << "\n";
  if (!seed)
    fatal("zero seed");

  process::process(input_file, pcf_file, output_file, post_pack_blif,
               post_pack_verilog, post_place_pcf, post_place_blif, device,
               package_name, seed, max_passes, promote_globals, route_only);

  return 0;
}
