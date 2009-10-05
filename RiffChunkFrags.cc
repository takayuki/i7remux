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
#include <err.h>
#include <stdio.h>
#include <string.h>
#include "AviFile.h"
#include "RiffChunkFrags.h"

RiffChunkFrags::RiffChunkFrags(const char* fcc,uint32_t cb)
  : RiffChunk(fcc,cb)
{
}

RiffChunkFrags::~RiffChunkFrags(void)
{
  Cons<RiffFrag*>* hd = &this->head;
  Cons<RiffFrag*>* tl = hd->cdr;
  RiffFrag* car;

  while (tl) {
    hd = tl; tl = hd->cdr;
    car = hd->car;
    delete hd;
    delete car;
  }
}

RiffChunkFrags::RiffChunkFrags(const RiffChunkFrags& src)
  : RiffChunk(src.fcc,src.cb)
{
  const Cons<RiffFrag*>* hd = &src.head;
  const Cons<RiffFrag*>* tl = hd->cdr;
  RiffFrag* car;

  while (tl) {
    hd = tl; tl = hd->cdr;
    car = hd->car;
    this->head.append(car->clone());
  }
}

const RiffChunkFrags&
RiffChunkFrags::operator=(const RiffChunkFrags& src)
{
  this->RiffChunk::operator=(src);
  return *this;
}

RiffChunkFrags*
RiffChunkFrags::clone(void)
{
  return new RiffChunkFrags(*this);
}

RiffFrag*
RiffChunkFrags::append(RiffFrag* data)
{
  RiffFrag* ent = this->head.append(data);
  this->cb += ent->cb;
  return ent;
}

int
RiffChunkFrags::write(int fd,Reloc* reloc,AviIdx1* idx1,int flags)
{
  Cons<RiffFrag*>* hd = &this->head;
  Cons<RiffFrag*>* tl = hd->cdr;
  RiffFrag* car;
  off_t offset;
  off_t pos;
  uint32_t total;

  offset = seek_rel(fd,0);
  this->offset = offset;
  write_fcc(fd,this->fcc);
  pos = seek_rel(fd,0);
  write_dword(fd,this->cb);

  total = 0;
  while (tl) {
    hd = tl; tl = hd->cdr;
    car = hd->car;
    total += car->write(fd,reloc,idx1,flags);
  }
  reloc->push(new RelocEntry32(pos,total));
  this->cb = total;
  idx1->push(this->fcc,this->offset,this->cb,flags);
  if (this->cb % 2) {
    write_padding(fd);
    return (this->cb + 1 + 8);
  } else {
    return (this->cb + 8);
  }
}

int
RiffChunkFrags::dump(int fd,int flags)
{
  Cons<RiffFrag*>* hd = &this->head;
  Cons<RiffFrag*>* tl = hd->cdr;
  RiffFrag* car;

  while (tl) {
    hd = tl; tl = hd->cdr;
    car = hd->car;
    car->dump(fd,flags);
  }
  return 0;
}
