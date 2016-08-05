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

#include "port.hh"

#include "casting.hh"
#include "netlist.hh"

void
Port::disconnect()
{
  if (m_connection)
    {
      m_connection->m_connections.erase(this);
      m_connection = nullptr;
    }
}

void
Port::connect(Net *n)
{
  if (m_connection)
    disconnect();
  
  assert(!m_connection);
  m_connection = n;
  if (n)
    n->m_connections.insert(this);
}

Port *
Port::connection_other_port() const
{
  Net *n = connection();
  if (!n
      || n->connections().size() != 2)
    return nullptr;
  
  auto i = n->connections().begin();
  if (*i == this)
    ++i;
  return *i;
}

bool
Port::is_output() const
{
  assert(m_node
         && (isa<Model>(m_node)
             || isa<Instance>(m_node)));
  return (isa<Instance>(m_node)
          ? m_dir == Direction::OUT
          : m_dir == Direction::IN); // model
}

bool
Port::is_input() const
{
  assert(m_node
         && (isa<Model>(m_node)
             || isa<Instance>(m_node)));
  return (isa<Instance>(m_node)
          ? m_dir == Direction::IN
          : m_dir == Direction::OUT); // model
}
