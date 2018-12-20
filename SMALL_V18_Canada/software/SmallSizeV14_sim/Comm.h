#ifndef _COMM_H_
#define _COMM_H_

#include <string.h>
#include "Config.h"
#include "Robot.h"
#include "Misc.h"
#include "nRF2401.h"
#include "Packet.h"
#include "Action.h"


#define MAX_BUFFER_LEN 25
#define nRF2401_BUFFER_LEN 256

struct _comm_
{
	unsigned char buffer[ MAX_BUFFER_LEN ];
	unsigned char buffer_pos;

//	unsigned char status;

//	int packet_error; //count number of bad packet received

} g_comm;


void init_comm();
void do_comm();
void is_comm_overflow();
void set_receive_mode();

#endif