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
#include <unistd.h>
#include <err.h>
#include <string.h>
#include "AviFile.h"
#include "RiffChunkEmpty.h"

RiffChunkEmpty::RiffChunkEmpty(const char* fcc)
  : RiffChunk(fcc,0)
{
}

RiffChunkEmpty::~RiffChunkEmpty(void)
{
}

RiffChunkEmpty::RiffChunkEmpty(const RiffChunkEmpty& src)
  : RiffChunk(src.fcc,src.cb)
{
}

const RiffChunkEmpty&
RiffChunkEmpty:: operator=(const RiffChunkEmpty& src)
{
  this->RiffChunk::operator=(src);
  return *this;
}

RiffChunkEmpty*
RiffChunkEmpty::clone(void)
{
  return new RiffChunkEmpty(*this);
}

int
RiffChunkEmpty::write(int fd,Reloc* reloc,AviIdx1* idx1,int flags)
{
  off_t offset;

  offset = seek_rel(fd,0);
  this->offset = offset;
  idx1->push(this->fcc,this->offset,this->cb,flags);
  write_fcc(fd,this->fcc);
  write_dword(fd,this->cb);
  return 8;
}

int
RiffChunkEmpty::dump(int fd,int flags)
{
  return 0;
}
