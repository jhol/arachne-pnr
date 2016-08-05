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

#ifndef PNR_NETLIST_HH
#define PNR_NETLIST_HH

#include "const.hh"
#include "direction.hh"
#include "identified.hh"
#include "value.hh"
#include "vector.hh"

#include <map>
#include <set>
#include <string>
#include <vector>

class Port;
class Node;
class Model;
class Design;

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

class Instance : public Node
{
  Model *m_parent;
  Model *m_instance_of;
  
  std::map<std::string, Const> m_params;
  std::map<std::string, Const> m_attrs;
  
public:
  static const Kind kindof = Kind::instance;
  
  Model *parent() const { return m_parent; }
  Model *instance_of() const { return m_instance_of; }
  const std::map<std::string, Const> &attrs() const { return m_attrs; }
  const std::map<std::string, Const> &params() const { return m_params; }
  
  Instance(Model *p, Model *inst_of);
  
  void set_attr(const std::string &an, const Const &val) { m_attrs[an] = val; }
  
  bool has_attr(const std::string &an) const
  {
    return contains_key(m_attrs, an);
  }
  
  const Const &get_attr(const std::string &an) const
  {
    return m_attrs.at(an);
  }
  
  void merge_attrs(const Instance *inst);
  
  void set_param(const std::string &pn, const Const &val) { m_params[pn] = val; }
  
  bool has_param(const std::string &pn) const;
  const Const &get_param(const std::string &pn) const;
  
  bool self_has_param(const std::string &pn) const { return contains_key(m_params, pn); }
  const Const &
  self_get_param(const std::string &pn) const
  {
    return m_params.at(pn);
  }
  
  void remove();
  
  void dump() const;
  void write_blif(std::ostream &s,
                  const std::map<Net *, std::string, IdLess> &net_name) const;
  void write_verilog(std::ostream &s,
                     const std::map<Net *, std::string, IdLess> &net_name,
                     const std::string &inst_name) const;
};

class Model : public Node
{
  friend class Instance;
  
  static int counter;
  
  std::string m_name;
  std::map<std::string, Net *> m_nets;
  std::set<Instance *, IdLess> m_instances;
  
  std::map<std::string, Const> m_params;
  
public:
  static const Kind kindof = Kind::model;
  
  const std::string &name() const { return m_name; }
  
  const std::set<Instance *, IdLess> &instances() const { return m_instances; }
  const std::map<std::string, Net *> &nets() const { return m_nets; }
  const std::map<std::string, Const> &params() const { return m_params; }
  
  Model(Design *d, const std::string &n);
  ~Model();
  
  Net *find_net(const std::string &n);
  Net *find_or_add_net(const std::string &n);
  Net *add_net();
  Net *add_net(const std::string &name);
  Net *add_net(Net *orig) { return add_net(orig->name()); }
  void remove_net(Net *n);
  Instance *add_instance(Model *inst_of);
  
  void
  set_param(const std::string &pn, const std::string &val)
  {
    m_params[pn] = Const(val);
  }
  
  void
  set_param(const std::string &pn, const BitVector &val)
  {
    m_params[pn] = Const(val);
  }
  
  const Const &
  get_param(const std::string &pn)
  { 
    return m_params.at(pn);
  }
  
  bool has_param(const std::string &pn) { return contains_key(m_params, pn); }
  
  std::set<Net *, IdLess> boundary_nets(const Design *d) const;
  std::pair<std::vector<Net *>, std::map<Net *, int, IdLess>>
    index_nets() const;
  std::pair<std::vector<Net *>, std::map<Net *, int, IdLess>>
    index_internal_nets(const Design *d) const;
  
  std::pair<BasedVector<Instance *, 1>, std::map<Instance *, int, IdLess>>
    index_instances() const;
  
  void prune();

  std::pair<std::map<Net *, std::string, IdLess>,
            std::set<Net *, IdLess>>
    shared_names() const;
  void write_verilog(std::ostream &s) const;
  void write_blif(std::ostream &s) const;
  void rename_net(Net *n, const std::string &new_name);
#ifndef NDEBUG
  void check(const Design *d) const;
#endif
};

class Design
{
  friend class Model;
  
  Model *m_top;
  std::map<std::string, Model *> m_models;
  
public:
  Model *top() const { return m_top; }
  void set_top(Model *t);
  
  Design();
  ~Design();
  
  void create_standard_models();
  Model *find_model(const std::string &n) const;
  void prune();
  void write_verilog(std::ostream &s) const;
  void write_blif(std::ostream &s) const;
  void dump() const;
#ifndef NDEBUG
  void check() const;
#endif
};

class Models
{
public:
  Model *lut4,
    *carry;
  Model *lc,
    *io,
    *gb,
    *gb_io,
    *ram,
    *ramnr,
    *ramnw,
    *ramnrnw,
    *warmboot,
    *tbuf;
  
public:
  Models(const Design *d);
  
  bool is_dff(Instance *inst) const
  {
    return is_prefix("SB_DFF", inst->instance_of()->name());
  }
  bool is_lut4(Instance *inst) const { return inst->instance_of() == lut4; }
  bool is_carry(Instance *inst) const { return inst->instance_of() == carry; }
  
  bool is_lc(Instance *inst) const { return inst->instance_of() == lc; }
  bool is_io(Instance *inst) const { return inst->instance_of() == io; }
  bool is_gb(Instance *inst) const { return inst->instance_of() == gb; }
  bool is_gb_io(const Instance *inst) const { return inst->instance_of() == gb_io; }
  bool is_ioX(const Instance *inst) const
  {
    return inst->instance_of() == io
      || inst->instance_of() == gb_io;
  }
  bool is_gbX(const Instance *inst) const
  {
    return inst->instance_of() == gb
      || inst->instance_of() == gb_io;
  }
  
  bool is_ram(Instance *inst) const { return inst->instance_of() == ram; }
  bool is_ramnr(Instance *inst) const { return inst->instance_of() == ramnr; }
  bool is_ramnw(Instance *inst) const { return inst->instance_of() == ramnw; }
  bool is_ramnrnw(Instance *inst) const { return inst->instance_of() == ramnrnw; }
  bool is_warmboot(Instance *inst) const { return inst->instance_of() == warmboot; }
  bool is_tbuf(Instance *inst) const { return inst->instance_of() == tbuf; }
  
  bool is_ramX(Instance *inst) const
  { 
    return (inst->instance_of() == ram
            || inst->instance_of() == ramnr
            || inst->instance_of() == ramnw
            || inst->instance_of() == ramnrnw);
  }
  bool is_pllX(const Instance *inst) const
  {
    return is_prefix("SB_PLL40_", inst->instance_of()->name());
  }
};


#endif
