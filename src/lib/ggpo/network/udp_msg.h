/* -----------------------------------------------------------------------
 * GGPO.net (http://ggpo.net)  -  Copyright 2009 GroundStorm Studios, LLC.
 *
 * Use of this software is governed by the MIT license that can be found
 * in the LICENSE file.
 */

#ifndef _UDP_MSG_H
#define _UDP_MSG_H

#include "game_input.h"

//want GAMEINPUT_MAX_BYTES*UDP_MSG_MAX_FRAMES + rest of the input struct size to be < the steam max packet size (1400)
#define UDP_MSG_MAX_FRAMES           4
#define UDP_MSG_MAX_PLAYERS          6

#pragma pack(push, 1)

struct UdpMsg
{
   enum MsgType {
      Invalid       = 0,
      SyncRequest   = 1,
      SyncReply     = 2,
      Input         = 3,
      QualityReport = 4,
      QualityReply  = 5,
      KeepAlive     = 6,
      InputAck      = 7,
   };

   struct connect_status {
      unsigned int   disconnected:1;
      int            last_frame:31;
   };

   struct {
      uint16         magic;
      uint16         sequence_number;
      uint8          type;            /* packet type */
   } hdr;
   union {
      struct {
         uint32      random_request;  /* please reply back with this random data */
         uint16      remote_magic;
         uint8       remote_endpoint;
      } sync_request;
      
      struct {
         uint32      random_reply;    /* OK, here's your random data back */
      } sync_reply;
      
      struct {
         int8        frame_advantage; /* what's the other guy's frame advantage? */
         uint32      ping;
      } quality_report;
      
      struct {
         uint32      pong;
      } quality_reply;

      struct {
         connect_status    peer_connect_status[UDP_MSG_MAX_PLAYERS];

         uint32            start_frame;

         int               disconnect_requested:1;
         int               ack_frame:31;

         uint16            size; // this is used by PayloadSize() to prevent us from sending more data then needed
         uint16            input_size; // XXX: shouldn't be in every single packet!
         uint8             bits[GAMEINPUT_MAX_BYTES*UDP_MSG_MAX_FRAMES]; /* must be last */
      } input;

      struct {
         int               ack_frame:31;
      } input_ack;

   } u;

public:
   int PacketSize() {
      return sizeof(hdr) + PayloadSize();
   }

   int PayloadSize() {
       int size;

      switch (hdr.type) {
      case SyncRequest:   return sizeof(u.sync_request);
      case SyncReply:     return sizeof(u.sync_reply);
      case QualityReport: return sizeof(u.quality_report);
      case QualityReply:  return sizeof(u.quality_reply);
      case InputAck:      return sizeof(u.input_ack);
      case KeepAlive:     return 0;
      case Input:
          size = (int)((char*)&u.input.bits - (char*)&u.input);
          size += u.input.size;
          return size;
      }
      ASSERT(false);
      return 0;
   }

   UdpMsg(MsgType t) { hdr.type = (uint8)t; }
};

#pragma pack(pop)

#endif   
