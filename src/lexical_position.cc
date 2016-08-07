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

#include "lexical_position.hh"

#include <iostream>

namespace pnr {

std::ostream &
operator<<(std::ostream &s, const LexicalPosition &lp)
{
  if (lp.internal)
    s << "<internal>";
  else
    s << lp.file << ":" << lp.line;
  return s;
}

void
LexicalPosition::fatal(const std::string &msg) const
{
  std::cerr << *this << ": fatal error: " << msg << "\n";
  exit(EXIT_FAILURE);
}

void
LexicalPosition::warning(const std::string &msg) const
{
  std::cerr << *this << ": warning: " << msg << "\n";
}

}
