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
#include "RiffChunkDisk.h"

RiffChunkDisk::RiffChunkDisk(const char* fcc,uint32_t cb,int fd,off_t pos)
  : RiffChunk(fcc,cb)
{
  this->fd = fd;
  this->pos = pos;
}

RiffChunkDisk::~RiffChunkDisk(void)
{
}

RiffChunkDisk::RiffChunkDisk(const RiffChunkDisk& src)
  : RiffChunk(src.fcc,src.cb)
{
  this->fd = src.fd;
  this->pos = src.pos;
}

const RiffChunkDisk&
RiffChunkDisk::operator=(const RiffChunkDisk& src)
{
  this->RiffChunk::operator=(src);
  this->fd = src.fd;
  this->pos = src.pos;
  return *this;
}

RiffChunkDisk*
RiffChunkDisk::clone(void)
{
  return new RiffChunkDisk(*this);
}

int
RiffChunkDisk::write(int fd,Reloc* reloc,AviIdx1* idx1,int flags)
{
  int status;
  char buf[512];
  uint32_t rest;
  off_t offset;

  seek_abs(this->fd,this->pos);
  offset = seek_rel(fd,0);
  this->offset = offset;
  idx1->push(this->fcc,this->offset,this->cb,flags);
  write_fcc(fd,this->fcc);
  write_dword(fd,this->cb);
  
  rest = this->cb;
  while (0 < rest) {
    status = read(this->fd,buf,(rest < sizeof(buf) ? rest : sizeof(buf)));
    if (status == -1)
      err(1,"read");
    status = ::write(fd,buf,status);
    if (status == -1)
      err(1,"write");
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
RiffChunkDisk::dump(int fd,int flags)
{
  int status;
  char buf[512];
  uint32_t rest;

  seek_abs(this->fd,this->pos);
  rest = this->cb;
  while (0 < rest) {
    status = read(this->fd,buf,(rest < sizeof(buf) ? rest : sizeof(buf)));
    if (status == -1)
      err(1,"read");
    status = ::write(fd,buf,status);
    if (status == -1)
      err(1,"write");
    rest -= status;
  }
  return this->cb;
}
