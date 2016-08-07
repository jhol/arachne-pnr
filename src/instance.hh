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

#ifndef PNR_INSTANCE_HH
#define PNR_INSTANCE_HH

#include "const.hh"
#include "port.hh"
#include "node.hh"

#include <map>
#include <string>

class Model;

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

#endif
