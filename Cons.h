/*
 *  i7remux -- video converter for COWON iAUDIO 7 (and other products)
 *  Copyright (C) 2007 Takayuki Usui
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */
#ifndef _CONS_H
#define _CONS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

template<class ListType>
class Cons {
 public:
  ListType car;
  Cons<ListType>* cdr;

 public:
  Cons<ListType>(ListType,Cons<ListType>*);
  Cons<ListType>(void);
  ~Cons<ListType>(void);

 public:
  Cons<ListType>(const Cons<ListType>&);
  const Cons<ListType>& operator=(const Cons<ListType>&);

 public:
  int length(void);
  ListType append(ListType);
  ListType push(ListType);
  ListType pop(void);
  ListType ref(int);
  ListType last(void);
  void reverse(void);
  void swap(void);
};

#include "Cons.cc"

#endif
