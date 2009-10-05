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
#include "RiffChunkMem.h"

RiffChunkMem::RiffChunkMem(const char* fcc,uint32_t cb)
  : RiffChunk(fcc,cb)
{
  this->data = new char[cb];
}

RiffChunkMem::RiffChunkMem(const char* fcc,uint32_t cb,char* data)
  : RiffChunk(fcc,cb)
{
  this->data = new char[cb];
  memcpy(this->data,data,cb);
}

RiffChunkMem::~RiffChunkMem(void)
{
  delete this->data;
}

RiffChunkMem::RiffChunkMem(const RiffChunkMem& src)
  : RiffChunk(src.fcc,src.cb)
{
  this->data = new char[src.cb];
  memcpy(this->data,src.data,src.cb);
}

const RiffChunkMem&
RiffChunkMem:: operator=(const RiffChunkMem& src)
{
  this->RiffChunk::operator=(src);
  delete this->data;
  this->data = new char[src.cb];
  memcpy(this->data,src.data,src.cb);
  return *this;
}

RiffChunkMem*
RiffChunkMem::clone(void)
{
  return new RiffChunkMem(*this);
}

int
RiffChunkMem::write(int fd,Reloc* reloc,AviIdx1* idx1,int flags)
{
  int status;
  off_t offset;
  uint32_t rest,done;

  offset = seek_rel(fd,0);
  this->offset = offset;
  write_fcc(fd,this->fcc);
  write_dword(fd,this->cb);

  rest = this->cb; done = 0;
  while (0 < rest) {
    status = ::write(fd,(this->data+done),rest);
    if (status == -1)
      err(1,"write");
    done += status;
    rest -= status;
  }
  if (this->cb % 2) {
    write_padding(fd);
    return (this->cb + 1 + 8);
  } else {
    return (this->cb + 8);
  }
}

int
RiffChunkMem::dump(int fd,int flags)
{
  int status;
  uint32_t rest,done;

  rest = this->cb; done = 0;
  while (0 < rest) {
    status = ::write(fd,(this->data+done),rest);
    if (status == -1)
      err(1,"write");
    done += status;
    rest -= status;
  }
  return this->cb;
}
