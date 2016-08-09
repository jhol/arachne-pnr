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

#ifndef PNR_NETLIST_CONST_HH
#define PNR_NETLIST_CONST_HH

#include "util.hh"
#include "bitvector.hh"
#include "lexical_position.hh"

#include <ostream>
#include <string>

namespace pnr {
namespace netlist {

// for parameters, attributes
class Const
{
private:
  friend std::ostream &operator<<(std::ostream &s, const Const &c);
  
  LexicalPosition m_lp;
  bool m_is_bits;
  std::string m_strval;
  BitVector m_bitval;

public:
  const LexicalPosition &lexpos() const { return m_lp; }
  
public:
  Const() : m_is_bits(false) {}
  Const(const std::string &sv) 
    : m_is_bits(false), m_strval(sv)
  {}
  Const(const LexicalPosition &lp, const std::string &sv) 
    : m_lp(lp), m_is_bits(false), m_strval(sv)
  {}
  Const(const BitVector &bv) 
    : m_is_bits(true), m_bitval(bv)
  {}
  Const(const LexicalPosition &lp, const BitVector &bv) 
    : m_lp(lp), m_is_bits(true), m_bitval(bv)
  {}
  
  const std::string &as_string() const
  {
    if (m_is_bits)
      m_lp.fatal("expected string constant");
    return m_strval;
  }
  
  const BitVector &as_bits() const
  {
    if (!m_is_bits)
      m_lp.fatal("expected integer constant");
    return m_bitval;
  }
  
  bool get_bit(int i) const
  {
    if (!m_is_bits)
      m_lp.fatal("expected integer constant");
    if (i >= (int)m_bitval.size())
      return 0;
    else
      return m_bitval[i];
  }
  
  void write_verilog(std::ostream &s) const;
};

}
}

#endif
