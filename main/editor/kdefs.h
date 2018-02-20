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
COPYRIGHT 1993-1999 PARALLAX SOFTWARE CORPORATION.  ALL RIGHTS RESERVED.
*/

// In khelp.c
int DoHelp(void);

// In kcurve.c
int InitCurve(void);
int GenerateCurve(void);
int DecreaseR4(void);
int IncreaseR4(void);
int DecreaseR1(void);
int IncreaseR1(void);
int DeleteCurve(void);
int SetCurve(void);

// In kmine.c
int SaveMine(void);
int LoadMine(void);
int MineMenu(void);
int CreateNewMine(void);
int LoadOldMine(void);

int SaveSituation(void);
int LoadSituation(void);

// In kgame.c
int SetPlayerPosition(void);
int SaveGameData(void);
int LoadGameData(void);
int LoadMineOnly(void);
void ResetFilename(void);

// In group.c
int LoadGroup(void);
int SaveGroup(void);
int PrevGroup(void);
int NextGroup(void);
int CreateGroup(void);
int SubtractFromGroup(void);
int DeleteGroup(void);
int MarkGroupSegment(void);
int MoveGroup(void);
int CopyGroup(void);
int AttachSegmentNew(void);
int UngroupSegment(void);
int GroupSegment(void);
int Degroup(void);
int RotateGroup(void);

// In segment.c
int ToggleBottom(void);
void make_curside_bottom_side(void);
int select_segment_by_number(void);
int select_segment_with_powerup(void);

// In editor.c
int UndoCommand(void);

// In kview.c
int ZoomOut(void);
int ZoomIn(void);
int MoveAway(void);
int MoveCloser(void);
int ToggleChaseMode(void);

// In kbuild.c
int CreateBridge(void);
int FormJoint(void);
int CreateAdjacentJoint(void);
int CreateAdjacentJointsSegment(void);
int CreateAdjacentJointsAll(void);
int CreateSloppyAdjacentJoint(void);
int CreateSloppyAdjacentJointsGroup(void);

// In ksegmove.c
int DecreaseHeading(void);
int IncreaseHeading(void);
int DecreasePitch(void);
int IncreasePitch(void);
int DecreaseBank(void);
int IncreaseBank(void);

// In ksegsel.c
int SelectCurrentSegForward(void);
int SelectCurrentSegBackward(void);
int SelectNextSide(void);
int SelectPrevSide(void);
int CopySegToMarked(void);
int SelectBottom(void);
int SelectFront(void);
int SelectTop(void);
int SelectBack(void);
int SelectLeft(void);
int SelectRight(void);

// In ksegsize.c
int IncreaseSegLength(void);
int DecreaseSegLength(void);
int DecreaseSegWidth(void);
int IncreaseSegWidth(void);
int IncreaseSegHeight(void);
int DecreaseSegHeight(void);
int ToggleSegSizeMode(void);
int PerturbCurside(void);
int PerturbCursideBig(void);

int IncreaseSegLengthBig(void);
int DecreaseSegLengthBig(void);
int DecreaseSegWidthBig(void);
int IncreaseSegWidthBig(void);
int IncreaseSegHeightBig(void);
int DecreaseSegHeightBig(void);

int IncreaseSegLengthDefault(void);
int DecreaseSegLengthDefault(void);
int IncreaseSegWidthDefault(void);
int DecreaseSegWidthDefault(void);
int IncreaseSegHeightDefault(void);
int DecreaseSegHeightDefault(void);

//	In ktmap.c
int AssignTexture(void);
int AssignTexture2(void);
int ClearTexture2(void);
int PropagateTextures(void);
int PropagateTexturesMove(void);
int PropagateTexturesMoveUVs(void);
int PropagateTexturesUVs(void);
int PropagateTexturesSelected(void);

//--//// In macro.c
//--//int MacroMenu();
//--//int MacroPlayFast();
//--//int MacroPlayNormal();
//--//int MacroRecordAll();
//--//int MacroRecordKeys();
//--//int MacroSave();
//--//int MacroLoad();

// In editor.c
int medlisp_update_screen(void);
int medlisp_delete_segment(void);
int medlisp_scale_segment(void);
int medlisp_rotate_segment(void);
int medlisp_add_segment(void);
int AttachSegment(void);
int DeleteSegment(void);
int DosShell(void);
int CallLisp(void);
int ExitEditor(void);
int ShowAbout(void);
int ExchangeMarkandCurseg(void);
int CopySegtoMarked(void);
int med_keypad_goto_prev(void);
int med_keypad_goto_next(void);
int med_keypad_goto(void);
int med_increase_tilings(void);
int med_decrease_tilings(void);
int ToggleAutosave(void);
int MarkStart(void);
int MarkEnd(void);

//	Texture.c
int TexFlipX(void);
int TexFlipY(void);
int TexSlideUp(void);
int TexSlideLeft(void);
int TexSetDefault(void);
int TexSetDefaultSelected(void);
int TexSlideRight(void);
int TexRotateLeft(void);
int TexSlideDown(void);
int TexRotateRight(void);
int TexSelectActiveEdge(void);
int TexRotate90Degrees(void);
int TexIncreaseTiling(void);
int TexDecreaseTiling(void);
int TexSlideUpBig(void);
int TexSlideLeftBig(void);
int TexSlideRightBig(void);
int TexRotateLeftBig(void);
int TexSlideDownBig(void);
int TexRotateRightBig(void);
int TexStretchDown(void);
int TexStretchUp(void);
int TexChangeAll(void);
int TexChangeAll2(void);

//	object.c
int ObjectPlaceObject(void);
int ObjectMakeCoop(void);
int ObjectPlaceObjectTmap(void);
int ObjectDelete(void);
int ObjectMoveForward(void);
int ObjectMoveLeft(void);
int ObjectSetDefault(void);
int ObjectMoveRight(void);
int ObjectMoveBack(void);
int ObjectMoveDown(void);
int ObjectMoveUp(void);
int ObjectMoveNearer(void);
int ObjectMoveFurther(void);
int ObjectSelectNextinSegment(void);
int ObjectSelectNextType(void);
int ObjectDecreaseBank(void);
int ObjectIncreaseBank(void);
int ObjectDecreasePitch(void);
int ObjectIncreasePitch(void);
int ObjectDecreaseHeading(void);
int ObjectIncreaseHeading(void);
int ObjectResetObject(void);


//	elight.c
int LightSelectNextVertex(void);
int LightSelectNextEdge(void);
int LightCopyIntensity(void);
int LightCopyIntensitySegment(void);
int LightDecreaseLightVertex(void);
int LightIncreaseLightVertex(void);
int LightDecreaseLightSide(void);
int LightIncreaseLightSide(void);
int LightDecreaseLightSegment(void);
int LightIncreaseLightSegment(void);
int LightSetMaximum(void);
int LightSetDefault(void);
int LightSetDefaultAll(void);
int LightAmbientLighting(void);

// seguvs.c
int fix_bogus_uvs_on_side(void);
int fix_bogus_uvs_all(void);
int set_average_light_on_curside(void);
int set_average_light_on_all(void);
int set_average_light_on_all_quick(void);

// Miscellaneous, please put in correct file if you have time
int IncreaseDrawDepth(void);
int DecreaseDrawDepth(void);
int GotoMainMenu(void);
int GotoGameScreen(void);
int DropIntoDebugger(void);
int CreateDefaultNewSegment(void);
int CreateDefaultNewSegmentandAttach(void);
int ClearSelectedList(void);
int ClearFoundList(void);
int SortSelectedList(void);
int SetPlayerFromCurseg(void);
int SetPlayerFromCursegAndRotate(void);
int SetPlayerFromCursegMinusOne(void);
int FindConcaveSegs(void);
int SelectNextFoundSeg(void);
int SelectPreviousFoundSeg(void);
int do_reset_orient(void);
int GameZoomOut(void);
int GameZoomIn(void);

// John's temp page stuff
int medtmp_set_page(void);

// In objpage.c
int objpage_goto_next_object(void);

// In medsel.c
extern int SortSelectedList(void);
extern int SelectNextFoundSeg(void);
extern int SelectPreviousFoundSeg(void);

// In wall.c
extern int wall_add_blastable(void);
extern int wall_add_door(void);
extern int wall_add_closed_wall(void);
extern int wall_add_external_wall(void);
extern int wall_lock_door(void);
extern int wall_unlock_door(void);
extern int wall_automate_door(void);
extern int wall_deautomate_door(void);
extern int wall_add_illusion(void);
extern int wall_remove(void);
extern int wall_restore_all(void);
extern int wall_assign_door_1(void);
extern int wall_assign_door_2(void);
extern int wall_assign_door_3(void);
extern int wall_assign_door_4(void);
extern int wall_assign_door_5(void);
extern int wall_assign_door_6(void);
extern int wall_assign_door_7(void);
extern int wall_assign_door_8(void);
extern int do_wall_dialog(void);
extern int do_trigger_dialog(void);
extern int check_walls(void);
extern int delete_all_walls(void);
extern int delete_all_controlcen_triggers(void);

// In centers.c
extern int do_centers_dialog(void);

// In switch.c
//extern int trigger_add_damage(void);
//extern int trigger_add_blank(void);
//extern int trigger_add_exit(void);
//extern int trigger_add_repair(void);
//extern int trigger_control(void);
//extern int trigger_remove(void);
//extern int trigger_add_if_control_center_dead(void);
extern int bind_wall_to_control_trigger(void);

// In med.c
extern int fuelcen_create_from_curseg(void);
extern int repaircen_create_from_curseg(void);
extern int controlcen_create_from_curseg(void);
extern int robotmaker_create_from_curseg(void);
extern int fuelcen_reset_all(void);
extern int RestoreGameState(void);
extern int fuelcen_delete_from_curseg(void);
extern int goal_blue_create_from_curseg(void);
extern int goal_red_create_from_curseg(void);

// In editor\robot.c
extern int do_robot_dialog(void);
extern int do_object_dialog(void);

// In editor\hostage.c
extern int do_hostage_dialog(void);

