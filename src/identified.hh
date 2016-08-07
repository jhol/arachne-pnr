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

#ifndef PNR_IDENTIFIED_HH
#define PNR_IDENTIFIED_HH

#include <functional>

namespace pnr {

class Net;
class Node;
class Instance;
class Model;

class Identified
{
private:
  template<typename T> friend struct std::hash;
  friend class IdLess;
  
  static int id_counter;
  int id;
  
public:
  Identified()
    : id(id_counter++)
  {
  }
};

class IdLess
{
public:
  bool operator()(const Identified *lhs, const Identified *rhs) const
  {
    return lhs->id < rhs->id;
  }
};

}

namespace std {
  
template<>
struct hash<pnr::Identified *>
{
public:
  size_t operator()(const pnr::Identified *x) const
  {
    std::hash<int> hasher;
    return hasher(x->id);
  }
};

template<> struct hash<pnr::Net *> : public std::hash<pnr::Identified *> {};
template<> struct hash<pnr::Node *> : public std::hash<pnr::Identified *> {};
template<> struct hash<pnr::Instance *> : public std::hash<pnr::Identified *> {};
template<> struct hash<pnr::Model *> : public std::hash<pnr::Identified *> {};

}

#endif
