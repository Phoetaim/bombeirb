/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#include <SDL/SDL_image.h>
#include <assert.h>

#include <monster.h>
#include <sprite.h>
#include <window.h>
#include <misc.h>
#include <constant.h>


struct monster {
	int x, y;
	enum direction current_direction;
};

struct monster* monster_init() {
	struct monster* monster = malloc(sizeof(*monster));
	if (!monster)
		error("Memory error");

	monster->current_direction = SOUTH;

	return monster;
}


void monster_set_position(struct monster *monster, int x, int y) {
	assert(monster);
	monster->x = x;
	monster->y = y;
}


void monster_free(struct monster* monster) {
	assert(monster);
	free(monster);
}

int monster_get_x(struct monster* monster) {
	assert(monster != NULL);
	return monster->x;
}

int monster_get_y(struct monster* monster) {
	assert(monster != NULL);
	return monster->y;
}

void monster_set_current_way(struct monster* monster, enum direction way) {
	assert(monster);
	monster->current_direction = way;
}

static int monster_move_aux(struct monster* monster, struct map* map, int x, int y) {

	if (!map_is_inside(map, x, y))
		return 0;

	if (map_get_cell_type(map, x, y)==CELL_EMPTY){
		return 1;
	}

	// monster has moved
	return 0;
}

int monster_move(struct monster* monster, struct map* map) {

	int x = monster->x;
	int y = monster->y;
	int move = 0;

	switch (monster->current_direction) {
	case NORTH:
		if (monster_move_aux(monster, map, x, y - 1)){
			map_set_cell_type(map,x,y-1,CELL_MONSTER_N);
			monster->y--;
			move = 1;
		}
		break;

	case SOUTH:
		if (monster_move_aux(monster, map, x , y + 1)){
			map_set_cell_type(map,x,y+1,CELL_MONSTER_S);
			monster->y++;
			move=1;
			}
		break;

	case WEST:
		if (monster_move_aux(monster, map, x - 1 , y )){
			map_set_cell_type(map,x-1,y,CELL_MONSTER_W);
			monster->x--;
			move=1;
		}
		break;

	case EAST:
		if (monster_move_aux(monster, map, x + 1 , y )){
			map_set_cell_type(map,x+1,y,CELL_MONSTER_E);
			monster->x++;
			move=1;
		}
		break;
	}
	if (move)
		map_set_cell_type(map,x,y,CELL_EMPTY);
	return move;
}

