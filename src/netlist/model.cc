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

#include "model.hh"

#include "casting.hh"
#include "design.hh"
#include "instance.hh"
#include "models.hh"
#include "port.hh"

namespace pnr {
namespace netlist {

int Model::counter = 0;

Model::Model(Design *d, const std::string &n)
  : Node(Node::Kind::model),
    m_name(n)
{
  if (contains(d->m_models, n)) {
    std::ostringstream s;
    s << "model name \"" << n << "\" conflicts with another defined model";
    fatal(s.str());
  }
  extend(d->m_models, n, this);
}

Model::~Model()
{
  for (Instance *inst : m_instances)
    delete inst;
  m_instances.clear();
  
  // disconnect ports before deleting nets
  for (Port *p : m_ordered_ports)
    p->disconnect();
  
  for (const auto &p : m_nets)
    delete p.second;
  m_nets.clear();
}

Net *
Model::find_net(const std::string &n)
{
  return lookup_or_default(m_nets, n, (Net *)nullptr);
}

Net *
Model::find_or_add_net(const std::string &n)
{
  assert(!n.empty());
  return lookup_or_create(m_nets, n, [&n]() { return new Net(n); });
}

Net *
Model::add_net()
{
 L:
  std::string net_name = fmt("$temp$" << counter);
  ++counter;
  if (contains_key(m_nets, net_name))
    goto L;
  
  Net *new_n = new Net(net_name);
  extend(m_nets, net_name, new_n);
  return new_n;
}

Net *
Model::add_net(const std::string &orig)
{
  int i = 2;
  std::string net_name = orig;
 L:
  if (contains_key(m_nets, net_name))
    {
      net_name = fmt(orig << "$" << i);
      ++i;
      goto L;
    }
  
  Net *new_n = new Net(net_name);
  extend(m_nets, net_name, new_n);
  return new_n;
}

void
Model::remove_net(Net *n)
{
  assert(n->connections().empty());
  m_nets.erase(n->name());
}

Instance *
Model::add_instance(Model *inst_of)
{
  Instance *new_inst = new Instance(this, inst_of);
  m_instances.insert(new_inst);
  return new_inst;
}

std::set<Net *, IdLess>
Model::boundary_nets(const Design *d) const
{
  Models models(d);
  std::set<Net *, IdLess> bnets;
  for (Port *p : m_ordered_ports)
    {
      Net *n = p->connection();
      if (n)
        {
          Port *q = p->connection_other_port();
          if (q
              && isa<Instance>(q->node())
              && ((models.is_ioX(cast<Instance>(q->node()))
                   && q->name() == "PACKAGE_PIN")
                  || (models.is_pllX(cast<Instance>(q->node()))
                      && q->name() == "PACKAGEPIN")))
            extend(bnets, n);
        }
    }
  return bnets;
}

std::pair<std::vector<Net *>, std::map<Net *, int, IdLess>>
Model::index_nets() const
{
  int n_nets = 0;
  std::vector<Net *> vnets;
  std::map<Net *, int, IdLess> net_idx;
  vnets.push_back(nullptr);
  ++n_nets;
  for (const auto &p : m_nets)
    {
      Net *n = p.second;
      vnets.push_back(n);
      extend(net_idx, n, n_nets);
      ++n_nets;
    }
  return std::make_pair(vnets, net_idx);
}

std::pair<std::vector<Net *>, std::map<Net *, int, IdLess>>
Model::index_internal_nets(const Design *d) const
{
  std::set<Net *, IdLess> bnets = boundary_nets(d);
  
  std::vector<Net *> vnets;
  std::map<Net *, int, IdLess> net_idx;
  
  int n_nets = 0;
  for (const auto &p : m_nets)
    {
      Net *n = p.second;
      if (contains(bnets, n))
        continue;
      
      vnets.push_back(n);
      extend(net_idx, n, n_nets);
      ++n_nets;
    }
  return std::make_pair(vnets, net_idx);
}

std::pair<BasedVector<Instance *, 1>, std::map<Instance *, int, IdLess>>
Model::index_instances() const
{
  BasedVector<Instance *, 1> gates;
  std::map<Instance *, int, IdLess> gate_idx;
  
  int n_gates = 0;
  for (Instance *inst : m_instances)
    {
      ++n_gates;
      gates.push_back(inst);
      extend(gate_idx, inst, n_gates);
    }
  return std::make_pair(gates, gate_idx);
}

void
Model::prune()
{
  for (auto i = m_nets.begin(); i != m_nets.end();)
    {
      Net *n = i->second;
      auto t = i;
      ++i;
      
      int n_distinct = n->connections().size();
      bool driver = false,
        input = false;
      if (n->is_constant())
        {
          driver = true;
          ++n_distinct;
        }
      for (Port *p : n->connections())
        {
          if (p->is_input()
              || p->is_bidir())
            input = true;
          if (p->is_output()
              || p->is_bidir())
            driver = true;
          if (input && driver)
            break;
        }
      
      if (input && driver && n_distinct > 1)
        continue;
      
      // remove n
      for (auto j = n->connections().begin();
           j != n->connections().end();)
        {
          Port *p = *j;
          ++j;
          p->disconnect();
        }
      m_nets.erase(t);
      delete n;
    }
}

void
Model::rename_net(Net *n, const std::string &new_name)
{
  const std::string &old_name = n->name();
  
  int i = 2;
  std::string net_name = new_name;
 L:
  if (contains(m_nets, net_name)
      || net_name == old_name)
    {
      net_name = fmt(new_name << "$" << i);
      ++i;
      goto L;
    }
  
  m_nets.erase(old_name);
  
  n->m_name = net_name;
  extend(m_nets, net_name, n);
}

#ifndef NDEBUG
void
Model::check(const Design *d) const
{
  Models models(d);
  
  for (Port *p : m_ordered_ports)
    {
      if (p->is_bidir())
        {
          Net *n = p->connection();
          if (n)
            {
              Port *q = p->connection_other_port();
              assert (q
                      && isa<Instance>(q->node())
                      && ((models.is_ioX(cast<Instance>(q->node()))
                           && q->name() == "PACKAGE_PIN")
                          || (models.is_pllX(cast<Instance>(q->node()))
                              && q->name() == "PACKAGEPIN")));
            }
        }
    }
  
  std::set<Net *, IdLess> bnets = boundary_nets(d);
  
  for (const auto &p : m_nets)
    {
      Net *n = p.second;
      assert(p.first == n->name());
      assert(!n->connections().empty());
      
      if (contains(bnets, n))
        continue;
      
      int n_drivers = 0;
      bool input = false;
      if (n->is_constant())
        ++n_drivers;
      for (Port *p2 : n->connections())
        {
          assert(!p2->is_bidir());
          if (p2->is_input())
            input = true;
          if (p2->is_output())
            ++n_drivers;
        }
      
      assert(n_drivers == 1 && input);
    }
}
#endif

std::pair<std::map<Net *, std::string, IdLess>,
          std::set<Net *, IdLess>>
Model::shared_names() const
{
  std::set<std::string> names;
  std::map<Net *, std::string, IdLess> net_name;
  std::set<Net *, IdLess> is_port;
  for (Port *p : m_ordered_ports)
    {
      Net *n = p->connection();
      extend(names, p->name());
      if (n
          && n->name() == p->name())
        {
          extend(net_name, n, p->name());
          extend(is_port, n);
        }
    }
  for (const auto &p : m_nets)
    {
      if (contains(is_port, p.second))
        continue;
      
      int i = 2;
      std::string shared_net_name = p.first;
    L:
      if (contains(names, shared_net_name))
        {
          shared_net_name = fmt(p.first << "$" << i);
          ++i;
          goto L;
        }
      extend(names, shared_net_name);
      extend(net_name, p.second, shared_net_name);
    }
  return std::make_pair(net_name, is_port);
}

void
Model::write_blif(std::ostream &s) const
{
  s << ".model " << m_name << "\n";
  
  s << ".inputs";
  for (Port *p : m_ordered_ports)
    {
      if (p->direction() == Direction::IN
          || p->direction() == Direction::INOUT)
        s << " " << p->name();
    }
  s << "\n";
  
  s << ".outputs";
  for (Port *p : m_ordered_ports)
    {
      if (p->direction() == Direction::OUT
          || p->direction() == Direction::INOUT)
        s << " " << p->name();
    }
  s << "\n";
  
  std::map<Net *, std::string, IdLess> net_name;
  std::set<Net *, IdLess> is_port;
  std::tie(net_name, is_port) = shared_names();
  
  for (const auto &p : net_name)
    {
      if (p.second != p.first->name())
        s << "# " << p.first->name() << " -> " << p.second << "\n";
    }
  
  for (const auto &p : m_nets)
    {
      if (p.second->is_constant())
        {
          s << ".names " << p.first << "\n";
          if (p.second->constant() == Value::ONE)
            s << "1\n";
          else
            assert(p.second->constant() == Value::ZERO);
        }
    }
  
  for (auto i : m_instances)
    i->write_blif(s, net_name);
  
  for (Port *p : m_ordered_ports)
    {
      Net *n = p->connection();
      if (n
          && n->name() != p->name())
        {
          if (p->is_input())
            s << ".names " << net_name.at(n) << " " << p->name() << "\n";
          else
            {
              assert(p->is_output());
              s << ".names " << p->name() << " " << net_name.at(n) << "\n";
            }
          s << "1 1\n";
        }
    }
  
  s << ".end\n";
}

void
Model::write_verilog(std::ostream &s) const
{
  s << "module ";
  write_verilog_name(s, m_name);
  s << "(";
  bool first = true;
  for (Port *p : m_ordered_ports)
    {
      if (first)
        first = false;
      else
        s << ", ";
      switch(p->direction())
        {
        case Direction::IN:
          s << "input ";
          break;
        case Direction::OUT:
          s << "output ";
          break;
        case Direction::INOUT:
          s << "inout ";
          break;
        }
      write_verilog_name(s, p->name());
    }
  s << ");\n";
  
  std::map<Net *, std::string, IdLess> net_name;
  std::set<Net *, IdLess> is_port;
  std::tie(net_name, is_port) = shared_names();
  
  for (const auto &p : net_name)
    {
      if (p.second != p.first->name())
        s << "  // " << p.first->name() << " -> " << p.second << "\n";
    }
  
  for (const auto &p : m_nets)
    {
      if (contains(is_port, p.second))
        continue;
      
      s << "  wire ";
      write_verilog_name(s, net_name.at(p.second));
      if (p.second->is_constant())
        {
          s << " = ";
          if (p.second->constant() == Value::ONE)
            s << "1";
          else
            {
              assert(p.second->constant() == Value::ZERO);
              s << "0";
            }
        }
      s << ";\n";
    }
  
  for (Port *p : m_ordered_ports)
    {
      Net *n = p->connection();
      if (n
          && n->name() != p->name())
        {
          if (p->is_input())
            {
              s << "  assign ";
              write_verilog_name(s, net_name.at(n));
              s << " = " << p->name() << ";\n";
            }
          else
            {
              assert(p->is_output());
              s << "  assign " << p->name() << " = ";
              write_verilog_name(s, net_name.at(n));
              s << ";\n";
            }
        }
      else
        assert(contains(is_port, n));
    }
  
  int k = 0;
  for (Instance *inst : m_instances)
    {
      inst->write_verilog(s, net_name, fmt("$inst" << k));
      ++k;
    }
  
  s << "endmodule\n";
}

}
}
