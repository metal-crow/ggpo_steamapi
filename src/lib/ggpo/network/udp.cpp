/* -----------------------------------------------------------------------
 * GGPO.net (http://ggpo.net)  -  Copyright 2009 GroundStorm Studios, LLC.
 *
 * Use of this software is governed by the MIT license that can be found
 * in the LICENSE file.
 */

#include "types.h"
#include "udp.h"

Udp::Udp() :
    _steamNetMessages(NULL),
    _steamFriends(NULL),
   _callbacks(NULL)
{
}

Udp::~Udp(void)
{
}

typedef void* gfGetSteamInterface(int iSteamUser, int iUnkInt, const char* pcVersion, const char* pcInterface);
typedef ISteamClient* gfCreateSteamInterface(const char* pSteamClientVer, uint32_t iUnkZero);

bool
Udp::Init(Poll *poll, Callbacks *callbacks)
{
   _callbacks = callbacks;

   _poll = poll;
   _poll->RegisterLoop(this);

   Log("connecting to steam messages.\n");

   if (!SteamAPI_GetHSteamUser() || !SteamAPI_GetHSteamPipe())
   {
       return false;
   }

   HMODULE hSteamClient64 = GetModuleHandleA("steamclient64.dll");
   if (!hSteamClient64)
   {
       return false;
   }

   gfCreateSteamInterface* fCreateSteamInterface = (gfCreateSteamInterface*)GetProcAddress(hSteamClient64, "CreateInterface");
   if (!fCreateSteamInterface)
   {
       return false;
   }

   ISteamClient* SteamClient = fCreateSteamInterface("SteamClient017", 0);
   if (!SteamClient)
   {
       return false;
   }

   _steamNetMessages = (ISteamNetworkingMessages*)SteamClient->GetISteamGenericInterface(1, 1, STEAMNETWORKINGMESSAGES_INTERFACE_VERSION);
   if (!_steamNetMessages)
   {
       return false;
   }

   return true;
}

void
Udp::SendTo(const void* buffer, int len, const SteamNetworkingIdentity &dst)
{
   int flags = k_nSteamNetworkingSend_AutoRestartBrokenSession | k_nSteamNetworkingSend_UnreliableNoNagle;
   EResult res = _steamNetMessages->SendMessageToUser(dst, buffer, len, flags, 1);
   if (res != k_EResultOK) {
      Log("unknown error in sendto (erro: %d).\n", res);
      ASSERT(FALSE && "Unknown error in sendto");
   }
   Log("sent packet length %d to %llx (ret:%d).\n", len, dst.GetSteamID64(), res);
}

bool
Udp::OnLoopPoll(void *cookie)
{
   SteamNetworkingMessage_t* recv_message[1];
   SteamNetworkingIdentity recv_id;
   uint32_t len;

   for (;;) {
      int nummsgs = _steamNetMessages->ReceiveMessagesOnChannel(1, recv_message, 1);

      if (nummsgs > 0 && recv_message[0] != nullptr) {
         len = recv_message[0]->GetSize();
         recv_id = recv_message[0]->m_identityPeer;

         Log("recvfrom returned (len:%d  from:%llx).\n", len, recv_id.GetSteamID64());

         UdpMsg *msg = (UdpMsg *)recv_message[0]->GetData();
         _callbacks->OnMsg(recv_id, msg, len);

         recv_message[0]->Release();
         recv_message[0] = NULL;
      } 
   }
}

void
Udp::Log(const char *fmt, ...)
{
   char buf[1024];
   size_t offset;
   va_list args;

   strcpy_s(buf, "udp | ");
   offset = strlen(buf);
   va_start(args, fmt);
   vsnprintf(buf + offset, ARRAY_SIZE(buf) - offset - 1, fmt, args);
   buf[ARRAY_SIZE(buf)-1] = '\0';
   ::Log(buf);
   va_end(args);
}
