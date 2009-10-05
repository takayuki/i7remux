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
#include "RiffChunkIndx.h"

RiffChunkIndx::RiffChunkIndx(const char* fcc,uint32_t cb)
  : RiffChunkMem(fcc,cb)
{
  this->index_type = 0;
}

RiffChunkIndx::RiffChunkIndx(const char* fcc,uint32_t cb,char* data)
  : RiffChunkMem(fcc,cb,data)
{
  this->index_type = ((_aviindex_chunk*)data)->bIndexType;
}

RiffChunkIndx::~RiffChunkIndx(void)
{
}

RiffChunkIndx::RiffChunkIndx(const RiffChunkIndx& src)
  : RiffChunkMem(src.fcc,src.cb,src.data)
{
  this->index_type = src.index_type;
}

const RiffChunkIndx&
RiffChunkIndx:: operator=(const RiffChunkIndx& src)
{
  this->RiffChunkMem::operator=(src);
  this->index_type = src.index_type;
  return *this;
}

RiffChunkIndx*
RiffChunkIndx::clone(void)
{
  return new RiffChunkIndx(*this);
}
