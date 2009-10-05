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
#include <string.h>
#include <unistd.h>
#include "AviFile.h"
#include "RiffChunkIdx1.h"

RiffChunkIdx1::RiffChunkIdx1(const char* fcc)
  : RiffChunk(fcc,cb)
{
}

RiffChunkIdx1::~RiffChunkIdx1(void)
{
}

RiffChunkIdx1::RiffChunkIdx1(const RiffChunkIdx1& src)
  : RiffChunk(src)
{
}

const RiffChunkIdx1&
RiffChunkIdx1::operator=(const RiffChunkIdx1& src)
{
  this->RiffChunk::operator=(src);
  return *this;
}

RiffChunkIdx1*
RiffChunkIdx1::clone(void)
{
  return new RiffChunkIdx1(*this);
}

int
RiffChunkIdx1::write(int fd,Reloc* reloc,AviIdx1* idx1,int flags)
{
  Cons<AviIdx1Ent*>* hd = (idx1->reverse(),&idx1->head);
  Cons<AviIdx1Ent*>* tl = idx1->head.cdr;
  AviIdx1Ent* ent;
  off_t pos;
  off_t offset;
  uint32_t total;

  offset = seek_rel(fd,0);
  this->offset = offset;
  write_fcc(fd,this->fcc);
  pos = seek_rel(fd,0);
  write_dword(fd,0);

  total = 0;
  while (tl) {
    hd = tl; tl = hd->cdr;
    ent = hd->car;
    write_fcc(fd,ent->dwChunkId);
    write_dword(fd,ent->dwFlags);
    if ((flags & OPT_AVI_IDX1ABS)) {
      write_dword(fd,ent->dwOffset);
    } else {
      uint32_t start_movi = 0;
      {
	Cons<RelocEntry*>* hd = &reloc->head;
	Cons<RelocEntry*>* tl = reloc->head.cdr;
	RelocEntry* ent;
    
	while (tl) {
	  hd = tl; tl = hd->cdr;
	  ent = hd->car;

	  if (ent->pos == 0) {
	    start_movi = ((RelocEntry32*)ent)->val;
	    break;
	  }
	}
      }
      write_dword(fd,ent->dwOffset - start_movi);
    }
    write_dword(fd,ent->dwSize);
    total += 16;
  }
  reloc->push(new RelocEntry32(pos,total));
  return (total + 8);
}

int
RiffChunkIdx1::dump(int fd,int flags)
{
  return 0;
}
