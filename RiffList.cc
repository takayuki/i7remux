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
#include "RiffChunk.h"
#include "RiffChunkDisk.h"
#include "RiffChunkFile.h"
#include "RiffChunkFrags.h"
#include "RiffChunkIdx1.h"
#include "RiffChunkMem.h"
#include "RiffList.h"

RiffList::RiffList(const char* fcc,uint32_t cb,char* type)
  : RiffData(fcc,cb)
{
  memcpy(this->type,type,4);
  this->type[4] = 0;
}

RiffList::~RiffList(void)
{
  Cons<RiffData*>* hd = &this->head;
  Cons<RiffData*>* tl = hd->cdr;
  RiffData* car;

  while (tl) {
    hd = tl; tl = hd->cdr;
    car = hd->car;
    delete hd;
    delete car;
  }
}

RiffList::RiffList(const RiffList& src)
  : RiffData(src.fcc,src.cb)
{
  const Cons<RiffData*>* hd = &src.head;
  const Cons<RiffData*>* tl = hd->cdr;
  RiffData* car;

  memcpy(this->type,src.type,4);
  while (tl) {
    hd = tl; tl = hd->cdr;
    car = hd->car;
    this->head.push(car->clone());
  }
  this->head.reverse();
}

RiffList*
RiffList::clone(void)
{
  return new RiffList(*this);
}

RiffData*
RiffList::append(RiffData* data)
{
  return this->head.append(data);
}

RiffData*
RiffList::push(RiffData* data)
{
  return this->head.push(data);
}

void
RiffList::reverse(void)
{
  this->head.reverse();
}

void
RiffList::swap(void)
{
  this->head.swap();
}

RiffData*
RiffList::ref(const char* fcc)
{
  Cons<RiffData*>* hd = &this->head;
  Cons<RiffData*>* tl = hd->cdr;
  RiffData* ent;

  while (tl) {
    hd = tl; tl = hd->cdr;
    ent = hd->car;
    if (!memcmp(ent->fcc,fcc,4))
      return ent;
  }
  return 0;
}

static void
indent(int depth)
{
  int i = 0;
  for (i = 0; i < depth; i++)
    printf("\t");
}

void
RiffList::show_chunk(int depth)
{
  Cons<RiffData*>* hd = &this->head;
  Cons<RiffData*>* tl = this->head.cdr;
  RiffData* car;

  indent(depth);
  printf("%s %s %u %llu\n",
	 this->fcc,this->type,this->cb,(uint64_t)this->offset);
  while (tl) {
    hd = tl; tl = hd->cdr;
    car = hd->car;
    void* data;
    if ((data = dynamic_cast<RiffList*>(car))) {
      RiffList* list = (RiffList*)data;
      list->show_chunk(depth+1);
    } else if ((data = dynamic_cast<RiffChunk*>(car))) {
      RiffChunk* chunk = (RiffChunk*)data;
      indent(depth+1);
      printf("%s %u %llu\n",chunk->fcc,chunk->cb,(uint64_t)chunk->offset);
    }
  }
  return;
}

int
RiffList::write(int fd,Reloc* reloc,AviIdx1* idx1,int flags)
{
  Cons<RiffData*>* hd = &this->head;
  Cons<RiffData*>* tl = hd->cdr;
  RiffData* car;
  void* data;
  off_t offset;
  off_t pos;
  uint32_t total;

  offset = seek_rel(fd,0);
  this->offset = offset;
  write_fcc(fd,this->fcc);
  pos = seek_rel(fd,0);
  write_dword(fd,this->cb);
  if (!memcmp(this->type,"movi",4)) {
    reloc->push(new RelocEntry32(0,seek_rel(fd,0)));
  }
  write_fcc(fd,this->type);

  total = 0;
  while (tl) {
    hd = tl; tl = hd->cdr;
    car = hd->car;

    if ((data = dynamic_cast<RiffList*>(car))) {
      RiffList* list = (RiffList*)data;
      total += list->write(fd,reloc,idx1,flags);
    } else if ((data = dynamic_cast<RiffChunk*>(car))) {
      RiffChunk* chunk = (RiffChunk*)data;
      total += chunk->write(fd,reloc,idx1,flags);
    } else {
      errx(1,"unknown data in data list");
    }
  }
  reloc->push(new RelocEntry32(pos,total + 4));
  return (total + 12);
}
