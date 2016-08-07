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

#ifndef PNR_DESIGN_HH
#define PNR_DESIGN_HH

#include <map>
#include <string>

namespace pnr {

class Model;

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

}

#endif
