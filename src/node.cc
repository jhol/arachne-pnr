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

#include "node.hh"

#include "port.hh"
#include "ullmanset.hh"

namespace pnr {

Node::~Node()
{
  for (Port *p : m_ordered_ports)
    {
      p->disconnect();
      delete p;
    }
  m_ports.clear();
  m_ordered_ports.clear();
}

Port *
Node::add_port(Port *t)
{
  Port *new_port = new Port(this, t->name(), t->direction(), t->undriven());
  extend(m_ports, new_port->name(), new_port);
  m_ordered_ports.push_back(new_port);
  return new_port;
}

Port *
Node::add_port(const std::string &n, Direction dir)
{
  Port *new_port = new Port(this, n, dir);
  extend(m_ports, new_port->name(), new_port);
  m_ordered_ports.push_back(new_port);
  return new_port;
}

Port *
Node::add_port(const std::string &n, Direction dir, Value u)
{
  Port *new_port = new Port(this, n, dir, u);
  extend(m_ports, new_port->name(), new_port);
  m_ordered_ports.push_back(new_port);
  return new_port;
}

Port *
Node::find_port(const std::string &n)
{
  return lookup_or_default(m_ports, n, nullptr);
}

void
Node::write_verilog_name(std::ostream &s, const std::string &name)
{
  bool quote = false;
  for (char ch : name)
    {
      if (! (isalnum(ch)
             || ch == '_'
             || ch == '$'))
        {
          quote = true;
          break;
        }
    }
  if (quote)
    s << '\\';
  s << name;
  if (quote)
    s << ' ';
}

}
