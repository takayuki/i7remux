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
#include <ctype.h>
#include <err.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#include <unistd.h>
#include "AviFile.h"
#include "AviIdx1.h"
#include "Cons.h"
#include "Reloc.h"
#include "RiffFragDisk.h"
#include "RiffChunk.h"
#include "RiffChunkDisk.h"
#include "RiffChunkEmpty.h"
#include "RiffChunkFile.h"
#include "RiffChunkFrags.h"
#include "RiffChunkIdx1.h"
#include "RiffChunkIgn.h"
#include "RiffChunkIndx.h"
#include "RiffChunkMem.h"
#include "RiffData.h"
#include "RiffList.h"
#include "RiffListIgn.h"

AviFile::AviFile(void) : RiffList("RIFF",0,"AVI ")
{
  this->avih = 0;
  this->movi = 0;
  this->idx1 = 0;
}

AviFile::~AviFile(void)
{
}

AviFile::AviFile(const AviFile& src) : RiffList(src)
{
  this->avih = 0;
  this->movi = 0;
  this->idx1 = 0;
}

AviFile*
AviFile::clone(void)
{
  AviFile* newavi = new AviFile(*this);
  Cons<RiffData*>* hd = &newavi->head;
  Cons<RiffData*>* tl = hd->cdr;
  RiffList* list;
  RiffList* strl;

  while (tl) {
    hd = tl; tl = hd->cdr;
    list = dynamic_cast<RiffList*>(hd->car);
    if (list != 0 && !memcmp(list->type,"hdrl",4)) {
      newavi->avih = dynamic_cast<RiffChunkMem*>(list->ref("avih"));
      newavi->idx1 = this->idx1;
      Cons<RiffData*>* hd = &list->head;
      Cons<RiffData*>* tl = hd->cdr;

      while (tl) {
	hd = tl; tl = hd->cdr;
	strl = dynamic_cast<RiffList*>(hd->car);
	if (strl != 0 && !memcmp(strl->type,"strl",4)) {
	  AviStrEnt* strh;
	  strh = new AviStrEnt((RiffChunk*)strl->ref("strh"));
	  newavi->strl.append(strh);
	  AviStrEnt* ent = newavi->strl.last();
	  ent->strl = strl;
	  ent->strf = (RiffChunk*)strl->ref("strf");
	  ent->indx = (RiffChunk*)strl->ref("indx");
	}
      }
    } else if (list != 0 && !memcmp(list->type,"movi",4)) {
      newavi->movi = list;
    }
  }
  return newavi;
}

void
AviFile::show_chunk(void)
{
  this->RiffList::show_chunk(0);
}

void
AviFile::show_header(void)
{
  Cons<AviStrEnt*>* hd = &this->strl.head;
  Cons<AviStrEnt*>* tl = hd->cdr;
  RiffChunkMem* chunkmem;
  RiffChunkIndx* chunkindx;
  AviStrEnt* ent;
  _avistreamheader* strh;

  chunkmem = dynamic_cast<RiffChunkMem*>(this->avih);
  show_avi_header((_avimainheader*)chunkmem->data);
  while (tl) {
    hd = tl; tl = hd->cdr;
    ent = hd->car;

    chunkmem = dynamic_cast<RiffChunkMem*>(ent->strh);
    strh = (_avistreamheader*)chunkmem->data;
    show_stream_header(strh);
    chunkmem = dynamic_cast<RiffChunkMem*>(ent->strf);
    if (!memcmp(strh->fccType,"vids",4)) {
      show_vids_format((_avistreamformat*)chunkmem->data);
    } else if (!memcmp(strh->fccType,"auds",4)) {
      show_auds_format((_avistreamformat*)chunkmem->data);
    }
    if (ent->indx) {
      chunkindx = dynamic_cast<RiffChunkIndx*>(ent->indx);
      show_super_index((_aviindex_chunk*)chunkindx->data);
    }
  }
}

RiffList*
AviFile::fetch_list(int fd,uint32_t* rest,int flags)
{
  char fcc[4];
  uint32_t cb,size;
  RiffList* list;

  read_dword(fd,&cb,rest);
  size = cb;
  read_fcc(fd,fcc,&size);
  if(!memcmp(fcc,"hdrl",4) ||
     !memcmp(fcc,"strl",4) ||
     !memcmp(fcc,"movi",4)) {
    list = new RiffList("LIST",cb,fcc);
  } else {
    list = new RiffListIgn("LIST",cb,fcc);
  }
  fetch_data(fd,list,&size,flags);
  if (size != 0) {
    warnx("wrong list size");
  }
  *rest -= (cb - size);
  list->reverse();
  return list;
}

RiffChunk*
AviFile::create_chunk(char* fcc,uint32_t cb,int fd,uint32_t* rest,int flags)
{
  off_t pos = seek_rel(fd,0);
  RiffChunk* chunk;

  if (flags & OPT_AVI_VERBOSE)
    printf("chunk %c%c%c%c %u %llu\n",
	   fcc[0],fcc[1],fcc[2],fcc[3],cb,(uint64_t)pos-8);
  if (*rest < cb)
    errx(1,"chunk is too large beyond end of file");
  if (!memcmp(fcc,"avih",4)) {
    char* avih = new char[cb];
    ::read(fd,avih,cb);
    if (flags & OPT_AVI_VERBOSE)
      show_avi_header((_avimainheader*)avih);
    seek_abs(fd,pos);
    chunk = new RiffChunkMem(fcc,cb,avih);
    this->avih = (RiffChunkMem*)chunk;
    delete avih;
  } else if (!memcmp(fcc,"strh",4)) {
    char* strh = new char[cb];
    ::read(fd,strh,cb);
    if (flags & OPT_AVI_VERBOSE)
      show_stream_header((_avistreamheader*)strh);
    seek_abs(fd,pos);
    chunk = new RiffChunkMem(fcc,cb,strh);
    this->strl.append(new AviStrEnt(chunk));
    delete strh;
  } else if (!memcmp(fcc,"strf",4)) {
    char* strf = new char[cb];
    ::read(fd,strf,cb);
    seek_abs(fd,pos);
    chunk = new RiffChunkMem(fcc,cb,strf);
    AviStrEnt* ent = this->strl.last();
    if (ent != 0) {
      ent->strf = chunk;
      if (flags & OPT_AVI_VERBOSE) {
	RiffChunkMem* strh = (RiffChunkMem*)ent->strh;
	if (!memcmp(((_avistreamheader*)strh->data)->fccType,"vids",4))
	  show_vids_format((_avistreamformat*)strf);
	if (!memcmp(((_avistreamheader*)strh->data)->fccType,"auds",4))
	  show_auds_format((_avistreamformat*)strf);
      }
    }
    delete strf;
  } else if (!memcmp(fcc,"ISFT",4) ||
	     !memcmp(fcc,"JUNK",4)) {
    chunk = new RiffChunkIgn(fcc);
  } else if (!memcmp(fcc,"00db",4) ||
	     !memcmp(fcc,"00dc",4) ||
	     !memcmp(fcc,"01db",4) ||
	     !memcmp(fcc,"01dc",4)) {
    chunk = new RiffChunkDisk(fcc,cb,fd,pos);
  } else if (!memcmp(fcc,"00wb",4) ||
	     !memcmp(fcc,"01wb",4)) {
    chunk = new RiffChunkDisk(fcc,cb,fd,pos);
  } else if (!memcmp(fcc,"idx1",4)) {
    chunk = new RiffChunkIdx1(fcc);
    this->idx1 = (RiffChunkIdx1*)chunk;
  } else if (!memcmp(fcc,"indx",4) ||
	     !memcmp(fcc,"ix00",4) ||
	     !memcmp(fcc,"ix01",4)) {
    char* indx = new char[cb];
    ::read(fd,indx,cb);
    seek_abs(fd,pos);
    chunk = new RiffChunkIndx(fcc,cb,indx);
    if (!memcmp(fcc,"indx",4)) {
      AviStrEnt* ent = this->strl.last();
      if (ent != 0) ent->indx = chunk;
    }
    delete indx;
  } else if (!memcmp(fcc,"dmlh",4)) {
    chunk = new RiffChunkDisk(fcc,cb,fd,pos);
  } else {
    chunk = new RiffChunkIgn(fcc);
  }
  chunk->offset = pos-8;
  if (cb % 2) {
    cb++; // word boundary alignment
  }
  seek_rel(fd,cb);
  *rest -= cb;
  return chunk;
}

int
AviFile::fetch_data(int fd,RiffList* list,uint32_t* rest,int flags)
{
  char fcc[5] = { 0,0,0,0,0 };
  uint32_t cb;

  while (0 < *rest) {
    read_fcc(fd,fcc,rest);
    if (!memcmp(fcc,"LIST",4)) {
      RiffList* newlist = fetch_list(fd,rest,flags);
      if(!memcmp(newlist->type,"movi",4)) {
	this->movi = newlist;
      } else if(!memcmp(newlist->type,"strl",4)) {
	AviStrEnt* ent = this->strl.last();
	ent->strl = newlist;
      }
      list->push(newlist);
    } else {
      read_dword(fd,&cb,rest);
      list->push(create_chunk(fcc,cb,fd,rest,flags));
    }
  }
  return 0;
}

int
AviFile::read(int fd,int flags)
{
  int status;
  struct stat st;
  uint32_t rest;
  char fcc[4];
  uint32_t cb;
  AviStrEnt* ent;

  status = fstat(fd,&st);
  if (status == -1)
    err(1,"fstat");
  rest = st.st_size;
  read_fcc(fd,fcc,&rest);
  if (memcmp(fcc,"RIFF",4))
    errx(1,"RIFF signature is not found");
  read_dword(fd,&cb,&rest);
  rest = cb;
  read_fcc(fd,fcc,&rest);
  if (memcmp(fcc,"AVI ",4))
    errx(1,"AVI signature is not found");

  this->fetch_data(fd,this,&rest,flags);
  this->cb = cb;
  if (this->avih == 0)
    errx(1,"avi header is not found");
  for (int i = 0; i <= 1; i++) {
    ent = this->strl.ref(i);
    if (ent == 0) {
      errx(1,"stream #%d is not found",i);
    } else {
      if (ent->strh == 0)
	errx(1,"stream header of stream #%d is not found",i);
      if (ent->strf == 0)
	errx(1,"stream format of stream #%d is not found",i);
    }
  }
  if (this->movi == 0)
    errx(1,"movi chunk is not found");
  this->reverse();
  if (flags & OPT_AVI_VERBOSE) {
    printf("---- Riff Structure ----\n");
    this->show_chunk();
    printf("---- Riff Structure ----\n");
    this->verify_index(fd,flags);
  }
  return 0;
}

static int
veryfy_stdent(int fd,_avistdindex_chunk* chunk,uint32_t chunk_size)
{
  _avistdindex_entry* ent;
  off_t offset;
  char fcc[4];
  uint32_t i;
  uint32_t cb;
  uint32_t size;

  size = sizeof(_avistdindex_chunk);
  size += sizeof(_avistdindex_entry) * chunk->nEntriesInUse;
  if (chunk_size < size)
    errx(1,"index is too small");
  show_std_index(chunk);
  for (i = 0; i < chunk->nEntriesInUse; i++) {
    ent = &chunk->aIndex[i];
    offset = chunk->qwBaseOffset + ent->dwOffset - 8;
    seek_abs(fd,offset);
    read_fcc(fd,fcc,0);
    if (memcmp((char*)&chunk->dwChunkId,fcc,4) != 0) {
      errx(1,"chunk is not detected corresponding to an index entry");
    }
    read_dword(fd,&cb,0);
    if (ent->dwSize != cb) {
      errx(1,"chunk has wrong size to index entry");
    }
  }
  return 0;
}

static int
veryfy_superent(int fd,_avisuperindex_chunk* chunk,uint32_t chunk_size)
{
  _avisuperindex_entry* ent;
  char id[6] = {'i','x',0,0,0,0};
  char fcc[4];
  uint32_t i;
  uint32_t cb;
  uint32_t size;

  size = sizeof(_avisuperindex_chunk);
  size += sizeof(_avisuperindex_entry) * chunk->nEntriesInUse;
  if (chunk_size < size)
    errx(1,"index is too small");
  show_super_index((_aviindex_chunk*)chunk);
  memcpy(&id[2],(char*)&chunk->dwChunkId,4);
  for (i = 0; i < chunk->nEntriesInUse; i++) {
    ent = &chunk->aIndex[i];
    seek_abs(fd,ent->qwOffset);
    read_fcc(fd,fcc,0);
    if (memcmp(id,fcc,4) != 0) {
      errx(1,"index is not detected corresponding to superindex entry");
    }
    read_dword(fd,&cb,0);
    if (ent->dwSize != (cb+8) && ent->dwSize != (cb-8)) {
      errx(1,"index has wrong size to index entry");
    }
    {
      _avistdindex_chunk* chunk;
      int status;

      chunk = (_avistdindex_chunk*)new char[cb];
      status = ::read(fd,(char*)chunk,cb);
      if (status != (int)cb)
	err(1,"read");
      veryfy_stdent(fd,chunk,cb);
      delete chunk;
    }
  }
  return 0;
}

static int
veryfy_idx1(int fd,off_t offset)
{
  char fcc[4];
  uint32_t cb;
  struct _aviidx1_entry {
    char dwChunkId[4];
    uint32_t dwFlags;
    uint32_t dwOffset;
    uint32_t dwSize;
  } __attribute__ ((packed)) ent;
  int i;

  seek_abs(fd,offset);
  read_fcc(fd,fcc,0);
  if (memcmp(fcc,"idx1",4) != 0) {
    errx(1,"idx1 is not detected");
  }
  read_dword(fd,&cb,0);
  i = 0;
  while (0 < cb) {
    int status;
    status = ::read(fd,(char*)&ent,sizeof(ent));
    if (status != (int)sizeof(ent))
      err(1,"read");    
    printf("idx1[%d].dwChunkId=%c%c%c%c\n"
	   "idx1[%d].dwFlags=%u\n"
	   "idx1[%d].dwOffset=%u\n"
	   "idx1[%d].dwSize=%u\n",
	   i,ent.dwChunkId[0],
	   ent.dwChunkId[1],
	   ent.dwChunkId[2],
	   ent.dwChunkId[3],
	   i,ent.dwFlags,
	   i,ent.dwOffset,
	   i,ent.dwSize);
    off_t pos = seek_rel(fd,0);
    if (!memcmp(ent.dwChunkId,"7Fxx",4) != 0) {
      char fcc[5];
      seek_abs(fd,ent.dwOffset);
      read_fcc(fd,fcc,0);
      if (memcmp(fcc,"ix00",4) != 0)
	errx(1,"ix00 is not found");
      if (ent.dwSize != 0)
	errx(1,"index has wrong size");
    } else {
      char fcc[5];
      uint32_t cb;
      seek_abs(fd,ent.dwOffset);
      read_fcc(fd,fcc,0);
      if (memcmp(ent.dwChunkId,fcc,4) != 0)
	errx(1,"index has wrong id");
      read_dword(fd,&cb,0);
      if (ent.dwSize != cb)
	errx(1,"index has wrong size");
    }
    seek_abs(fd,pos);
    cb -= sizeof(ent); i++;
  }
  return 0;
}

int
AviFile::verify_index(int fd,int flags)
{
  {
    Cons<AviStrEnt*>* hd = &this->strl.head;
    Cons<AviStrEnt*>* tl = hd->cdr;
    AviStrEnt* ent;

    while (tl) {
      hd = tl; tl = hd->cdr;
      ent = hd->car;
      if (ent->indx) {
	RiffChunkIndx* indx = dynamic_cast<RiffChunkIndx*>(ent->indx);
	if (indx->index_type == AVI_INDEX_OF_INDEXES) {
	  veryfy_superent(fd,(_avisuperindex_chunk*)indx->data,indx->cb);
	} if (indx->index_type == AVI_INDEX_OF_CHUNKS) {
	  veryfy_stdent(fd,(_avistdindex_chunk*)indx->data,indx->cb);
	}
      }
    }
  }
  veryfy_idx1(fd,this->idx1->offset);
  return 0;
}

int
AviFile::relocate(int fd,Reloc* reloc,int flags)
{
  Cons<RelocEntry*>* hd = &reloc->head;
  Cons<RelocEntry*>* tl = hd->cdr;
  RelocEntry* ent;
  RelocEntry32* ent32;
  RelocEntry64* ent64;
    
  while (tl) {
    hd = tl; tl = hd->cdr;
    ent = hd->car;

    if (ent->pos < 4)
      continue;
    seek_abs(fd,ent->pos);
    if ((ent32 = dynamic_cast<RelocEntry32*>(ent)))
      write_dword(fd,ent32->val);
    if ((ent64 = dynamic_cast<RelocEntry64*>(ent)))
      write_qword(fd,ent64->val);
  }
  return 0;
}

uint64_t
gcd(uint64_t x,uint64_t y)
{
  if (x == y)
    return x;
  else if (x < y)
    return gcd(x,y-x);
  else
    return gcd(y,x);
}

int
AviFile::i7remux(int fd,int flags)
{
  Cons<RiffChunkFrags*> newauds;
  char vid[5] = {0,0,0,0,0};
  char aid[5] = {0,0,0,0,0};
  uint32_t totalFrame = 0;
  Reloc reloc;
  AviIdx1 idx1;
  if (flags & OPT_AVI_XMP3FRM) {
    uint64_t rate = 12;
    uint64_t scale = 1;
    uint64_t avg = 16000;
    {
      Cons<AviStrEnt*>* hd = &this->strl.head;
      Cons<AviStrEnt*>* tl = hd->cdr;

      while (tl) {
	hd = tl; tl = hd->cdr;
	AviStrEnt* ent = hd->car;
	_avistreamheader* strh;
	strh = (_avistreamheader*)((RiffChunkMem*)ent->strh)->data;
	if (!memcmp(strh->fccType,"vids",4)) {
	  rate = strh->dwRate;
	  scale = strh->dwScale;
	} else if (!memcmp(strh->fccType,"auds",4)) {
	  _waveformatex* strf;
	  strf = (_waveformatex*)((RiffChunkMem*)ent->strf)->data;
	  avg = strf->nAvgBytesPerSec;
	  break;
	}
      }
    }
    {
      Cons<RiffData*>* hd = &this->movi->head;
      Cons<RiffData*>* tl = hd->cdr;
      RiffChunkFrags* wb = 0;
      uint64_t p = avg * scale / gcd(avg * scale,rate);
      uint64_t q = rate / gcd(avg * scale,rate);
      uint64_t n = 0;
      uint64_t needed = p / q;

      while (tl) {
	hd = tl; tl = hd->cdr;
	RiffData* ent = hd->car;
	if (ent->fcc[2] == 'd') {
	  totalFrame++;
	} else if (ent->fcc[2] == 'w') {
	  memcpy(aid,ent->fcc,4);
	  if (wb == 0)
	    wb = new RiffChunkFrags(ent->fcc,0);
	  RiffChunkDisk* f = (RiffChunkDisk*)ent;
	split:
	  if (f->cb == 0)
	    continue;
	  if (f->cb < needed) {
	    wb->append(new RiffFragDisk(f->cb,f->fd,f->pos));
	    needed -= f->cb;
	  } else {
	    wb->append(new RiffFragDisk(needed,f->fd,f->pos));
	    newauds.push(wb); n++;
	    wb = new RiffChunkFrags(ent->fcc,0);
	    f->cb -= needed; f->pos += needed;
	    needed = p*(n+1)/q-p*n/q;
	    goto split;
	  }
	}
      }
      if (wb != 0 && 0 < wb->cb)
	newauds.push(wb);
    }
  } else {
    Cons<RiffData*>* hd = &this->movi->head;
    Cons<RiffData*>* tl = hd->cdr;
    RiffChunkFrags* wb = 0;

    while (tl) {
      hd = tl; tl = hd->cdr;
      RiffData* ent = hd->car;
      if (ent->fcc[2] == 'd') {
	memcpy(vid,ent->fcc,4);
	if (totalFrame != 0 && wb != 0) {
	  newauds.push(wb);
	  wb = 0;
	}
	totalFrame++;
      } else if (ent->fcc[2] == 'w') {
	memcpy(aid,ent->fcc,4);
	if (wb == 0)
	  wb = new RiffChunkFrags(ent->fcc,0);
	RiffChunkDisk* chunk = (RiffChunkDisk*)ent;
	wb->append(new RiffFragDisk(chunk->cb,chunk->fd,chunk->pos));
      }
    }
  }
  newauds.reverse();
  {
    Cons<RiffData*>* hd = &this->movi->head;
    Cons<RiffData*>* tl = hd->cdr;
    RiffList* newmovi = new RiffList("LIST",0,"movi");
    RiffData* wb = 0;

    while (tl) {
      hd = tl; tl = hd->cdr;
      RiffData* ent = hd->car;

      if (ent->fcc[2] == 'd') {
	newmovi->push(ent->clone());
	wb = newauds.pop();
	if (wb != 0) {
	  newmovi->push(wb);
	} else {
	  newmovi->push(new RiffChunkEmpty(aid));
	}
      } else if (!memcmp(ent->fcc,"ix",2)) {
	newmovi->push(ent->clone());
      }
    }
    while ((wb = newauds.pop())) {
      newmovi->push(new RiffChunkEmpty(vid));
      newmovi->push(wb);
      totalFrame++;
    }
    newmovi->reverse();
    {
      Cons<RiffData*>* hd = &this->head;
      Cons<RiffData*>* tl = hd->cdr;

      while (tl) {
	hd = tl; tl = hd->cdr;
	RiffList* list = dynamic_cast<RiffList*>(hd->car);
	if (list != 0 && !memcmp(list->type,"movi",4)) {
	  delete hd->car;
	  hd->car = this->movi = newmovi;
	  break;
	}
      }
    }
  }
  {
    Cons<AviStrEnt*>* hd = &this->strl.head;
    Cons<AviStrEnt*>* tl = hd->cdr;
    int stream = 0;

    while (tl) {
      hd = tl; tl = hd->cdr;
      AviStrEnt* ent = hd->car;
      _avistreamheader* strh;
      strh = (_avistreamheader*)((RiffChunkMem*)ent->strh)->data;
      char id[4];
      if (!memcmp(strh->fccType,"vids",4)) {
	sprintf(id,"%02d",stream++);
	id[2] = 'd';
	id[3] = 'c';
	strh->dwLength = totalFrame;
      } else if (!memcmp(strh->fccType,"auds",4)) {
	sprintf(id,"%02d",stream++);
	id[2] = 'w';
	id[3] = 'b';
	_waveformatex* strf;
	strf = (_waveformatex*)((RiffChunkMem*)ent->strf)->data;
	strh->dwScale = 1;
	strh->dwRate = strf->nAvgBytesPerSec;
	strh->dwLength = 0;
	strh->dwSampleSize = 1;
      } else {
	stream++;
	continue;
      }
      {
	_avisuperindex_chunk chunk = {4,0,0,1,*(uint32_t*)&id,{0,0,0}};
	_avisuperindex_entry entry = {0,0,0};
	uint32_t cb = sizeof(chunk) + sizeof(entry);
	char* data = new char[cb];
	memcpy(data,&chunk,sizeof(chunk));
	memcpy(data+sizeof(chunk),&entry,sizeof(entry));
	if (ent->indx == 0) {
	  ent->indx = new RiffChunkIndx("indx",cb,data);
	  ent->strl->append(ent->indx);
	} else {
	  RiffChunkIndx* indx = new RiffChunkIndx("indx",cb,data);
	  *(RiffChunkIndx*)ent->indx = *indx;
	  delete indx;
	}
	delete data;
      }
      RiffChunkIndx* indx = (RiffChunkIndx*)ent->indx;
      char ix[6] = {'i','x',0,0,0,0};
      memcpy(&ix[2],(char*)&((_aviindex_chunk*)indx->data)->dwChunkId,4);
      RiffData* ixxx = this->movi->ref(ix);
      {
	_avistdindex_chunk chunk = {2,0,1,totalFrame,*(uint32_t*)&id,0,0};
	_avistdindex_entry entry = {0,0};
	uint32_t cb = sizeof(chunk) + sizeof(entry) * totalFrame;
	char* data = new char[cb];
	memcpy(data,&chunk,sizeof(chunk));
	memset(data+sizeof(chunk),0,sizeof(entry) * totalFrame);
	if (ixxx == 0) {
	  ent->ixxx = new RiffChunkIndx(ix,cb,data);
	  this->movi->push(ent->ixxx);
	  if (stream == 2) this->movi->swap();
	} else {
	  RiffChunkIndx* indx = new RiffChunkIndx(ix,cb,data);
	  *(RiffChunkIndx*)ixxx = *indx;
	  ent->ixxx = (RiffChunk*)ixxx;
	  delete indx;
	}
      }
    }
  }
  idx1.push("7Fxx",0,0,0);
  this->RiffList::write(fd,&reloc,&idx1,flags);
  {
    Cons<AviIdx1Ent*>* hd = &idx1.head;
    Cons<AviIdx1Ent*>* tl = hd->cdr;
    int stream;
    int frame[2] = {0,0};

    while (tl) {
      hd = tl; tl = hd->cdr;
      AviIdx1Ent* ent = hd->car;
      stream = ent->dwChunkId[1] - '0';
      if (!(0 <= stream && stream < 2))
	continue;
      RiffData* ixxx = this->strl.ref(stream)->ixxx;
      off_t offset = ixxx->offset + 8 + sizeof(_avistdindex_chunk);
      offset += sizeof(_avistdindex_entry) * frame[stream];
      reloc.push(new RelocEntry32(offset,ent->dwOffset+8));
      reloc.push(new RelocEntry32(offset+4,ent->dwSize));
      frame[stream]++;
    }
  }
  {
    Cons<AviStrEnt*>* hd = &this->strl.head;
    Cons<AviStrEnt*>* tl = hd->cdr;

    while (tl) {
      hd = tl; tl = hd->cdr;
      AviStrEnt* ent = hd->car;
      off_t offset = ent->indx->offset + 8 + sizeof(_avisuperindex_chunk);
      reloc.push(new RelocEntry64(offset,ent->ixxx->offset));
      if (flags & OPT_AVI_IXSZBUG) {
	reloc.push(new RelocEntry32(offset+8,ent->ixxx->cb+8));
      } else {
	reloc.push(new RelocEntry32(offset+8,ent->ixxx->cb-8));
      }
      reloc.push(new RelocEntry32(offset+12,totalFrame));
      reloc.push(new RelocEntry32(ent->ixxx->offset+12,totalFrame));
    }
  }
  {
    RiffData* idx1 = this->ref("idx1");
    RiffData* ix00 = this->strl.ref("vids")->ixxx;
    reloc.push(new RelocEntry32(idx1->offset+16,ix00->offset));
  }
  this->relocate(fd,&reloc,flags);
  return 0;
}
