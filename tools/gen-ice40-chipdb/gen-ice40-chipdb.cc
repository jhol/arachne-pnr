/* Copyright (C) 2016 Joel Holdsworth
   
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

#include "chipdb/chipdb.hh"
#include "chipdbparser.hh"
#include "util.hh"

#include <fstream>
#include <memory>

const char *program_name;

void
usage()
{
  std::cout <<
    "Usage:\n"
    "  "  << program_name << " <input-file> <output-file>\n"
    "\n";
}

int
main(int argc, const char **argv)
{
  using std::string;

  program_name = argv[0];
	
  if (argc != 3) {
    usage();
    return EXIT_FAILURE;
  }

  const char *chipdb_file = argv[1];
  const char *binary_chipdb = argv[2];

  std::ifstream ifs(chipdb_file, std::ifstream::in | std::ifstream::binary);
  if (ifs.fail())
    fatal(fmt("Failed to open `" << chipdb_file << "': "
              << strerror(errno)));

  const std::unique_ptr<pnr::chipdb::ChipDB> chipdb(
    ChipDBParser(chipdb_file, ifs).parse());

  std::ofstream ofs(binary_chipdb, std::ofstream::out | std::ofstream::binary);
  if (ofs.fail())
    fatal(fmt("Failed to open `" << binary_chipdb << "': "
              << strerror(errno)));
  pnr::obstream obs(ofs);
  chipdb->bwrite(obs);
  
  return EXIT_SUCCESS;
}
