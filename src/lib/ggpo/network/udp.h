/* -----------------------------------------------------------------------
 * GGPO.net (http://ggpo.net)  -  Copyright 2009 GroundStorm Studios, LLC.
 *
 * Use of this software is governed by the MIT license that can be found
 * in the LICENSE file.
 */

#ifndef _UDP_H
#define _UDP_H

#include "poll.h"
#include "udp_msg.h"
#include "ggponet.h"
#include "ring_buffer.h"
#pragma warning( push )
#pragma warning( disable : 4996 )
#include "steam_api.h"
#pragma warning( pop )

class Udp : public IPollSink
{
public:
   struct Stats {
      int      bytes_sent;
      int      packets_sent;
      float    kbps_sent;
   };

   struct Callbacks {
      virtual ~Callbacks() { }
      virtual void OnMsg(SteamNetworkingIdentity &from, UdpMsg *msg, int len) = 0;
   };


protected:
   void Log(const char *fmt, ...);

public:
   Udp();

   void Init(ISteamNetworkingMessages* steamNetMessages, Poll *p, Callbacks *callbacks);
   
   void SendTo(const void* buffer, int len, const SteamNetworkingIdentity &dst);

   virtual bool OnLoopPoll(void *cookie);

public:
   ~Udp(void);

protected:
   // Network transmission information
   ISteamNetworkingMessages* _steamNetMessages;

   // state management
   Callbacks      *_callbacks;
   Poll           *_poll;
};

#endif
