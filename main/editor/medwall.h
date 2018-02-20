/*
THE COMPUTER CODE CONTAINED HEREIN IS THE SOLE PROPERTY OF PARALLAX
SOFTWARE CORPORATION ("PARALLAX").  PARALLAX, IN DISTRIBUTING THE CODE TO
END-USERS, AND SUBJECT TO ALL OF THE TERMS AND CONDITIONS HEREIN, GRANTS A
ROYALTY-FREE, PERPETUAL LICENSE TO SUCH END-USERS FOR USE BY SUCH END-USERS
IN USING, DISPLAYING,  AND CREATING DERIVATIVE WORKS THEREOF, SO LONG AS
SUCH USE, DISPLAY OR CREATION IS FOR NON-COMMERCIAL, ROYALTY OR REVENUE
FREE PURPOSES.  IN NO EVENT SHALL THE END-USER USE THE COMPUTER CODE
CONTAINED HEREIN FOR REVENUE-BEARING PURPOSES.  THE END-USER UNDERSTANDS
AND AGREES TO THE TERMS HEREIN AND ACCEPTS THE SAME BY USE OF THIS FILE.
COPYRIGHT 1993-1998 PARALLAX SOFTWARE CORPORATION.  ALL RIGHTS RESERVED.
*/

/*
 *
 * Created from version 1.6 of main\wall.h
 *
 */

#ifndef _MEDWALL_H
#define _MEDWALL_H

#include "inferno.h"


// Restores all the walls to original status
extern int wall_restore_all(void);

// Reset a wall.
extern void wall_reset(segment *seg, int side);

// Adds a removable wall (medwall.c)
extern int wall_add_removable(void);

// Adds a door (medwall.c)
extern int wall_add_door(void);

// Adds an illusory wall (medwall.c)
extern int wall_add_illusion(void);

// Removes a removable wall (medwall.c) 
extern int wall_remove_blastable(void);

// Adds a wall. (visually)
extern int wall_add_to_curside(void);
extern int wall_add_to_markedside(sbyte type);
 
// Removes a wall. (visually)
extern int wall_remove(void);

// Removes a specific side.
int wall_remove_side(segment *seg, short side);

extern int bind_wall_to_control_center(void);

extern void close_wall_window(void);

extern void do_wall_window(void);

extern int wall_link_doors(void);
extern int wall_unlink_door(void);
extern void copy_group_walls(int old_group, int new_group);

#endif

