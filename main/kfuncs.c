/*
 * $Source: f:/miner/source/main/editor/rcs/kfuncs.c $
 * $Revision: 2.1 $
 * $Author: yuan $
 * $Date: 1995/03/08 16:07:23 $
 * 
 * .
 * 
 * $Log: kfuncs.c $
 * Revision 2.1  1995/03/08  16:07:23  yuan
 * Added segment sizing default functions.
 * 
 * Revision 2.0  1995/02/27  11:34:45  john
 * Version 2.0! No anonymous unions, Watcom 10.0, with no need
 * for bitmaps.tbl.
 * 
 * Revision 1.112  1995/01/12  12:10:31  yuan
 * Added coop object capability.
 * 
 * Revision 1.111  1994/11/27  23:17:16  matt
 * Made changes for new mprintf calling convention
 * 
 * Revision 1.110  1994/11/23  12:17:22  mike
 * bind retmap functions.
 * 
 * Revision 1.109  1994/11/19  15:20:54  mike
 * rip out unused code and data.
 * 
 * Revision 1.108  1994/11/17  11:39:02  matt
 * Ripped out code to load old mines
 * 
 * Revision 1.107  1994/10/27  10:06:12  mike
 * kill macro stuff.
 * 
 * Revision 1.106  1994/10/18  15:31:43  mike
 * Bind mine correction function to test keypad, key=1.
 * 
 * Revision 1.105  1994/09/26  23:22:06  matt
 * Added functions to keep player's starting position from getting messed up
 * 
 * Revision 1.104  1994/09/24  14:15:41  mike
 * Custom colored object support.
 * 
 * Revision 1.103  1994/09/23  18:03:54  yuan
 * Finished wall checking code.
 * 
 * Revision 1.102  1994/09/22  19:03:52  mike
 * Test function test_shortpos on ctrl-keypad3
 * 
 * Revision 1.101  1994/09/20  14:35:56  mike
 * Bind functions to keypad 2, 3.
 * 
 * Revision 1.100  1994/09/15  22:58:10  matt
 * Made new objects be oriented to their segment
 * Added keypad function to flip an object upside-down
 * 
 * Revision 1.99  1994/09/14  16:50:50  yuan
 * Added load mine only function
 * 
 * Revision 1.98  1994/08/25  21:57:52  mike
 * IS_CHILD stuff.
 * 
 * Revision 1.97  1994/08/16  18:10:52  yuan
 * Maded C place you in the center of a segment.
 * 
 * Revision 1.96  1994/08/15  17:48:01  yuan
 * Added external walls.
 * 
 * Revision 1.95  1994/08/13  17:32:38  mike
 * test function.
 * 
 * Revision 1.94  1994/08/05  21:17:57  matt
 * Allow two doors to be linked together
 * 
 * Revision 1.93  1994/08/05  18:18:52  matt
 * Made object rotation have 4x resolution, and SHIFT+rotate do old resolution.
 * 
 * Revision 1.92  1994/08/03  10:31:06  mike
 * Add texture map propagation without uv assignment.
 * 
 * Revision 1.91  1994/08/02  14:17:00  mike
 * Clean up dialog boxes, add one for Matt.
 * 
 * Revision 1.90  1994/07/30  17:04:47  mike
 * Bind key for max_depth on path testing.
 * 
 */


#pragma off (unreferenced)
static char rcsid[] = "$Id: kfuncs.c 2.1 1995/03/08 16:07:23 yuan Exp $";
#pragma on (unreferenced)

#include <stdlib.h>

#include "inferno.h"
#include "func.h"
#include "kdefs.h"
#include "segment.h"
#include "editor.h"
#include "error.h"
#include "slew.h"
#include "mono.h"
#include "gamesave.h"
#include "eobject.h"
#include "medwall.h"

// Test function prototypes (replace Test1, 2 and 3 with whatever function 
//										you wish to test.)
extern void test_create_path();
extern void test_create_all_paths();
extern void test_create_path_many();
extern void create_all_paths();
extern void test_create_all_anchors();
// extern void make_curside_bottom_side();
extern void move_object_to_mouse_click();
extern void test_create_n_segment_path();

extern void set_all_modes_to_hover(void);

int Test1() 
{
//	{ int i;
//		for (i=Highest_segment_index+1; i<MAX_SEGMENTS; i++)
//			Segments[i].segnum = -1;
//	}

	mprintf((0, "Initializing replacement list.\n"));
	init_replacements();

	return 0;
}

extern void check_for_overlapping_segments(void);
extern void init_replacements();
int Test2() 
{
//	mprintf((0, "\nCalling check_for_overlapping_segments.\n"));
//	check_for_overlapping_segments();

	mprintf((0, "Going to do texture replacements.\n"));
	do_replacements();

	return 0;
}

//extern fix fcd_test(void);
//extern void test_shortpos(void);

extern void do_replacements(void);
extern void do_replacements_all(void);

int Test3() 
{
	Int3();	//	Are you sure you want to do this?
	//	This will replace all textures in your replacement list
	//	in all mines.
	//	If you don't want to do this, set eip to the return statement
	//	and continue.

	mprintf((0, "Going to do texture replacements.\n"));
	do_replacements_all();

	return 0;
}

FUNCTION med_functions[] = {

// Test functions
{   "med-test-1",                         0,    Test1 },
{   "med-test-2",                         0,    Test2 },
{   "med-test-3",                         0,    Test3 },

// In khelp.c
{   "med-help",                         0,      DoHelp },

// In kcurve.c
{   "med-curve-init",                   0,      InitCurve },
{   "med-curve-generate",               0,      GenerateCurve },
{   "med-curve-decrease-r4",            0,      DecreaseR4 },
{   "med-curve-increase-r4",            0,      IncreaseR4 },
{   "med-curve-decrease-r1",            0,      DecreaseR1 },
{   "med-curve-increase-r1",            0,      IncreaseR1 },
{   "med-curve-delete",                 0,      DeleteCurve },
{   "med-curve-set",                    0,      SetCurve },

// In kmine.c
{   "med-mine-save",                    0,      SaveMine },
//{   "med-mine-load",                    0,      LoadMine },
{   "med-mine-menu",                    0,      MineMenu },
{   "med-mine-create-new",              0,      CreateNewMine },
//{	 "med-mine-load-old",					 0,		LoadOldMine },			
{   "med-situation-save",               0,      SaveSituation },
{   "med-situation-load",               0,      LoadSituation },
{	 "med-restore-game-state",				 0,		RestoreGameState },
//{   "load-mine-only",                    0,      LoadMineOnly   },

// In kgame.c
{   "med-game-save",                    0,      SaveGameData },
{   "med-game-load",                    0,      LoadGameData },

// In kview.c
{   "med-view-zoom-out",                0,      ZoomOut },
{   "med-view-zoom-in",                 0,      ZoomIn },
{   "med-view-move-away",               0,      MoveAway },
{   "med-view-move-closer",             0,      MoveCloser },
{   "med-view-toggle-chase",            0,      ToggleChaseMode },

// In kbuild.c
{   "med-build-bridge",                 0,      CreateBridge },
{   "med-build-joint",                  0,      FormJoint },
{   "med-build-adj-joint",              0,      CreateAdjacentJoint },
{   "med-build-sloppy-adj-joint",       0,      CreateSloppyAdjacentJoint },
{   "med-build-sloppy-adj-joint-group", 0,      CreateSloppyAdjacentJointsGroup },
{   "med-build-adj-joints-segment",     0,      CreateAdjacentJointsSegment },
{   "med-build-adj-joints-all",         0,      CreateAdjacentJointsAll },

// In segment.c
{   "med-segment-bottom",         		 0,      ToggleBottom }, //make_curside_bottom_side },
{   "med-segment-show-bottom",          0,      ToggleBottom },

// In ksegmove.c
{   "med-segmove-decrease-heading",     0,      DecreaseHeading },
{   "med-segmove-increase-heading",     0,      IncreaseHeading },
{   "med-segmove-decrease-pitch",       0,      DecreasePitch },
{   "med-segmove-increase-pitch",       0,      IncreasePitch },
{   "med-segmove-decrease-bank",        0,      DecreaseBank },
{   "med-segmove-increase-bank",        0,      IncreaseBank },

// In ksegsel.c
{   "med-segsel-next-segment",          0,      SelectCurrentSegForward },
{   "med-segsel-prev-segment",          0,      SelectCurrentSegBackward },
{   "med-segsel-next-side",             0,      SelectNextSide },
{   "med-segsel-prev-side",             0,      SelectPrevSide },
{   "med-segsel-set-marked",            0,      CopySegToMarked },
{   "med-segsel-bottom",                0,      SelectBottom },
{   "med-segsel-front",                 0,      SelectFront },
{   "med-segsel-top",                   0,      SelectTop },
{   "med-segsel-back",                  0,      SelectBack },
{   "med-segsel-left",                  0,      SelectLeft },
{   "med-segsel-right",                 0,      SelectRight },

// In ksegsize.c
{   "med-segsize-increase-length",      0,      IncreaseSegLength },
{   "med-segsize-decrease-length",      0,      DecreaseSegLength },
{   "med-segsize-decrease-width",       0,      DecreaseSegWidth },
{   "med-segsize-increase-width",       0,      IncreaseSegWidth },
{   "med-segsize-increase-height",      0,      IncreaseSegHeight },
{   "med-segsize-decrease-height",      0,      DecreaseSegHeight },

{   "med-segsize-increase-length-big",  0,      IncreaseSegLengthBig },
{   "med-segsize-decrease-length-big",  0,      DecreaseSegLengthBig },
{   "med-segsize-decrease-width-big",   0,      DecreaseSegWidthBig },
{   "med-segsize-increase-width-big",   0,      IncreaseSegWidthBig },
{   "med-segsize-increase-height-big",  0,      IncreaseSegHeightBig },
{   "med-segsize-decrease-height-big",  0,      DecreaseSegHeightBig },
{   "med-segsize-toggle-mode",  			 0,      ToggleSegSizeMode },
{   "med-segsize-perturb-curside",		 0,      PerturbCurside },
{   "med-segsize-perturb-curside-big",	 0,      PerturbCursideBig },
{   "med-segsize-increase-length-default",  0,  IncreaseSegLengthDefault },
{   "med-segsize-decrease-length-default",  0,  DecreaseSegLengthDefault },
{   "med-segsize-increase-width-default",  0,   IncreaseSegWidthDefault },
{   "med-segsize-decrease-width-default",  0,   DecreaseSegWidthDefault },
{   "med-segsize-increase-height-default",  0,   IncreaseSegHeightDefault },
{   "med-segsize-decrease-height-default",  0,   DecreaseSegHeightDefault },

//  In ktmap.c
{   "med-tmap-assign",                  0,      AssignTexture },
{   "med-tmap-assign2",                 0,      AssignTexture2 },
{   "med-tmap-clear2",                  0,      ClearTexture2 },
{   "med-tmap-propogate",               0,      PropagateTextures },
{   "med-tmap-propogate-move",          0,      PropagateTexturesMove },
{   "med-tmap-propogate-move-uvs",      0,      PropagateTexturesMoveUVs },
{   "med-tmap-propogate-move-uvs",      0,      PropagateTexturesMoveUVs },
{   "med-tmap-propogate-uvs",           0,      PropagateTexturesUVs },
{   "med-tmap-propogate-selected",      0,      PropagateTexturesSelected },

//  In wall.c
{   "med-wall-add-blastable",           0,      wall_add_blastable },
{   "med-wall-add-door",           		 0,      wall_add_door },
{   "med-wall-add-closed",         		 0,		wall_add_closed_wall },
{   "med-wall-add-external",         	 0,		wall_add_external_wall },
{   "med-wall-add-illusion",  	       0,      wall_add_illusion },
{   "med-wall-restore-all",				 0,      wall_restore_all },
{   "med-wall-remove",        			 0,      wall_remove },
{   "do-wall-dialog",						 0,      do_wall_dialog },
{   "med-link-doors",						 0,		wall_link_doors },
{   "med-unlink-door",						 0,		wall_unlink_door },
{   "check-walls",						 	 0,		check_walls },
{   "delete-all-walls",						 0,		delete_all_walls },

// In centers.c
{   "do-centers-dialog",						 0,      do_centers_dialog },

//  In switch.c
//{   "med-trigger-add-damage",           0,      trigger_add_damage },
//{   "med-trigger-add-exit",	          0,      trigger_add_exit },
//{   "med-trigger-control",     			 0,      trigger_control },
//{   "med-trigger-remove",		          0,      trigger_remove },
//{   "med-bind-wall-to-control",         0,      bind_wall_to_control_trigger },
{   "do-trigger-dialog",					 0,      do_trigger_dialog },

//--//// In macro.c
//--//{   "med-macro-menu",                   0,      MacroMenu },
//--//{   "med-macro-play-fast",              0,      MacroPlayFast },
//--//{   "med-macro-play-normal",            0,      MacroPlayNormal },
//--//{   "med-macro-record-all",             0,      MacroRecordAll },
//--//{   "med-macro-record-keys",            0,      MacroRecordKeys },
//--//{   "med-macro-save",                   0,      MacroSave },
//--//{   "med-macro-load",                   0,      MacroLoad },

// In editor.c
{   "med-update",                       0,      medlisp_update_screen },
{   "med-segment-add",                  0,      AttachSegment },
{   "med-segment-delete",               0,      medlisp_delete_segment },
{   "med-segment-scale",                3,      medlisp_scale_segment },
{   "med-segment-rotate",               3,      medlisp_rotate_segment },
{   "med-dos-shell",                    0,      DosShell },
//{   "med-lisp-call",                    0,      CallLisp },
{   "med-editor-exit",                  0,      ExitEditor },
{   "med-segment-exchange",             0,      ExchangeMarkandCurseg },
{   "med-segment-mark",                 0,      CopySegtoMarked },
{	 "med-about",								 0,      ShowAbout },
{	 "med-mark-start",						 0,		MarkStart },
{	 "med-mark-end",						 	 0,		MarkEnd },

//	In group.c
{	 "med-group-load",						 0, 		LoadGroup },
{	 "med-group-save",						 0, 		SaveGroup },
{   "med-move-group",                   0,      MoveGroup },
{   "med-copy-group",                   0,      CopyGroup },
{	 "med-rotate-group",						 0,		RotateGroup },
{   "med-segment-add-new",              0,      AttachSegmentNew },
{	 "med-mark-groupseg",					 0,		MarkGroupSegment },
{	 "med-next-group",						 0,		NextGroup },
{	 "med-prev-group",						 0,		PrevGroup },
{	 "med-delete-group",						 0,      DeleteGroup },
{	 "med-create-group",						 0,		CreateGroup },
{	 "med-ungroup-segment",					 0,		UngroupSegment },
{	 "med-group-segment",					 0,		GroupSegment },
{	 "med-degroup-group",					 0,		Degroup },
{	 "med-subtract-from-group",			 0,		SubtractFromGroup },

//  In autosave.c
{   "med-autosave-undo",                0,      UndoCommand },
{	 "med-autosave-toggle",					 0,		ToggleAutosave },

//	In texture.c
{	"med-tass-flip-x",						 0,	TexFlipX },
{	"med-tass-flip-y",						 0,	TexFlipY },
{	"med-tass-slide-up",						 0,	TexSlideUp },
{	"med-tass-slide-left",					 0,	TexSlideLeft },
{	"med-tass-set-default",					 0,	TexSetDefault },
{	"med-tass-slide-right",					 0,	TexSlideRight },
{	"med-tass-rotate-left",					 0,	TexRotateLeft },
{	"med-tass-slide-down",					 0,	TexSlideDown },
{	"med-tass-stretch-down",				 0,	TexStretchDown },
{	"med-tass-stretch-up",					 0,	TexStretchUp },
{	"med-tass-rotate-right",				 0,	TexRotateRight },
{	"med-tass-select-active-edge",		 0,	TexSelectActiveEdge },
{	"med-tass-rotate-90-degrees",			 0,	TexRotate90Degrees },
{  "med-tass-increase-tiling",          0,   TexIncreaseTiling },
{  "med-tass-decrease-tiling",          0,   TexDecreaseTiling },
{	"med-tass-slide-up-big",				 0,	TexSlideUpBig },
{	"med-tass-slide-left-big",				 0,	TexSlideLeftBig },
{	"med-tass-slide-right-big",			 0,	TexSlideRightBig },
{	"med-tass-rotate-left-big",			 0,	TexRotateLeftBig },
{	"med-tass-slide-down-big",				 0,	TexSlideDownBig },
{	"med-tass-rotate-right-big",			 0,	TexRotateRightBig },

//	In eobject.c
{	"med-obj-set-player",					 0,	SetPlayerPosition },
{	"med-obj-place-object",					 0,	ObjectPlaceObject },
{	"med-obj-place-object-tmap",			 0,	ObjectPlaceObjectTmap },
{	"med-obj-move-nearer",					 0,	ObjectMoveNearer },
{	"med-obj-move-further",					 0,	ObjectMoveFurther },
{	"med-obj-delete-object",				 0,	ObjectDelete },
{  "med-obj-move-forward",					 0,	ObjectMoveForward },
{  "med-obj-move-left",						 0,	ObjectMoveLeft },
{  "med-obj-set-default",					 0,	ObjectSetDefault },
{  "med-obj-move-right",					 0,	ObjectMoveRight },
{  "med-obj-move-back",						 0,	ObjectMoveBack },
{  "med-obj-move-down",						 0,	ObjectMoveDown },
{  "med-obj-move-up",						 0,	ObjectMoveUp },
{  "med-obj-select-next-in-segment",	 0,	ObjectSelectNextinSegment },
{  "med-obj-decrease-bank",				 0,	ObjectDecreaseBank },
{  "med-obj-increase-bank",				 0,	ObjectIncreaseBank },
{  "med-obj-decrease-pitch",				 0,	ObjectDecreasePitch },
{  "med-obj-increase-pitch",				 0,	ObjectIncreasePitch },
{  "med-obj-decrease-heading",	 		 0,	ObjectDecreaseHeading },
{  "med-obj-increase-heading", 			 0,	ObjectIncreaseHeading },
{  "med-obj-decrease-bank-big",			 0,	ObjectDecreaseBankBig },
{  "med-obj-increase-bank-big",			 0,	ObjectIncreaseBankBig },
{  "med-obj-decrease-pitch-big",			 0,	ObjectDecreasePitchBig },
{  "med-obj-increase-pitch-big",			 0,	ObjectIncreasePitchBig },
{  "med-obj-decrease-heading-big",	 	 0,	ObjectDecreaseHeadingBig },
{  "med-obj-increase-heading-big", 		 0,	ObjectIncreaseHeadingBig },
{  "med-obj-reset",							 0,	ObjectResetObject },
{  "med-obj-flip",							 0,	ObjectFlipObject },
{	"med-obj-make-coop",					 0,	ObjectMakeCoop },
//{	"med-obj-place-hostage", 				 0,	ObjectPlaceHostage },

// In objpage.c
{	"med-obj-select-next-type",			 0,	objpage_goto_next_object },


//	In elight.c
{	"med-light-select-next-vertex",		 0,	LightSelectNextVertex },
{	"med-light-select-next-edge",			 0,	LightSelectNextEdge },
{	"med-light-copy-intensity-side",		 0,	LightCopyIntensity },
{	"med-light-copy-intensity-segment",	 0,	LightCopyIntensitySegment },
{	"med-light-decrease-light-vertex",	 0,	LightDecreaseLightVertex },
{	"med-light-increase-light-vertex",	 0,	LightIncreaseLightVertex },
{	"med-light-decrease-light-side",		 0,	LightDecreaseLightSide },
{	"med-light-increase-light-side",		 0,	LightIncreaseLightSide },
{	"med-light-decrease-light-segment",	 0,	LightDecreaseLightSegment },
{	"med-light-increase-light-segment",	 0,	LightIncreaseLightSegment },
{	"med-light-set-maximum",		 		 0,	LightSetMaximum },
{	"med-light-set-default",		 		 0,	LightSetDefault },
{	"med-light-assign-default-all",		 0,	LightSetDefaultAll },
{	"med-light-ambient-lighting",			 0,	LightAmbientLighting },

// In seguvs.c																		
{	"med-seguvs-fix-bogus-uvs-on-side",	 0,	fix_bogus_uvs_on_side},
{	"med-seguvs-fix-bogus-uvs-all",	 	 0,	fix_bogus_uvs_all},
{	"med-seguvs-smooth-lighting-all",	 0,   set_average_light_on_all},
{	"med-seguvs-smooth-lighting-all-quick",	 0,   set_average_light_on_all_quick},
{	"med-seguvs-smooth-lighting",	 		 0,   set_average_light_on_curside},

// Miscellaneous, please neaten and catagorize me!

{   "med-increase-draw-depth",          0,        IncreaseDrawDepth },
{   "med-decrease-draw-depth",          0,        DecreaseDrawDepth },
{   "med-goto-game",                    0,        GotoGame },
{   "med-goto-game-screen",             0,        GotoGameScreen },
{   "med-drop-into-debugger",           0,        DropIntoDebugger },
// {   "med-sync-large-view",              0,        SyncLargeView },
{   "med-create-default-new-segment",   0,        CreateDefaultNewSegment },
{   "med-create-default-new-segment-and-attach",   0,        CreateDefaultNewSegmentandAttach },
{   "med-clear-selected-list",          0,        ClearSelectedList },
{   "med-clear-found-list",             0,        ClearFoundList },
{   "med-sort-selected-list",           0,        SortSelectedList },
{   "med-set-player-from-curseg",       0,        SetPlayerFromCursegAndRotate },
{   "med-set-player-from-curseg-minus-one", 0,        SetPlayerFromCursegMinusOne },
{   "med-find-concave-segs",            0,        FindConcaveSegs },
{   "med-select-next-found-seg",        0,        SelectNextFoundSeg },
{   "med-select-prev-found-seg",        0,        SelectPreviousFoundSeg },
{   "med-stop-slew",                    0,        slew_stop },
//{   "med-reset-orientation",            0,        do_reset_orient },
{   "med-game-zoom-out",                0,        GameZoomOut },
{   "med-game-zoom-in",                 0,        GameZoomIn },
{	 "med-keypad-goto-prev",				 0,        med_keypad_goto_prev },
{	 "med-keypad-goto-next",				 0,        med_keypad_goto_next },
{	 "med-keypad-goto",				 1,        med_keypad_goto },

// John's temporary function page stuff
// {   "med-set-function-page",            1,        medtmp_set_page },

// In fuelcen.c
{	"med-fuelcen-create",					0, 	fuelcen_create_from_curseg },
{	"med-repaircen-create",					0, 	repaircen_create_from_curseg },
{	"med-controlcen-create",					0,  controlcen_create_from_curseg },
{	"med-robotmaker-create",					0,  robotmaker_create_from_curseg },
{ 	"med-fuelcen-reset-all", 					0,  fuelcen_reset_all },
{	"med-fuelcen-delete",					0,		fuelcen_delete_from_curseg },

// In robot.c
{	"do-robot-dialog",							0, do_robot_dialog },
{	"do-object-dialog",							0, do_object_dialog },
{	"do-hostage-dialog",							0, do_hostage_dialog },

// In gamesavec
{	"rename-level",							0, get_level_name },

// The terminating marker
{   NULL, 0, NULL } };

void init_med_functions()
{
	func_init(med_functions, (sizeof(med_functions)/sizeof(FUNCTION))-1 );
}


