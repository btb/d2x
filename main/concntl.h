#ifndef _CONCNTL_H_
#define _CONCNTL_H_ 1

#include "fix.h"


#define CONCNTL_NUM_CONTROLS 29

typedef enum
{
	CONCNTL_LOOKDOWN,   // Pitch forward
	CONCNTL_LOOKUP,     // Pitch backward
	CONCNTL_LEFT,       // Turn left
	CONCNTL_RIGHT,      // Turn right
	CONCNTL_STRAFE,     // Slide on
	CONCNTL_MOVELEFT,   // Slide left
	CONCNTL_MOVERIGHT,  // Slide right
	CONCNTL_MOVEUP,     // Slide up
	CONCNTL_MOVEDOWN,   // Slide down
	CONCNTL_BANK,       // Bank on
	CONCNTL_BANKLEFT,   // Bank left
	CONCNTL_BANKRIGHT,  // Bank right
	CONCNTL_ATTACK,     // Fire primary
	CONCNTL_ATTACK2,    // Fire secondary
	CONCNTL_FLARE,      // Fire flare
	CONCNTL_FORWARD,    // Accelerate
	CONCNTL_BACK,       // Reverse
	CONCNTL_BOMB,       // Drop bomb
	CONCNTL_REARVIEW,   // Rear view
	CONCNTL_CRUISEUP,   // Cruise faster
	CONCNTL_CRUISEDOWN, // Cruise slower
	CONCNTL_CRUISEOFF,  // Cruise off
	CONCNTL_AUTOMAP,    // Automap
	CONCNTL_AFTERBURN,  // Afterburner
	CONCNTL_CYCLE,      // Cycle primary
	CONCNTL_CYCLE2,     // Cycle secondary
	CONCNTL_HEADLIGHT,  // Headlight
	CONCNTL_NRGSHIELD,  // Energy->Shield
	CONCNTL_TOGGLEBOMB, // Toggle bomb
} console_control;


fix console_control_down_time(console_control control);
unsigned int console_control_down_count(console_control control);
unsigned int console_control_up_count(console_control control);
unsigned int console_control_state(console_control control);
void console_control_set_state(console_control control, int state);
void console_control_init(void);


#endif
