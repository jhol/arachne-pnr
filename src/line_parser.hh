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

#ifndef PNR_LINE_PARSER_HH
#define PNR_LINE_PARSER_HH

#include "lexical_position.hh"

#include <iostream>
#include <ostream>
#include <string>
#include <vector>

namespace pnr {

class LineParser
{
  std::istream &s;
  
protected:
  LexicalPosition lp;
  
  std::string line;
  std::vector<std::string> words;
  
  void fatal(const std::string &msg) const { lp.fatal(msg); }
  void warning(const std::string &msg) const { lp.warning(msg); }
  
  bool eof() { return s.eof(); }
  
  void split_line();
  void read_line();
  
  LineParser(const std::string &f, std::istream &s_)
    : s(s_), lp(f)
  {}
};

}

#endif
