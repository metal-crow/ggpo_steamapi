/* -----------------------------------------------------------------------
 * GGPO.net (http://ggpo.net)  -  Copyright 2009 GroundStorm Studios, LLC.
 *
 * Use of this software is governed by the MIT license that can be found
 * in the LICENSE file.
 */

#ifndef _GAMEINPUT_H
#define _GAMEINPUT_H

#include <stdio.h>
#include <memory.h>
#include "ggponet.h"

#define GAMEINPUT_MAX_BYTES      300
#define GAMEINPUT_MAX_PLAYERS    6

extern GGPOSessionCallbacks GameInputCallbacks;

struct GameInput {
   enum Constants {
      NullFrame = -1
   };
   int      frame;
   bool     duplicate;
   bool     local;
   int      size; /* size in bytes of the entire input for all players */
   uint8    bits[GAMEINPUT_MAX_BYTES * GAMEINPUT_MAX_PLAYERS];

   bool is_null() { return frame == NullFrame; }
   void init(int frame, char *bits, int size, int offset);
   void init(int frame, char *bits, int size);
   void erase() { memset(bits, 0, sizeof(bits)); }
   void desc(char *buf, size_t buf_size, bool show_frame = true) const;
   void log(char *prefix, bool show_frame = true) const;
   bool equal(GameInput &input, bool bitsonly = false);
};

#endif
