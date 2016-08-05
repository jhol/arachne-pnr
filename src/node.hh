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

#ifndef PNR_NODE_HH
#define PNR_NODE_HH

#include "direction.hh"
#include "identified.hh"
#include "value.hh"

#include <string>
#include <vector>
#include <map>

class Port;

class Node : public Identified
{
protected:
  std::map<std::string, Port *> m_ports;
  std::vector<Port *> m_ordered_ports;
  
public:
  typedef Node Base;
  enum class Kind
  {
    model,
    instance,
  };
private:
  Kind m_kind;
  
public:
  const std::map<std::string, Port *> &ports() const { return m_ports; }
  const std::vector<Port *> &ordered_ports() const { return m_ordered_ports; }
  
  Kind kind() const { return m_kind; }
  
  Node(Kind k) : m_kind(k) {}
  ~Node();
  
  Port *add_port(Port *t);
  Port *add_port(const std::string &n, Direction dir);
  Port *add_port(const std::string &n, Direction dir, Value u);
  Port *find_port(const std::string &n);
};

#endif
