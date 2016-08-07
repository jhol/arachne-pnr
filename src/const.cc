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

#include "util.hh"
#include "model.hh"
#include "casting.hh"

#include <cassert>
#include <cstring>
#include <iostream>
#include <iomanip>

static void
write_string_escaped(std::ostream &s, const std::string &str)
{
  s << '"';
  for (char ch : str)
    {
      if (ch == '"'
          || ch == '\\')
        s << '\\' << ch;
      else if (isprint(ch))
        s << ch;
      else if (ch == '\n')
        s << "\n";
      else if (ch == '\t')
        s << "\t";
      else
        s << fmt(std::oct << std::setw(3) << std::setfill('0') << (int)ch);
    }
  s << '"';
}

std::ostream &
operator<<(std::ostream &s, const Const &c)
{
  if (c.m_is_bits)
    {
      for (int i = c.m_bitval.size() - 1; i >= 0; --i)
        s << (c.m_bitval[i] ? '1' : '0');
    }
  else
    write_string_escaped(s, c.m_strval);
  return s;
}

int Identified::id_counter = 0;

void
Const::write_verilog(std::ostream &s) const
{
  if (m_is_bits)
    {
      s << m_bitval.size()
        << "'b";
      for (int i = m_bitval.size() - 1; i >= 0; --i)
        s << (m_bitval[i] ? '1' : '0');
    }
  else
    write_string_escaped(s, m_strval);
}
