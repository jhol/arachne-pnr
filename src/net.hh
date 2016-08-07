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

#ifndef PNR_NET_HH
#define PNR_NET_HH

#include "identified.hh"
#include "value.hh"

#include <cassert>
#include <string>
#include <set>

namespace pnr {

class Port;

class Net : public Identified
{
  friend class Port;
  friend class Model;
  
  std::string m_name;
  bool m_is_constant;
  Value m_constant;
  
  std::set<Port *, IdLess> m_connections;
  
public:
  const std::string &name() const { return m_name; }
  
  bool is_constant() const { return m_is_constant; }
  void set_is_constant(bool c) { m_is_constant = c; }
  Value constant() const { return m_constant; }
  void set_constant(Value c) { m_constant = c; }
  
  const std::set<Port *, IdLess> &connections() const { return m_connections; }
  
  Net(const std::string &n)
    : m_name(n), m_is_constant(false), m_constant(Value::X)
  {
  }
  
  ~Net()
  {
    assert(m_connections.empty());
  }
  
  void replace(Net *new_n);
};

}

#endif
