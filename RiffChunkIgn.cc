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
#include <string.h>
#include <unistd.h>
#include "AviIdx1.h"
#include "Reloc.h"
#include "RiffChunkIgn.h"

RiffChunkIgn::RiffChunkIgn(const char* fcc)
  : RiffChunk(fcc,0)
{
}

RiffChunkIgn::~RiffChunkIgn(void)
{
}

RiffChunkIgn::RiffChunkIgn(const RiffChunkIgn& src)
  : RiffChunk(src)
{
}

const RiffChunkIgn&
RiffChunkIgn::operator=(const RiffChunkIgn& src)
{
  this->RiffChunk::operator=(src);
  return *this;
}

RiffChunkIgn*
RiffChunkIgn::clone(void)
{
  return new RiffChunkIgn(*this);
}

int
RiffChunkIgn::write(int fd,Reloc* reloc,AviIdx1* idx1,int flags)
{
  return 0;
}

int
RiffChunkIgn::dump(int fd,int flags)
{
  return 0;
}
