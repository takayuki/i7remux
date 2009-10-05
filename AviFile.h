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
#ifndef _AVIFILE_H
#define _AVIFILE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#include "AviStrList.h"
#include "Reloc.h"
#include "RiffChunk.h"
#include "RiffChunkMem.h"
#include "RiffChunkIdx1.h"
#include "RiffList.h"

enum opt_avi {
  OPT_AVI_VERBOSE = 0x0001,
  OPT_AVI_DRYRUN  = 0x0002,
  OPT_AVI_OVERWRT = 0x0004,
  OPT_AVI_IDX1ABS = 0x0008,
  OPT_AVI_IDX1WFG = 0x0010,
  OPT_AVI_XMP3FRM = 0x0020,
  OPT_AVI_IXSZBUG = 0x0040,
};

class AviFile : public RiffList {
 public:
  RiffChunkMem* avih;

 public:
  // CAVEAT!!
  //   Fundamentally, strl and movi are just lists of pointers to chunks
  //   which are maintainded by superclass RiffList.
  //   Do *NOT* release them in destructor of AviStrList (see AviStrList.cc)
  AviStrList strl;
  RiffList* movi;
  RiffChunkIdx1* idx1;

 public:
  AviFile(void);
  ~AviFile(void);

 private:
  AviFile(const AviFile&);
  const AviFile& operator=(const AviFile&);

 public:
  AviFile* clone(void);

 public:
  void show_chunk(void);
  void show_header(void);

 protected:
  RiffList* fetch_list(int,uint32_t*,int);
  RiffChunk* create_chunk(char*,uint32_t,int,uint32_t*,int);
  int fetch_data(int,RiffList*,uint32_t*,int);

 protected:
  int relocate(int,Reloc*,int);

 public:
  int verify_index(int,int);
  int read(int,int);
  int i7remux(int,int);
};

#endif
