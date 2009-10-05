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
#ifndef _RIFFCHUNKINDX_H
#define _RIFFCHUNKINDX_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "AviStrList.h"
#include "Reloc.h"
#include "RiffChunkMem.h"

class RiffChunkIndx : public RiffChunkMem {
 public:
  int index_type;

 public:
  RiffChunkIndx(const char*,uint32_t);
  RiffChunkIndx(const char*,uint32_t,char*);
  virtual ~RiffChunkIndx(void);

 public:
  RiffChunkIndx(const RiffChunkIndx&);
  const RiffChunkIndx& operator=(const RiffChunkIndx&);

 public:
  virtual RiffChunkIndx* clone(void);
};

#endif
