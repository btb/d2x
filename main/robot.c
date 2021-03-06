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

/*
 *
 * Code for handling robots
 *
 */


#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#include <stdio.h>

#include "dxxerror.h"
#include "inferno.h"
#include "mono.h"


int	N_robot_types = 0;
int	N_robot_joints = 0;

//	Robot stuff
robot_info Robot_info[MAX_ROBOT_TYPES];

#ifndef EDITOR
/* names "mech" through "escort" found in bitmaps.tbl */
char Robot_names[MAX_ROBOT_TYPES][ROBOT_NAME_LENGTH] = {
	"mech",         // medium hulk
	"green",        // green claw, medium lifter
	"spider",       // red spider, processing robot
	"josh",         // class 1 drone
	"violet",       // spike, class 2 drone
	"clkvulc",      // cloaked class 1 driller
	"clkmech",      // cloaked hulk
	"brain",        // class 2 supervisor robot
	"onearm",       // secondary lifter
	"plasguy",      // class 1 heavy driller
	"toaster",      // class 3 gopher robot
	"bird",         // class 1 platform robot
	"mislbird",     // class 2 platform robot
	"splitpod",     // polyhedron, split pod
	"smspider",     // baby spider
	"miniboss",     // mini boss, fusion hulk
	"suprmech",     // super mech, heavy hulk
	"boss1",        // shareware boss, super hulk
	"cloakgrn",     // cloaked lifter
	"vulcnguy",     // vulcan guy, class 1 driller
	"rifleman",     // toad, small hulk
	"fourclaw",     // advanced lifter
	"quadlaser",    // defense prototype
	"boss2",        // bigboss, mega hulk
	"babyplas",     // bper bot
	"newguy",       // smelter
	"icespidr",     // ice spindle defense robot
	"gaussguy",     // bulk destroyer
	"newguy2",      // trn racer
	"newguy3",      // fox attack bot
	"newguy4",      // sidearm
	"newguy5",      // zeta aquilae boss
	"newboss1",     // ?? boss2
	"escort",       // guide bot
	"plasguy2",     // ?? plasguy w/missiles
	"kamikaze",     // ?? kamikaze escort
	"itsc",         // internal tactical security control robot
	"itd",          // internal tactical droid
	"pest",         // portable equalizing standard transport
	"pig",          // preliminary integration groundbot
	"diamondclaw",  // diamond claw, second generation
	"redhornet",    // red hornet
	"thief",        // thief, bandit
	"seeker",       // seeker
	"ebandit",      // e-bandit
	"45",           // brimspark boss
	"46",           // quartzon boss
	"boarshead",    // boarshead
	"spider2",      // spider
	"omegadefense", // omega defense spawn
	"sidearmmodula", // sidearm modula
	"louguard",     // lou guard
	"52",           // baloris prime boss
	"53",           // brimspark mini boss
	"clkdiamondclaw", // cloaked diamond claw
	"clksmelter",   // cloaked smelter
	"purpleomega",  // ?? purple omegadefense
	"smelter",      // ?? another newguy
	"omegadefense2", // ?? another omegadefense
	"bper",         // ?? another babyplas
	"spider2b",     // ?? another spider2
	"spawn",        // spawn
	"62",           // limefrost spiral boss
	"spawn2",       // ?? another spawn
	"64",           // tycho brahe boss
	"65",           // ?? a reactor?
	/* vertigo robots, possibly not constant */
	"compactlifter", // compact lifter
	"fervid99",     // fervid 99
	"fiddler",      // fiddler
	"heavydriller2", // class 2 heavy driller
	"smelter2",     // smelter ii
	"max",          // maximum amplified xenophobe
	"sniperng",     // sniper ng
	"logikill",     // codename: logikill
	"canary",       // canary
	"75",           // vertigo boss 1
	"76",           // vertigo boss 2
	"spike",        // s.p.i.k.e.
};
#endif

//Big array of joint positions.  All robots index into this array

#define deg(a) ((int) (a) * 32768 / 180)

//test data for one robot
jointpos Robot_joints[MAX_ROBOT_JOINTS] = {

//gun 0
	{2,{deg(-30),0,0}},         //rest (2 joints)
	{3,{deg(-40),0,0}},

	{2,{deg(0),0,0}},           //alert
	{3,{deg(0),0,0}},

	{2,{deg(0),0,0}},           //fire
	{3,{deg(0),0,0}},

	{2,{deg(50),0,0}},          //recoil
	{3,{deg(-50),0,0}},

	{2,{deg(10),0,deg(70)}},    //flinch
	{3,{deg(0),deg(20),0}},

//gun 1
	{4,{deg(-30),0,0}},         //rest (2 joints)
	{5,{deg(-40),0,0}},

	{4,{deg(0),0,0}},           //alert
	{5,{deg(0),0,0}},

	{4,{deg(0),0,0}},           //fire
	{5,{deg(0),0,0}},

	{4,{deg(50),0,0}},          //recoil
	{5,{deg(-50),0,0}},

	{4,{deg(20),0,deg(-50)}},   //flinch
	{5,{deg(0),0,deg(20)}},

//rest of body (the head)

	{1,{deg(70),0,0}},          //rest (1 joint, head)

	{1,{deg(0),0,0}},           //alert

	{1,{deg(0),0,0}},           //fire

	{1,{deg(0),0,0}},           //recoil

	{1,{deg(-20),deg(15),0}},   //flinch

};

//given an object and a gun number, return position in 3-space of gun
//fills in gun_point
void calc_gun_point(vms_vector *gun_point,object *obj,int gun_num)
{
	polymodel *pm;
	robot_info *r;
	vms_vector pnt;
	vms_matrix m;
	int mn;				//submodel number

	Assert(obj->render_type==RT_POLYOBJ || obj->render_type==RT_MORPH);
	Assert(obj->id < N_robot_types);

	r = &Robot_info[obj->id];
	pm =&Polygon_models[r->model_num];

	if (gun_num >= r->n_guns)
	{
		mprintf((1, "Bashing gun num %d to 0.\n", gun_num));
		//Int3();
		gun_num = 0;
	}

//	Assert(gun_num < r->n_guns);

	pnt = r->gun_points[gun_num];
	mn = r->gun_submodels[gun_num];

	//instance up the tree for this gun
	while (mn != 0) {
		vms_vector tpnt;

		vm_angles_2_matrix(&m,&obj->rtype.pobj_info.anim_angles[mn]);
		vm_transpose_matrix(&m);
		vm_vec_rotate(&tpnt,&pnt,&m);

		vm_vec_add(&pnt,&tpnt,&pm->submodel_offsets[mn]);

		mn = pm->submodel_parents[mn];
	}

	//now instance for the entire object

	vm_copy_transpose_matrix(&m,&obj->orient);
	vm_vec_rotate(gun_point,&pnt,&m);
	vm_vec_add2(gun_point,&obj->pos);

}

//fills in ptr to list of joints, and returns the number of joints in list
//takes the robot type (object id), gun number, and desired state
int robot_get_anim_state(jointpos **jp_list_ptr,int robot_type,int gun_num,int state)
{

	Assert(gun_num <= Robot_info[robot_type].n_guns);

	*jp_list_ptr = &Robot_joints[Robot_info[robot_type].anim_states[gun_num][state].offset];

	return Robot_info[robot_type].anim_states[gun_num][state].n_joints;

}


//for test, set a robot to a specific state
void set_robot_state(object *obj,int state)
{
	int g,j,jo;
	robot_info *ri;
	jointlist *jl;

	Assert(obj->type == OBJ_ROBOT);

	ri = &Robot_info[obj->id];

	for (g=0;g<ri->n_guns+1;g++) {

		jl = &ri->anim_states[g][state];

		jo = jl->offset;

		for (j=0;j<jl->n_joints;j++,jo++) {
			int jn;

			jn = Robot_joints[jo].jointnum;

			obj->rtype.pobj_info.anim_angles[jn] = Robot_joints[jo].angles;

		}
	}
}

#include "mono.h"

//--unused-- int cur_state=0;

//--unused-- test_anim_states()
//--unused-- {
//--unused-- 	set_robot_state(&Objects[1],cur_state);
//--unused--
//--unused-- 	mprintf(0,"Robot in state %d\n",cur_state);
//--unused--
//--unused-- 	cur_state = (cur_state+1)%N_ANIM_STATES;
//--unused--
//--unused-- }

//set the animation angles for this robot.  Gun fields of robot info must
//be filled in.
void robot_set_angles(robot_info *r,polymodel *pm,vms_angvec angs[N_ANIM_STATES][MAX_SUBMODELS])
{
	int m,g,state;
	int gun_nums[MAX_SUBMODELS];			//which gun each submodel is part of

	for (m=0;m<pm->n_models;m++)
		gun_nums[m] = r->n_guns;		//assume part of body...

	gun_nums[0] = -1;		//body never animates, at least for now

	for (g=0;g<r->n_guns;g++) {
		m = r->gun_submodels[g];

		while (m != 0) {
			gun_nums[m] = g;				//...unless we find it in a gun
			m = pm->submodel_parents[m];
		}
	}

	for (g=0;g<r->n_guns+1;g++) {

		//mprintf(0,"Gun %d:\n",g);

		for (state=0;state<N_ANIM_STATES;state++) {

			//mprintf(0," State %d:\n",state);

			r->anim_states[g][state].n_joints = 0;
			r->anim_states[g][state].offset = N_robot_joints;

			for (m=0;m<pm->n_models;m++) {
				if (gun_nums[m] == g) {
					//mprintf(0,"  Joint %d: %x %x %x\n",m,angs[state][m].pitch,angs[state][m].bank,angs[state][m].head);
					Robot_joints[N_robot_joints].jointnum = m;
					Robot_joints[N_robot_joints].angles = angs[state][m];
					r->anim_states[g][state].n_joints++;
					N_robot_joints++;
					Assert(N_robot_joints < MAX_ROBOT_JOINTS);
				}
			}
		}
	}

}

#ifndef FAST_FILE_IO
/*
 * reads n jointlist structs from a CFILE
 */
static int jointlist_read_n(jointlist *jl, int n, CFILE *fp)
{
	int i;

	for (i = 0; i < n; i++) {
		jl[i].n_joints = cfile_read_short(fp);
		jl[i].offset = cfile_read_short(fp);
	}
	return i;
}

/*
 * reads n robot_info structs from a CFILE
 */
int robot_info_read_n(robot_info *ri, int n, CFILE *fp)
{
	int i, j;

	for (i = 0; i < n; i++) {
		ri[i].model_num = cfile_read_int(fp);
		for (j = 0; j < MAX_GUNS; j++)
			cfile_read_vector(&(ri[i].gun_points[j]), fp);
		cfread(ri[i].gun_submodels, MAX_GUNS, 1, fp);

		ri[i].exp1_vclip_num = cfile_read_short(fp);
		ri[i].exp1_sound_num = cfile_read_short(fp);

		ri[i].exp2_vclip_num = cfile_read_short(fp);
		ri[i].exp2_sound_num = cfile_read_short(fp);

		ri[i].weapon_type = cfile_read_byte(fp);
		ri[i].weapon_type2 = cfile_read_byte(fp);
		ri[i].n_guns = cfile_read_byte(fp);
		ri[i].contains_id = cfile_read_byte(fp);

		ri[i].contains_count = cfile_read_byte(fp);
		ri[i].contains_prob = cfile_read_byte(fp);
		ri[i].contains_type = cfile_read_byte(fp);
		ri[i].kamikaze = cfile_read_byte(fp);

		ri[i].score_value = cfile_read_short(fp);
		ri[i].badass = cfile_read_byte(fp);
		ri[i].energy_drain = cfile_read_byte(fp);

		ri[i].lighting = cfile_read_fix(fp);
		ri[i].strength = cfile_read_fix(fp);

		ri[i].mass = cfile_read_fix(fp);
		ri[i].drag = cfile_read_fix(fp);

		for (j = 0; j < NDL; j++)
			ri[i].field_of_view[j] = cfile_read_fix(fp);
		for (j = 0; j < NDL; j++)
			ri[i].firing_wait[j] = cfile_read_fix(fp);
		for (j = 0; j < NDL; j++)
			ri[i].firing_wait2[j] = cfile_read_fix(fp);
		for (j = 0; j < NDL; j++)
			ri[i].turn_time[j] = cfile_read_fix(fp);
		for (j = 0; j < NDL; j++)
			ri[i].max_speed[j] = cfile_read_fix(fp);
		for (j = 0; j < NDL; j++)
			ri[i].circle_distance[i] = cfile_read_fix(fp);
		cfread(ri[i].rapidfire_count, NDL, 1, fp);

		cfread(ri[i].evade_speed, NDL, 1, fp);

		ri[i].cloak_type = cfile_read_byte(fp);
		ri[i].attack_type = cfile_read_byte(fp);

		ri[i].see_sound = cfile_read_byte(fp);
		ri[i].attack_sound = cfile_read_byte(fp);
		ri[i].claw_sound = cfile_read_byte(fp);
		ri[i].taunt_sound = cfile_read_byte(fp);

		ri[i].boss_flag = cfile_read_byte(fp);
		ri[i].companion = cfile_read_byte(fp);
		ri[i].smart_blobs = cfile_read_byte(fp);
		ri[i].energy_blobs = cfile_read_byte(fp);

		ri[i].thief = cfile_read_byte(fp);
		ri[i].pursuit = cfile_read_byte(fp);
		ri[i].lightcast = cfile_read_byte(fp);
		ri[i].death_roll = cfile_read_byte(fp);

		ri[i].flags = cfile_read_byte(fp);
		cfread(ri[i].pad, 3, 1, fp);

		ri[i].deathroll_sound = cfile_read_byte(fp);
		ri[i].glow = cfile_read_byte(fp);
		ri[i].behavior = cfile_read_byte(fp);
		ri[i].aim = cfile_read_byte(fp);

		for (j = 0; j < MAX_GUNS + 1; j++)
			jointlist_read_n(ri[i].anim_states[j], N_ANIM_STATES, fp);

		ri[i].always_0xabcd = cfile_read_int(fp);
	}
	return i;
}

/*
 * reads n jointpos structs from a CFILE
 */
int jointpos_read_n(jointpos *jp, int n, CFILE *fp)
{
	int i;

	for (i = 0; i < n; i++) {
		jp[i].jointnum = cfile_read_short(fp);
		cfile_read_angvec(&jp[i].angles, fp);
	}
	return i;
}
#endif
