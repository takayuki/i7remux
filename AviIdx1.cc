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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <string.h>
#include "AviFile.h"
#include "AviIdx1.h"

AviIdx1Ent::AviIdx1Ent(const char* id,
		       uint32_t flags,
		       uint32_t offset,
		       uint32_t size)
{
  memcpy(this->dwChunkId,id,4);
  this->dwChunkId[4] = 0;
  this->dwFlags = flags;
  this->dwOffset = offset;
  this->dwSize = size;
}

AviIdx1Ent::~AviIdx1Ent(void)
{
}

AviIdx1::AviIdx1(void)
{
}

AviIdx1::~AviIdx1(void)
{
  Cons<AviIdx1Ent*>* hd = &this->head;  
  Cons<AviIdx1Ent*>* tl = hd->cdr;
  AviIdx1Ent* car;

  while (tl) {
    hd = tl; tl = hd->cdr;
    car = hd->car;
    delete hd;
    delete car;
  }
}

int
AviIdx1::push(const char* fcc,off_t offset,uint32_t cb,int flags)
{
  if (fcc[2] == 'd') {
    this->head.push(new AviIdx1Ent(fcc,0x10,offset,cb));
  } else if (fcc[2] == 'w') {
    if (flags & OPT_AVI_IDX1WFG)
      this->head.push(new AviIdx1Ent(fcc,0x10,offset,cb));
    else
      this->head.push(new AviIdx1Ent(fcc,0x0,offset,cb));
  } else {
    this->head.push(new AviIdx1Ent(fcc,0x0,offset,cb));
  }
  return 0;
}

void
AviIdx1::reverse(void)
{
  this->head.reverse();
}
