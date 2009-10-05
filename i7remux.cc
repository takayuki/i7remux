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
#include <err.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "AviFile.h"

void
version(void)
{
  fprintf(stderr,"%s\n",PACKAGE_VERSION);
  exit(1);
}

void
usage(void)
{
  
  fprintf(stderr,
"usage: i7remux [-dhvxyV] [[-i] <input avi>] [[-o] <output avi>]\n"
"  -d   dry run (read input, but do not actually write output)\n"
"  -v   verbose (useful with -d to parse and verify input)\n"
"  -x   ignore MP3 frame boundary (simulate jetAudio; NOT recommended)\n"
"  -y   overwrite output\n"
"  -h   print this message\n"
"  -V   print version\n"
	  );
  exit(1);
}

int
main(int argc,char* argv[])
{
  int ch;
  int opts = (OPT_AVI_IDX1ABS|OPT_AVI_IDX1WFG|OPT_AVI_IXSZBUG);
  char* input = 0;
  char* output = 0;
  int in;
  int out;
  AviFile src;

  while ((ch = getopt(argc,argv,"dhi:o:vxyV")) != -1) {
    switch (ch) {
    case 'd':
      opts |= OPT_AVI_DRYRUN;
      break;
    case 'i':
      input = optarg;
      break;
    case 'o':
      output = optarg;
      break;
    case 'v':
      opts |= OPT_AVI_VERBOSE;
      break;
    case 'x':
      opts |= OPT_AVI_XMP3FRM;
      break;
    case 'y':
      opts |= OPT_AVI_OVERWRT;
      break;
    case 'V':
      version();
    default:
      usage();
    }
  }
  argc -= optind;
  argv += optind;

  if (argc == 1) {
    if (input == 0) {
      input = argv[0];
    } else if (output == 0) {
      output = argv[0];
    } else {
      usage();
    }
  } else if (argc == 2) {
    if (input == 0 && output == 0) {
      input = argv[0]; output = argv[1];
    } else {
      usage();
    }
  } else if (2 < argc) {
    usage();
  }

  if (opts & OPT_AVI_DRYRUN) {
    if (input == 0)
      usage();
    in = open(input,O_RDONLY);
    if (in == -1)
      err(1,"open");
    src.read(in,opts);
  } else {
    if (input == 0 || output == 0)
      usage();
    in = open(input,O_RDONLY);
    if (in == -1)
      err(1,"open");
    if (opts & OPT_AVI_OVERWRT) {
      out = open(output,(O_RDWR|O_CREAT|O_TRUNC),0644);
    } else {
      out = open(output,(O_RDWR|O_EXCL|O_CREAT|O_TRUNC),0644);
    }
    if (out == -1) 
      err(1,"open");
    src.read(in,opts);
    src.i7remux(out,opts);
  }
  return 0;
}
