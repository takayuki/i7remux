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
#ifndef _RIFFCHUNK_H
#define _RIFFCHUNK_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#include "RiffData.h"
#include "AviIdx1.h"

int read_fcc(int,char*,uint32_t*);
int read_dword(int,uint32_t*,uint32_t*);
int read_qword(int,uint64_t*,uint32_t*);
int write_fcc(int,char*);
int write_dword(int,uint32_t);
int write_qword(int,uint64_t);
int write_padding(int);
off_t seek_abs(int,off_t);
off_t seek_rel(int,off_t);

int regist_idx1(AviIdx1*,char*,off_t,uint32_t,int);

class RiffChunk : public RiffData {

 protected:
  RiffChunk(const char*,uint32_t);
  RiffChunk(const char*,uint32_t,off_t);

 public:
  virtual ~RiffChunk(void);

 protected:
  RiffChunk(const RiffChunk&);
  const RiffChunk& operator=(const RiffChunk&);

 public:
  virtual RiffData* clone(void) = 0;

 public:
  virtual int write(int,Reloc*,AviIdx1*,int) = 0;
  virtual int dump(int,int) = 0;
};

#endif
