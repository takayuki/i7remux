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
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#include "Cons.h"
#include "Reloc.h"

RelocEntry::RelocEntry(void)
{
}

RelocEntry::~RelocEntry(void)
{
}

RelocEntry32::RelocEntry32(off_t pos,uint32_t val)
{
  this->pos = pos;
  this->val = val;
}

RelocEntry32::~RelocEntry32(void)
{
}

RelocEntry64::RelocEntry64(off_t pos,uint64_t val)
{
  this->pos = pos;
  this->val = val;
}

RelocEntry64::~RelocEntry64(void)
{
}

Reloc::Reloc(void)
{
}

Reloc::~Reloc(void)
{
  Cons<RelocEntry*>* hd = &this->head;
  Cons<RelocEntry*>* tl = hd->cdr;
  RelocEntry* car;

  while (tl) {
    hd = tl; tl = hd->cdr;
    car = hd->car;
    delete hd;
    delete car;
  }
}

RelocEntry*
Reloc::push(RelocEntry* ent)
{
  return this->head.push(ent);
}
