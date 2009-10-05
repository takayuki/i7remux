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
#ifndef _RIFFDATA_H
#define _RIFFDATA_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#include "AviIdx1.h"
#include "Reloc.h"
#include "RiffData.h"

class RiffData {
 public:
  char fcc[5];
  uint32_t cb;

 public:
  off_t offset;

 protected:
  RiffData(const char*,uint32_t);
  RiffData(const char*,uint32_t,off_t offset);

 public:
  virtual ~RiffData(void);

 protected:
  RiffData(const RiffData&);
  const RiffData& operator=(const RiffData&);

 public:
  virtual RiffData* clone(void) = 0;

 public:
  virtual int write(int,Reloc*,AviIdx1*,int) = 0;
};

#endif
