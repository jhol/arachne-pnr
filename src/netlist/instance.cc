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

#include "instance.hh"
#include "model.hh"

namespace pnr {
namespace netlist {

Instance::Instance(Model *parent_, Model *inst_of)
  : Node(Node::Kind::instance),
    m_parent(parent_),
    m_instance_of(inst_of)
{
  for (Port *p : m_instance_of->m_ordered_ports)
    add_port(p);
}

void
Instance::merge_attrs(const Instance *inst)
{
  auto i = inst->m_attrs.find("src");
  if (i != inst->m_attrs.end())
    {
      auto j = m_attrs.find("src");
      if (j != m_attrs.end())
        j->second = Const(j->second.as_string() + "|" + i->second.as_string());
      else
        m_attrs.insert(*i);
    }
}

bool
Instance::has_param(const std::string &pn) const
{ 
  return (contains_key(m_params, pn)
          || m_instance_of->has_param(pn));
}

const Const &
Instance::get_param(const std::string &pn) const
{
  auto i = m_params.find(pn);
  if (i == m_params.end())
    return m_instance_of->get_param(pn);  // default
  else
    return i->second;
}

void
Instance::remove()
{
  m_parent->m_instances.erase(this);
  for (const auto &p : ports())
    p.second->disconnect();
}

void
Instance::write_blif(std::ostream &s,
                     const std::map<Net *, std::string, IdLess> &net_name) const
{
  s << ".gate " << m_instance_of->name();
  for (Port *p : m_ordered_ports)
    {
      s << " " << p->name() << "=";
      if (p->connected())
        s << net_name.at(p->connection());
    }
  s << "\n";
  
  for (const auto &p : m_attrs)
    s << ".attr " << p.first << " " << p.second << "\n";
  for (const auto &p : m_params)
    s << ".param " << p.first << " " << p.second << "\n";
}

void
Instance::dump() const
{
  *logs << ".gate " << m_instance_of->name();
  for (Port *p : m_ordered_ports)
    {
      *logs << " " << p->name() << "=";
      if (p->connected())
        *logs << p->connection()->name();
    }
  *logs << " # " << this << "\n";
  
  for (const auto &p : m_attrs)
    *logs << ".attr " << p.first << " " << p.second << "\n";
  for (const auto &p : m_params)
    *logs << ".param " << p.first << " " << p.second << "\n";
}

void
Instance::write_verilog(std::ostream &s,
                        const std::map<Net *, std::string, IdLess> &net_name,
                        const std::string &inst_name) const
{
  if (!m_attrs.empty())
    {
      s << "  (* ";
      bool first = true;
      for (const auto &p : m_attrs)
        {
          if (first)
            first = false;
          else
            s << ", ";
          s << p.first << "=";
          p.second.write_verilog(s);
        }
      s << " *)\n";
    }
  
  s << "  ";
  write_verilog_name(s, m_instance_of->name());
  
  if (!m_params.empty())
    {
      s << " #(";
      bool first = true;
      for (const auto &p : m_params)
        {
          if (first)
            first = false;
          else
            s << ", ";
              
          s << "\n    .";
          write_verilog_name(s, p.first);
          s << "(";
          p.second.write_verilog(s);
          s << ")";
        }
      s << "\n  ) ";
    }
  
  write_verilog_name(s, inst_name);
  s << " (";
  bool first = true;
  for (Port *p : m_ordered_ports)
    {
      Net *conn = p->connection();
      if (conn)
        {
          if (first)
            first = false;
          else
            s << ",";
          s << "\n    .";
          write_verilog_name(s, p->name());
          s << "(";
          write_verilog_name(s, conn->name());
          s << ")";
        }
    }
  s << "\n  );\n";
}

}
}
