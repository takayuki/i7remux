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
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "RiffChunkFile.h"

RiffChunkFile::RiffChunkFile(const char* fcc,uint32_t cb,char* input)
  : RiffChunkDisk(fcc,cb,-1,0)
{
  int len = strlen(input);
  this->input = new char[len+1];
  memset(this->input,0,len+1);
  memcpy(this->input,input,len);
}

RiffChunkFile::~RiffChunkFile(void)
{
  delete this->input;
}

RiffChunkFile::RiffChunkFile(const RiffChunkFile& src)
  : RiffChunkDisk(src)
{
  int len = strlen(src.input);
  this->input = new char[len+1];
  memset(this->input,0,len+1);
  memcpy(this->input,src.input,len);
}

const RiffChunkFile&
RiffChunkFile::operator=(const RiffChunkFile& src)
{
  this->RiffChunk::operator=(src);
  delete this->input;
  int len = strlen(src.input);
  this->input = new char[len+1];
  memset(this->input,0,len+1);
  memcpy(this->input,src.input,len);
  return *this;
}

RiffChunkFile*
RiffChunkFile::clone(void)
{
  return new RiffChunkFile(*this);
}

int
RiffChunkFile::write(int fd,Reloc* reloc,AviIdx1* idx1,int flags)
{
  int in;
  int status;

  in = open(this->input,O_RDONLY);
  if (in == -1)
    err(1,"open");
  this->fd = in;
  this->pos = 0;
  status = this->RiffChunkDisk::write(fd,reloc,idx1,flags);
  this->fd = -1;
  close(in);
  return status;
}

int
RiffChunkFile::dump(int fd,int flags)
{
  int in;
  int status;

  in = open(this->input,O_RDONLY);
  if (in == -1)
    err(1,"open");
  this->fd = in;
  this->pos = 0;
  status = this->RiffChunkDisk::dump(fd,flags);
  this->fd = -1;
  close(in);
  return status;
}
