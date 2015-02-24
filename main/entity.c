
#include "inferno.h"
#include "hash.h"
#include "u_mem.h"


#define ENTITY_MAX_ENTITIES (MAX_ROBOT_TYPES + MAX_POWERUP_TYPES)

hashtable entity_hash;
entity *entity_list[ENTITY_MAX_ENTITIES];
int Num_entities;


static void entity_add_object_type(ubyte object_type, char typenames[][16], int num)
{
	int i;
	entity *ent;
	char entity_name[32];
	char *p;

	strcpy(entity_name, Object_type_names[object_type]);
	p = strchr(entity_name, ' ');
	if (!p)
		p = strchr(entity_name, '\0');
	*p++ = '_';
	for (i = 0; i < num; i++)
	{
		if (!typenames[i] || !typenames[i][0])
			continue;

		strcpy(p, typenames[i]);
		strlwr(entity_name);

		MALLOC(ent, entity, 1);
		ent->name = d_strdup(entity_name);
		ent->object_type = object_type;
		ent->object_number = i;

		hashtable_insert(&entity_hash, entity_name, Num_entities);
		con_printf(CON_DEBUG, "entity_add: added %s\n", ent->name);

		entity_list[Num_entities++] = ent;
	}
}


void entity_cmd_report_entities(int argc, char **argv)
{
	int i;

	if (argc > 1)
		return;

	for (i = 0; i < Num_entities; i++)
		con_printf(CON_NORMAL, "    %s\n", entity_list[i]->name);
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

	entity_add_object_type(OBJ_ROBOT, Robot_names, MAX_ROBOT_TYPES);
	entity_add_object_type(OBJ_POWERUP, Powerup_names, MAX_POWERUP_TYPES);

	cmd_addcommand("report_entities", entity_cmd_report_entities, "");

	atexit(entity_free);
}
