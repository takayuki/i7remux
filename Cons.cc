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
template<class ListType>
Cons<ListType>::Cons(ListType car,Cons<ListType>* cdr)
{
  this->car = car;
  this->cdr = cdr;
}

template<class ListType>
Cons<ListType>::Cons(void)
{
  this->car = 0;
  this->cdr = 0;
}

template<class ListType>
Cons<ListType>::~Cons(void)
{
}

template<class ListType>
Cons<ListType>::Cons(const Cons<ListType>& src)
{
  this->car = src.car;
  this->cdr = src.cdr;
}

template<class ListType>
const Cons<ListType>&
Cons<ListType>::operator=(const Cons<ListType>& src)
{
  this->car = src.car;
  this->cdr = src.cdr;
  return *this;
}

template<class ListType>
int
Cons<ListType>::length(void)
{
  Cons<ListType>* hd = this;
  Cons<ListType>* tl = this->cdr;
  int cnt = 0;

  while (tl) {
    hd = tl; tl = hd->cdr; cnt++;
  }
  return cnt;
}

template<class ListType>
ListType
Cons<ListType>::append(ListType ent)
{
  Cons<ListType>* hd = this;
  Cons<ListType>* tl = this->cdr;

  while (tl) {
    hd = tl; tl = hd->cdr;
  }
  hd->cdr = new Cons<ListType>(ent,0);
  return ent;
}

template<class ListType>
ListType
Cons<ListType>::push(ListType ent)
{
  this->cdr = new Cons<ListType>(ent,this->cdr);
  return ent;
}

template<class ListType>
ListType
Cons<ListType>::pop(void)
{
  if (this->cdr == 0) {
    return (ListType)0;
  } else {
    ListType ent = this->cdr->car;
    this->cdr = this->cdr->cdr;
    return ent;
  }
}

template<class ListType>
ListType
Cons<ListType>::ref(int idx)
{
  Cons<ListType>* hd = this;
  Cons<ListType>* tl = this->cdr;
  int cnt = 0;

  while (tl) {
    hd = tl; tl = hd->cdr;
    if (cnt == idx)
      return hd->car;
    cnt++;
  }
  return 0;
}

template<class ListType>
ListType
Cons<ListType>::last(void)
{
  Cons<ListType>* hd = this;
  Cons<ListType>* tl = this->cdr;

  while (tl) {
    hd = tl; tl = hd->cdr;
  }
  return hd->car;
}

template<class ListType>
void
Cons<ListType>::reverse(void)
{
  Cons<ListType> newhead;
  Cons<ListType>* hd = this;
  Cons<ListType>* tl = this->cdr;

  while (tl) {
    hd = tl; tl = hd->cdr;
    newhead.push(hd->car);
  }
  this->cdr = newhead.cdr;
  return;
}

template<class ListType>
void
Cons<ListType>::swap(void)
{
  Cons<ListType>* tl = this->cdr;
  ListType tmp;

  if (tl && tl->cdr && tl->cdr->cdr) {
    tmp = tl->car;
    tl->car = tl->cdr->car;
    tl->cdr->car = tmp;
  }
  return;
}
