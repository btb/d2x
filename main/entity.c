
#include "inferno.h"
#include "hash.h"
#include "u_mem.h"


#define ENTITY_MAX_ENTITIES (MAX_ROBOT_TYPES + MAX_POWERUP_TYPES)

hashtable entity_hash;
entity *entity_list[ENTITY_MAX_ENTITIES];
int Num_entities;


void entity_add(ubyte object_type, int object_id, char object_name[16])
{
	entity *ent;
	char entity_name[32];
	char *p;

	strcpy(entity_name, Object_type_names[object_type]);
	p = strchr(entity_name, ' ');
	if (!p)
		p = strchr(entity_name, '\0');
	*p++ = '_';

	strcpy(p, object_name);
	strlwr(entity_name);

	MALLOC(ent, entity, 1);
	ent->name = d_strdup(entity_name);
	ent->object_type = object_type;
	ent->object_number = object_id;

	hashtable_insert(&entity_hash, ent->name, Num_entities);
	con_printf(CON_DEBUG, "entity_add: added %s\n", ent->name);

	entity_list[Num_entities++] = ent;
}


static void entity_add_object_type(ubyte object_type, char typenames[][16], int num)
{
	int i;

	for (i = 0; i < num; i++)
	{
		if (!typenames[i] || !typenames[i][0])
			continue;

		entity_add(object_type, i, typenames[i]);
	}
}


entity *entity_find(const char *entity_name)
{
	int i;

	i = hashtable_search( &entity_hash, entity_name );

	if ( i < 0 )
		return NULL;

	return entity_list[i];
}


void entity_cmd_report_entities(int argc, char **argv)
{
	int i;

	if (argc > 1)
		return;

	for (i = 0; i < Num_entities; i++)
		con_printf(CON_NORMAL, "    %s\n", entity_list[i]->name);
}


#define SPIT_SPEED 20


void entity_cmd_create(int argc, char **argv)
{
	entity     *ent;
	int        objnum;
	object     *obj;
	vms_vector new_velocity, new_pos;
	fix        objsize;
	ubyte      ctype, mtype, rtype;

	if (argc < 2)
		return;

	ent = entity_find(argv[1]);

	if (!ent)
		return;

	vm_vec_scale_add(&new_velocity, &ConsoleObject->mtype.phys_info.velocity, &ConsoleObject->orient.fvec, i2f(SPIT_SPEED));

#ifdef NETWORK
	if (Game_mode & GM_MULTI)
	{
		if (Net_create_loc >= MAX_NET_CREATE_OBJECTS)
		{
			con_printf(CON_NORMAL, "ent_create: Not enough slots\n" ));
			return (-1);
		}
	}
#endif

	switch (ent->object_type) {
		case OBJ_POWERUP:
			objsize = Powerup_info[ent->object_number].size;
			ctype = CT_POWERUP;
			mtype = MT_PHYSICS;
			rtype = RT_POWERUP;
			break;
		case OBJ_ROBOT:
			objsize = Polygon_models[Robot_info[ent->object_number].model_num].rad;
			ctype = CT_AI;
			mtype = MT_PHYSICS;
			rtype = RT_POLYOBJ;
			break;
		default:
			Int3();
			break;
	}

	// there's a piece of code which lets the player pick up a powerup if
	// the distance between him and the powerup is less than 2 time their
	// combined radii.  So we need to create powerups pretty far out from
	// the player.
	vm_vec_scale_add(&new_pos, &ConsoleObject->pos, &ConsoleObject->orient.fvec, ConsoleObject->size + objsize);

	objnum = obj_create( ent->object_type, ent->object_number, ConsoleObject->segnum, &new_pos, &vmd_identity_matrix, objsize, ctype, mtype, rtype);

	if (objnum < 0 ) {
		con_printf(CON_NORMAL, "ent_create: cannot create. Aborting.\n");
		Int3();
		return;
	}

	obj = &Objects[objnum];

	switch (ent->object_type) {
		case OBJ_POWERUP:
			obj->mtype.phys_info.velocity = new_velocity;
			obj->mtype.phys_info.drag = 512;
			obj->mtype.phys_info.mass = F1_0;

			obj->mtype.phys_info.flags = PF_BOUNCE;

			obj->rtype.vclip_info.vclip_num = Powerup_info[obj->id].vclip_num;
			obj->rtype.vclip_info.frametime = Vclip[obj->rtype.vclip_info.vclip_num].frame_time;
			obj->rtype.vclip_info.framenum = 0;

			obj->ctype.powerup_info.flags |= PF_SPAT_BY_PLAYER;
			break;
		case OBJ_ROBOT:
			obj->mtype.phys_info.velocity = new_velocity;
			obj->rtype.pobj_info.model_num = Robot_info[obj->id].model_num;
			obj->rtype.pobj_info.subobj_flags = 0;

			obj->mtype.phys_info.mass = Robot_info[obj->id].mass;
			obj->mtype.phys_info.drag = Robot_info[obj->id].drag;

			obj->mtype.phys_info.flags |= (PF_LEVELLING);

			obj->shields = Robot_info[obj->id].strength;
			break;
		default:
			Int3();
			break;
	}
}


static void entity_free(void)
{
	while (Num_entities--) {
		d_free(entity_list[Num_entities]->name);
		d_free(entity_list[Num_entities]);
	}

	hashtable_free(&entity_hash);
}


void entity_init(void)
{
	hashtable_init(&entity_hash, ENTITY_MAX_ENTITIES);

	entity_add_object_type(OBJ_ROBOT, Robot_names, N_robot_types);
	entity_add_object_type(OBJ_POWERUP, Powerup_names, N_powerup_types);

	cmd_addcommand("report_entities", entity_cmd_report_entities, "");
	cmd_addcommand("ent_create", entity_cmd_create, "");

	atexit(entity_free);
}
