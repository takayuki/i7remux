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
#ifndef _AVISTRLIST_H
#define _AVISTRLIST_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#include "Cons.h"
#include "RiffChunk.h"
#include "RiffList.h"

struct _avimainheader {
  uint32_t dwMicroSecPerFrame;
  uint32_t dwMaxBytesPerSec;
  uint32_t dwPaddingGranularity;
  uint32_t dwFlags;
  uint32_t dwTotalFrames;
  uint32_t dwInitialFrames;
  uint32_t dwStreams;
  uint32_t dwSuggestedBufferSize;
  uint32_t dwWidth;
  uint32_t dwHeight;
  uint32_t dwReserved[4];
} __attribute__ ((packed));

struct _avistreamheader {
  char fccType[4];
  char fccHandler[4];
  uint32_t dwFlags;
  uint16_t wPriority;
  uint16_t wLanguage;
  uint32_t dwInitialFrames;
  uint32_t dwScale;
  uint32_t dwRate;
  uint32_t dwStart;
  uint32_t dwLength;
  uint32_t dwSuggestedBufferSize;
  uint32_t dwQuality;
  uint32_t dwSampleSize;
  struct {
    int16_t left;
    int16_t top;
    int16_t right;
    int16_t bottom;
  } rcFrame __attribute__ ((packed));
} __attribute__ ((packed));

struct _bitmapinfoheader {
  uint32_t biSize;
  uint32_t biWidth;
  uint32_t biHeight;
  uint16_t biPlanes;
  uint16_t biBitCount;
  char     biCompression[4];
  uint32_t biSizeImage;
  uint32_t biXPelsPerMeter;
  uint32_t biYPelsPerMeter;
  uint32_t biClrUsed;
  uint32_t biClrImportant;
} __attribute__ ((packed));

struct _waveformatex {
  uint16_t wFormatTag;
  uint16_t nChannels;
  uint32_t nSamplesPerSec;
  uint32_t nAvgBytesPerSec;
  uint16_t nBlockAlign;
  uint16_t wBitsPerSample;
  uint16_t cbSize;
} __attribute__ ((packed));

struct _mpeglayer3waveformat {
  uint16_t wID;
  uint32_t fdwFlags;
  uint16_t nBlockSize;
  uint16_t nFramesPerBlock;
  uint16_t nCodecDelay;
}  __attribute__ ((packed));

union _avistreamformat {
  _bitmapinfoheader bi;
  _waveformatex wf;
} __attribute__ ((packed));

#define AVI_INDEX_OF_INDEXES 0x00
#define AVI_INDEX_OF_CHUNKS  0x01
#define AVI_INDEX_IS_DATA    0x80

struct _aviindex_entry {
  uint32_t adw[];
} __attribute__ ((packed));

struct _aviindex_chunk {
  uint16_t wLongsPerEntry;
  uint8_t  bIndexSubType;
  uint8_t  bIndexType;
  uint32_t nEntriesInUse;
  uint32_t dwChunkId;
  uint32_t dwReserved[3];
  struct _aviindex_entry aIndex[];
} __attribute__ ((packed));

struct _avisuperindex_entry {
  uint64_t qwOffset;
  uint32_t dwSize;
  uint32_t dwDuration;
} __attribute__ ((packed));

struct _avisuperindex_chunk {
  uint16_t wLongsPerEntry;
  uint8_t  bIndexSubType;
  uint8_t  bIndexType;
  uint32_t nEntriesInUse;
  uint32_t dwChunkId;
  uint32_t dwReserved[3];
  struct _avisuperindex_entry aIndex[];
} __attribute__ ((packed));

struct _avistdindex_entry {
  uint32_t dwOffset;
  uint32_t dwSize;
} __attribute__ ((packed));

struct _avistdindex_chunk {
  uint16_t wLongsPerEntry;
  uint8_t  bIndexSubType;
  uint8_t  bIndexType;
  uint32_t nEntriesInUse;
  uint32_t dwChunkId;
  uint64_t qwBaseOffset;
  uint32_t dwReserved3;
  struct _avistdindex_entry aIndex[];
} __attribute__ ((packed));

struct _odmlextendedaviheader {
  uint32_t dwTotalFrames;
} __attribute__ ((packed));

void show_avi_header(_avimainheader*);
void show_stream_header(_avistreamheader*);
void show_vids_format(_avistreamformat*);
void show_auds_format(_avistreamformat*);
void show_super_index(_aviindex_chunk*);
void show_std_index(_avistdindex_chunk*);

class AviStrEnt {
 public:
  RiffList* strl;
  
 public:
  RiffChunk* strh;
  RiffChunk* strf;
  RiffChunk* indx;
  RiffChunk* ixxx;

 public:
  AviStrEnt(RiffChunk*);
  ~AviStrEnt(void);

 private:
  AviStrEnt(const AviStrEnt&);
  const AviStrEnt& operator=(const AviStrEnt&);

 public:
 RiffChunk* append(RiffChunk*);
};

class AviStrList {
 public:
  Cons<AviStrEnt*> head;

 public:
  AviStrList(void);
  ~AviStrList(void);

 private:
  AviStrList(const AviStrList&);
  const AviStrList& operator=(const AviStrList&);

 public:
  AviStrEnt* append(AviStrEnt*);
  AviStrEnt* ref(int);
  AviStrEnt* ref(const char*);
  AviStrEnt* last(void);
};

#endif
