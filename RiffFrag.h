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
#ifndef _RIFFFRAG_H
#define _RIFFFRAG_H

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

class RiffFrag {
 public:
  uint32_t cb;

 protected:
  RiffFrag(uint32_t);
  RiffFrag(const RiffFrag&);

 public:
  virtual ~RiffFrag(void);

 public:
  virtual RiffFrag* clone(void) = 0;

 protected:
  const RiffFrag& operator=(const RiffFrag&);

 public:
  virtual int write(int,Reloc*,AviIdx1*,int) = 0;
  virtual int dump(int,int) = 0;
};

#endif
