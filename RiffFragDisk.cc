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
#include <unistd.h>
#include <stdio.h>
#include "RiffChunk.h"
#include "RiffFragDisk.h"

RiffFragDisk::RiffFragDisk(uint32_t cb,int fd,off_t pos)
  : RiffFrag(cb)
{
  this->fd = fd;
  this->pos = pos;
}

RiffFragDisk::~RiffFragDisk(void)
{
}

RiffFragDisk::RiffFragDisk(const RiffFragDisk& src)
  : RiffFrag(src.cb)
{
  this->fd = src.fd;
  this->pos = src.pos;
}

const RiffFragDisk&
RiffFragDisk::operator=(const RiffFragDisk& src)
{
  this->RiffFrag::operator=(src);
  this->fd = src.fd;
  this->pos = src.pos;
  return *this;
}


RiffFrag*
RiffFragDisk::clone(void)
{
  return new RiffFragDisk(*this);
}

int
RiffFragDisk::write(int fd,Reloc* reloc,AviIdx1* idx1,int flags)
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

int
RiffFragDisk::dump(int fd,int flags)
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
