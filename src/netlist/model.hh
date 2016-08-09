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

#ifndef PNR_NETLIST_MODEL_HH
#define PNR_NETLIST_MODEL_HH

#include "const.hh"
#include "net.hh"
#include "node.hh"
#include "vector.hh"

#include <string>
#include <vector>
#include <set>
#include <map>

namespace pnr {
namespace netlist {

class Design;

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

}
}

#endif
