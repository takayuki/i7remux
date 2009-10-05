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
#ifdef HAVE_SYS_ENDIAN_H
#include <sys/endian.h>
#endif
#ifdef HAVE_ENDIAN_H
#include <endian.h>
#endif
#ifndef htole32
#if BYTE_ORDER == BIG_ENDIAN
#define htole32(x) bswap32((uint32_t)(x))
#else
#define htole32(x) (x)
#endif
#endif
#ifndef le32toh
#define le32toh(x) htole32(x)
#endif
#ifndef htole64
#if BYTE_ORDER == BIG_ENDIAN
#define htole64(x) bswap64((uint64_t)(x))
#else
#define htole64(x) (x)
#endif
#endif
#ifndef le64toh
#define le64toh(x) htole64(x)
#endif
#include "AviFile.h"
#include "RiffChunk.h"

int
read_fcc(int fd,char* fcc,uint32_t* rest)
{
  int status;

  if (rest != 0 && *rest < 4)
    errx(1,"unsufficient data supply");
  status = read(fd,fcc,4);
  if (status == -1)
    err(1,"read");
  if (status != 4)
    errx(1,"end of file");
  if (rest != 0)
    *rest -= 4;
  return *((int*)fcc);
}

int
read_dword(int fd,uint32_t* dw,uint32_t* rest)
{
  int status;
  uint32_t le;

  if (rest != 0 && *rest < 4)
    errx(1,"unsufficient data supply");
  status = read(fd,&le,4);
  if (status == -1)
    err(1,"read");
  if (status != 4)
    errx(1,"end of file");
  *dw = le32toh(le);
  if (rest != 0)
    *rest -= 4;
  return *dw;
}

int
read_qword(int fd,uint64_t* qw,uint32_t* rest)
{
  int status;
  uint64_t le;

  if (rest != 0 && *rest < 4)
    errx(1,"unsufficient data supply");
  status = read(fd,&le,8);
  if (status == -1)
    err(1,"read");
  if (status != 8)
    errx(1,"end of file");
  *qw = le64toh(le);
  if (rest != 0)
    *rest -= 8;
  return *qw;
}

int
write_fcc(int fd,char* fcc)
{
  int status;

  status = write(fd,fcc,4);
  if (status == -1)
    err(1,"write");
  if (status != 4)
    err(1,"write");
  return *((int*)fcc);
}

int
write_dword(int fd,uint32_t dw)
{
  int status;
  uint32_t le;

  le = htole32(dw);
  status = write(fd,&le,4);
  if (status == -1)
    err(1,"write");
  if (status != 4)
    err(1,"write");
  return dw;
}

int
write_qword(int fd,uint64_t qw)
{
  int status;
  uint64_t le;

  le = htole64(qw);
  status = write(fd,&le,8);
  if (status == -1)
    err(1,"write");
  if (status != 8)
    err(1,"write");
  return qw;
}

off_t
seek_abs(int fd,off_t offset)
{
  int status;

  status = lseek(fd,offset,SEEK_SET);
  if (status == -1)
    err(1,"lseek");
  return status;
}

off_t
seek_rel(int fd,off_t offset)
{
  int status;

  status = lseek(fd,offset,SEEK_CUR);
  if (status == -1)
    err(1,"lseek");
  return status;
}


int
write_padding(int fd)
{
  int status;

  status = write(fd,"\0",1);
  if (status == -1)
    err(1,"write");
  if (status != 1)
    err(1,"write");
  return 1;
}

RiffChunk::RiffChunk(const char* fcc,uint32_t cb)
  : RiffData(fcc,cb)
{
}

RiffChunk::RiffChunk(const char* fcc,uint32_t cb,off_t offset)
  : RiffData(fcc,cb,offset)
{
}

RiffChunk::~RiffChunk(void)
{
}

RiffChunk::RiffChunk(const RiffChunk& src)
  : RiffData(src.fcc,src.cb)
{
}

const RiffChunk&
RiffChunk::operator=(const RiffChunk& src)
{
  this->RiffData::operator=(src);
  return *this;
}
