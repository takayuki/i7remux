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
#ifndef _RIFFCHUNKIDX_H
#define _RIFFCHUNKIDX_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "Reloc.h"
#include "RiffChunk.h"

class RiffChunkIdx1 : public RiffChunk {

 public:
  RiffChunkIdx1(const char*);
  virtual ~RiffChunkIdx1(void);

 public:
  RiffChunkIdx1(const RiffChunkIdx1&);
  const RiffChunkIdx1& operator=(const RiffChunkIdx1&);

 public:
  virtual RiffChunkIdx1* clone(void);

 public:
  virtual int write(int,Reloc*,AviIdx1*,int);
  virtual int dump(int,int);
};

#endif
