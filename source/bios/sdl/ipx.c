

#include <SDL.h>

#include "pstypes.h"
#include "ipx.h"
#include "pstypes.h"


// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Data
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Functions
// ---------------------------------------------------------------------------

ubyte *ipx_get_my_local_address(void)
{
   return NULL;
}

ubyte *ipx_get_my_server_address(void)
{
   return NULL;
}

void ipx_get_local_target(ubyte *server, ubyte *node, ubyte *local_target)
{
   memset(local_target, 0, 6);
}

//---------------------------------------------------------------
// Initializes all IPX internals.
// If socket_number==0, then opens next available socket.
// Returns: 0  if successful.
//          -1 if socket already open.
//          -2 if socket table full.
//          -3 if IPX not installed.
//          -4 if couldn't allocate memory
//          -5 if error with getting internetwork address
int ipx_init(int socket_number, int show_address)
{
   return -1;
}


// ----------------------------------------------------------------------------
// Listen/Retrieve Packet Functions
// ----------------------------------------------------------------------------

int ipx_get_packet_data(ubyte *data)
{
   return 0;
}


// ----------------------------------------------------------------------------
// Send IPX Packet Functions
// ----------------------------------------------------------------------------

void ipx_send_packet_data(ubyte *data, int datasize, ubyte *network,
                          ubyte *address, ubyte *immediate_address)
{
}


void ipx_send_broadcast_packet_data(ubyte *data, int datasize)
{
}


// Functions Sends a non-localized packet... needs 4 byte server,
// 6 byte address

void ipx_send_internetwork_packet_data(ubyte *data, int datasize,
                                       ubyte * server, ubyte *address)
{
}


// ---------------------------------------------------------------------------
// Read IPX User file stuff
// ---------------------------------------------------------------------------

int ipx_change_default_socket(ushort socket_number)
{
   return -1;
}


void ipx_read_user_file(char *filename)
{
}


void ipx_read_network_file(char *filename)
{
}
