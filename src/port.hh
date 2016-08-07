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

#ifndef PNR_PORT_HH
#define PNR_PORT_HH

#include "direction.hh"
#include "identified.hh"
#include "value.hh"

#include <cassert>
#include <string>

namespace pnr {

class Net;
class Node;

class Port : public Identified
{
  Node *m_node;
  std::string m_name;
  Direction m_dir;
  Value m_undriven;
  Net *m_connection;
  
public:
  Node *node() const { return m_node; }
  const std::string &name() const { return m_name; }
  Direction direction() const { return m_dir; }
  void set_direction(Direction dir) { m_dir = dir; }
  Value undriven() const { return m_undriven; }
  void set_undriven(Value u) { m_undriven = u; }
  
  Port(Node *node_, const std::string &name_)
    : m_node(node_), m_name(name_), m_dir(Direction::IN), m_undriven(Value::X), m_connection(nullptr)
  {}
  Port(Node *node_, const std::string &name_, Direction dir)
    : m_node(node_), m_name(name_), m_dir(dir), m_undriven(Value::X), m_connection(nullptr)
  {}
  Port(Node *node_, const std::string &name_, Direction dir, Value u)
    : m_node(node_), m_name(name_), m_dir(dir), m_undriven(u), m_connection(nullptr)
  {}
  ~Port()
  {
    assert(m_connection == nullptr);
  }
  
  /* from the perspective of the body of a model */
  bool is_output() const;
  bool is_input() const;
  bool is_bidir() const { return m_dir == Direction::INOUT; }
  
  bool connected() const { return m_connection != nullptr; }
  void disconnect();
  void connect(Net *n);
  Net *connection() const { return m_connection; }
  
  Port *connection_other_port() const;
};

}

#endif
