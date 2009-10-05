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
#include <stdio.h>
#include "AviStrList.h"
#include "RiffChunkMem.h"

void
show_avi_header(_avimainheader* avih)
{
  printf("---- AVI Main Header ----\n"
	 "dwMicroSecPerFrame=%u\n"
	 "dwMaxBytesPerSec=%u\n"
	 "dwPaddingGranularity=%u\n"
	 "dwFlags=%u\n"
	 "dwTotalFrames=%u\n"
	 "dwInitialFrames=%u\n"
	 "dwStreams=%u\n"
	 "dwSuggestedBufferSize=%u\n"
	 "dwWidth=%u\n"
	 "dwHeight=%u\n"
	 "dwReserved[0]=%u\n"
	 "dwReserved[1]=%u\n"
	 "dwReserved[2]=%u\n"
	 "dwReserved[3]=%u\n",
	 avih->dwMicroSecPerFrame,
	 avih->dwMaxBytesPerSec,
	 avih->dwPaddingGranularity,
	 avih->dwFlags,
	 avih->dwTotalFrames,
	 avih->dwInitialFrames,
	 avih->dwStreams,
	 avih->dwSuggestedBufferSize,
	 avih->dwWidth,
	 avih->dwHeight,
	 avih->dwReserved[0],
	 avih->dwReserved[1],
	 avih->dwReserved[2],
	 avih->dwReserved[3]);
}

void
show_stream_header(_avistreamheader* strh)
{
  printf("---- Stream Header ----\n"
	 "fccType=%c%c%c%c\n"
	 "fccHandler=%c%c%c%c\n"
	 "dwFlags=%u\n"
	 "wPriority=%u\n"
	 "wLanguage=%u\n"
	 "dwInitialFrames=%u\n"
	 "dwScale=%u\n"
	 "dwRate=%u\n"
	 "dwStart=%u\n"
	 "dwLength=%u\n"
	 "dwSuggestedBufferSize=%u\n"
	 "dwQuality=%u\n"
	 "dwSampleSize=%u\n"
	 "left=%d\n"
	 "top=%d\n"
	 "right=%d\n"
	 "bottom=%d\n",
	 strh->fccType[0],
	 strh->fccType[1],
	 strh->fccType[2],
	 strh->fccType[3],
	 strh->fccHandler[0],
	 strh->fccHandler[1],
	 strh->fccHandler[2],
	 strh->fccHandler[3],
	 strh->dwFlags,
	 strh->wPriority,
	 strh->wLanguage,
	 strh->dwInitialFrames,
	 strh->dwScale,
	 strh->dwRate,
	 strh->dwStart,
	 strh->dwLength,
	 strh->dwSuggestedBufferSize,
	 strh->dwQuality,
	 strh->dwSampleSize,
	 strh->rcFrame.left,
	 strh->rcFrame.top,
	 strh->rcFrame.right,
	 strh->rcFrame.bottom);
}

void
show_vids_format(_avistreamformat* strf)
{
  printf("---- Stream Format ----\n"
	 "biSize=%u\n"
	 "biWidth=%u\n"
	 "biHeight=%u\n"
	 "biPlanes=%u\n"
	 "biBitCount=%u\n"
	 "biCompression=%c%c%c%c\n"
	 "biSizeImage=%u\n"
	 "biXPelsPerMeter=%u\n"
	 "biYPelsPerMeter=%u\n"
	 "biClrUsed=%u\n"
	 "biClrImportant=%u\n",
	 strf->bi.biSize,
	 strf->bi.biWidth,
	 strf->bi.biHeight,
	 strf->bi.biPlanes,
	 strf->bi.biBitCount,
	 strf->bi.biCompression[0],
	 strf->bi.biCompression[1],
	 strf->bi.biCompression[2],
	 strf->bi.biCompression[3],
	 strf->bi.biSizeImage,
	 strf->bi.biXPelsPerMeter,
	 strf->bi.biYPelsPerMeter,
	 strf->bi.biClrUsed,
	 strf->bi.biClrImportant);
}

void
show_auds_format(_avistreamformat* strf)
{
  printf("---- Stream Format ----\n"
	 "wFormatTag=%u\n"
	 "nChannels=%u\n"
	 "nSamplesPerSec=%u\n"
	 "nAvgBytesPerSec=%u\n"
	 "nBlockAlign=%u\n"
	 "wBitsPerSample=%u\n"
	 "cbSize=%u\n",
	 strf->wf.wFormatTag,
	 strf->wf.nChannels,
	 strf->wf.nSamplesPerSec,
	 strf->wf.nAvgBytesPerSec,
	 strf->wf.nBlockAlign,
	 strf->wf.wBitsPerSample,
	 strf->wf.cbSize);
  if (strf->wf.wFormatTag == 0x55 && strf->wf.cbSize == 12) {
    _mpeglayer3waveformat* mp3 =
      (_mpeglayer3waveformat*)(((char*)strf) + sizeof(_avistreamformat));
    printf("mp3.wID=%u\n"
	   "mp3.fdwFlags=%u\n"
	   "mp3.nBlockSize=%u\n"
	   "mp3.nFramesPerBlock=%u\n"
	   "mp3.nCodecDelay=%u\n",
	   mp3->wID,
	   mp3->fdwFlags,
	   mp3->nBlockSize,
	   mp3->nFramesPerBlock,
	   mp3->nCodecDelay);
  }
}

void
show_super_index(_aviindex_chunk* indx)
{
  printf("---- Super Index ----\n"
	 "wLongsPerEntry=%u\n"
	 "bIndexSubType=%u\n"
	 "bIndexType=%u\n"
	 "nEntriesInUse=%u\n"
	 "dwChunkId=%c%c%c%c\n"
	 "dwReserved[0]=%u\n"
	 "dwReserved[1]=%u\n"
	 "dwReserved[2]=%u\n",
	 indx->wLongsPerEntry,
	 indx->bIndexSubType,
	 indx->bIndexType,
	 indx->nEntriesInUse,
	 ((indx->dwChunkId&(0xFF))),
	 ((indx->dwChunkId&(0xFF<<8))>>8),
	 ((indx->dwChunkId&(0xFF<<16))>>16),
	 ((indx->dwChunkId&(0xFF<<24))>>24),
	 indx->dwReserved[0],
	 indx->dwReserved[1],
	 indx->dwReserved[2]);
  if (indx->bIndexType == 0 && indx->wLongsPerEntry == 4) {
    _avisuperindex_chunk* sindx = (_avisuperindex_chunk*)indx;

    for (uint32_t i = 0; i < indx->nEntriesInUse; i++) {
      printf("aIndex[%u].qwOffset=%llu\n",i,sindx->aIndex[i].qwOffset);
      printf("aIndex[%u].dwSize=%u\n",i,sindx->aIndex[i].dwSize);
      printf("aIndex[%u].dwDuration=%u\n",i,sindx->aIndex[i].dwDuration);
    }
  }
}

void
show_std_index(_avistdindex_chunk* indx)
{
  printf("---- Standard Index ----\n"
	 "wLongsPerEntry=%u\n"
	 "bIndexSubType=%u\n"
	 "bIndexType=%u\n"
	 "nEntriesInUse=%u\n"
	 "dwChunkId=%c%c%c%c\n"
	 "qwBaseOffset=%llu\n"
	 "dwReserved3=%u\n",
	 indx->wLongsPerEntry,
	 indx->bIndexSubType,
	 indx->bIndexType,
	 indx->nEntriesInUse,
	 ((indx->dwChunkId&(0xFF))),
	 ((indx->dwChunkId&(0xFF<<8))>>8),
	 ((indx->dwChunkId&(0xFF<<16))>>16),
	 ((indx->dwChunkId&(0xFF<<24))>>24),
	 indx->qwBaseOffset,
	 indx->dwReserved3);
  for (uint32_t i = 0; i < indx->nEntriesInUse; i++) {
    printf("aIndex[%u].dwOffset=%u\n",i,indx->aIndex[i].dwOffset);
    printf("aIndex[%u].dwSize=%u\n",i,indx->aIndex[i].dwSize);
  }
}

AviStrEnt::AviStrEnt(RiffChunk* strh)
{
  this->strl = 0;
  this->strh = strh;
  this->strf = 0;
  this->indx = 0;
  this->ixxx = 0;
}

AviStrEnt::~AviStrEnt(void)
{
  if (false) {
    if (this->strl) delete this->strl;
    if (this->strf) delete this->strf;
    if (this->strh) delete this->strh;
    if (this->indx) delete this->indx;
    if (this->ixxx) delete this->ixxx;
  }
}

AviStrList::AviStrList(void)
{
}

AviStrList::~AviStrList(void)
{
  Cons<AviStrEnt*>* hd = &this->head;  
  Cons<AviStrEnt*>* tl = hd->cdr;
  AviStrEnt* car;

  while (tl) {
    hd = tl; tl = hd->cdr;
    car = hd->car;
    delete hd;
    if (false) { delete car; }
  }
}

AviStrEnt*
AviStrList::append(AviStrEnt* ent)
{
  return this->head.append(ent);
}

AviStrEnt*
AviStrList::ref(const char* type)
{
  Cons<AviStrEnt*>* hd = &this->head;
  Cons<AviStrEnt*>* tl = hd->cdr;
  AviStrEnt* ent;
  RiffChunkMem* chunkmem;
  _avistreamheader* strh;

  while (tl) {
    hd = tl; tl = hd->cdr;
    ent = hd->car;
    chunkmem = dynamic_cast<RiffChunkMem*>(ent->strh);
    if (chunkmem) {
      strh = (_avistreamheader*)chunkmem->data;
      if (!memcmp(strh->fccType,type,4))
	return ent;
    }
  }
  return 0;
}

AviStrEnt*
AviStrList::ref(int idx)
{
  return this->head.ref(idx);
}

AviStrEnt*
AviStrList::last(void)
{
  return this->head.last();
}
