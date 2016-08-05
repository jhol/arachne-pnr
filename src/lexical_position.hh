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

#ifndef PNR_LEXICAL_POSITION_HH
#define PNR_LEXICAL_POSITION_HH

#include <ostream>
#include <string>

class LexicalPosition
{
public:
  friend std::ostream &operator<<(std::ostream &s, const LexicalPosition &lp);
  
  bool internal;
  std::string file;
  int line;
  
public:
  LexicalPosition()
    : internal(true), line(0)
  {}
  LexicalPosition(const std::string &f)
    : internal(false), file(f), line(0)
  {}
  LexicalPosition(const std::string &f, int n)
    : internal(false), file(f), line(n)
  {}
  
  void next_line() { ++line; }
  
  void fatal(const std::string &msg) const;
  void warning(const std::string &msg) const;
};

#endif
