/*
 *
 * parts from:
 * ipx.h header file for IPX for the DOS emulator
 * 		Tim Bird, tbird@novell.com
 *
 */

#ifndef _IPX_DOSEMU
#define _IPX_DOSEMU

#define MAX_PACKET_DATA		1500

typedef struct IPXAddressStruct {
  u_char Network[4] __pack__;
  u_char Node[6] __pack__;
  u_char Socket[2] __pack__;
} IPXAddress_t;

typedef struct IPXPacketStructure {
  u_short Checksum __pack__;
  u_short Length __pack__;
  u_char TransportControl __pack__;
  u_char PacketType __pack__;
  IPXAddress_t Destination __pack__;
  IPXAddress_t Source __pack__;
} IPXPacket_t;

typedef struct ipx_socket_struct {
#ifdef DOSEMU
  struct ipx_socket_struct *next;
  far_t listenList;
  int listenCount;
  far_t AESList;
  int AESCount;
  u_short PSP;
#endif  
  u_short socket;
  int fd;
} ipx_socket_t;

#include "mono.h"
#ifndef NMONO
#define n_printf(format, args...) _mprintf(1, format, ## args)
#else
#define n_printf(format, args...) fprintf(stderr,format, ## args)
#endif
#define enter_priv_on()
#define leave_priv_setting()

#endif
