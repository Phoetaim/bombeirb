/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#include <SDL/SDL_image.h>
#include <assert.h>

#include <player.h>
#include <sprite.h>
#include <window.h>
#include <misc.h>
#include <constant.h>
#include <monster.h>


struct player {
	int x, y;
	enum direction current_direction;
	int nb_bombs;
	int nb_pv;
	int nb_key;
	int bomb_range;
};

struct player* player_init(int bomb_number) {
	struct player* player = malloc(sizeof(*player));
	if (!player)
		error("Memory error");

	player->current_direction = SOUTH;
	player->nb_bombs = bomb_number;
	player->nb_pv = 2;
	player->nb_key=0;
	player->bomb_range = 2;
	return player;
}


void player_set_position(struct player *player, int x, int y) {
	assert(player);
	player->x = x;
	player->y = y;
}


void player_free(struct player* player) {
	assert(player);
	free(player);
}

/////////////////  POSITION  /////////////////

int player_get_x(struct player* player) {
	assert(player != NULL);
	return player->x;
}

int player_get_y(struct player* player) {
	assert(player != NULL);
	return player->y;
}

void player_set_current_way(struct player* player, enum direction way) {
	assert(player);
	player->current_direction = way;
}

/////////////////  BOMB  /////////////////

int player_get_nb_bomb(struct player* player) {
	assert(player);
	return player->nb_bombs;
}

void player_inc_nb_bomb(struct player* player) {
	assert(player);
	player->nb_bombs += 1;
}

void player_dec_nb_bomb(struct player* player) {
	assert(player);
	player->nb_bombs -= 1;
}

/////////////////  LIFE  /////////////////

int player_get_nb_pv(struct player* player) {
	assert(player);
	return player->nb_pv;
}

void player_inc_nb_pv(struct player* player) {
	assert(player);
	player->nb_pv += 1;
}

void player_dec_nb_pv(struct player* player) {
	assert(player);
	player->nb_pv -= 1;
	if (player->nb_pv<0){
		sprite_free();
	}
}

/////////////////  KEY  /////////////////

int player_get_nb_key(struct player* player) {
	assert(player);
	return player->nb_key;
}

void player_inc_nb_key(struct player* player) {
	assert(player);
	player->nb_key += 1;
}

void player_dec_nb_key(struct player* player) {
	assert(player);
	player->nb_key -= 1;
}

/////////////////  BOMB RANGE  /////////////////

int player_get_bomb_range(struct player* player) {
	assert(player);
	return player->bomb_range;
}

void player_inc_bomb_range(struct player* player) {
	assert(player);
	player->bomb_range += 1;
}

void player_dec_bomb_range(struct player* player) {
	assert(player);
	if (player->bomb_range>1){
		player->bomb_range -= 1;
	}
}

/////////////////  BONUS  /////////////////

void player_move_to_bonus(struct player* player, enum compose_type type){
	switch(type){
		case CELL_BONUS_RANGEINC : player_inc_bomb_range(player); break;
		case CELL_BONUS_RANGEDEC : player_dec_bomb_range(player); break;
		case CELL_BONUS_BOMBINC  : player_inc_nb_bomb(player);    break;
		case CELL_BONUS_BOMBDEC  : player_dec_nb_bomb(player);    break;
		case CELL_BONUS_LIFE     : player_inc_nb_pv(player);      break;
		default : break;
	}
}

/////////////////  MOUVEMENT  /////////////////

static int player_move_aux(struct player* player, struct map* map, int x, int y) {

	if (!map_is_inside(map, x, y))
		return 0;

	switch (map_get_cell_type(map, x, y)) {
	case (CELL_SCENERY) :
		return 0;
		break;
	case CELL_BOX :
		return 2;
		break;
	case CELL_DOOR:
		switch (map_get_cell_sub_type(map, x, y)) { // sub-types are encoded with the 4 less significant bits
			case CELL_CLOSED:
				return 3;
				break;
			case CELL_OPENED:
				return 8;
				break;
			default : break;
			}
		break;

	case CELL_KEY :
		return 4;
		break;
	case CELL_BONUS :
		return 5;
		break;
	case CELL_MONSTER :
		return 6;
		break;
	case CELL_BOMB :
		if (map_get_cell_sub_type(map, x, y)==CELL_BOMBFIRE)
			return 6;
		else
			return 7;
		break;
	default : break;
	}

	// Player has moved
	return 1;
}

int player_move(struct player* player, struct map* map) {
	int x = player->x;
	int y = player->y;
	int move = 0;

	switch (player->current_direction) {
	case NORTH:
		switch(player_move_aux(player, map, x, y - 1)){
			case 1 :				//If the next cell is empty
				player->y--;
				move = 1;
				break;
			case 2 :				//If the next cell is a box
				if(player_move_aux(player,map,x,y-2)==1){ // Checking the next next cell, if empty, we can move the box
					map_set_cell_type(map, x,y-2,CELL_BOX);
					map_set_cell_type(map, x,y-1,CELL_EMPTY);
					player->y--;
					move = 1;
				}
				break;
			case 3 :				//If the next cell is a closed door
				player->y--;
				if (player->nb_key>0){
					map_set_cell_type(map,x,y-1,CELL_OPENED);
					player_dec_nb_key(player);
				}
				move = 0;
				break;
			case 4 :				//If the next cell is a key
				player->y--;
				map_set_cell_type(map, x, y-1, CELL_EMPTY);
				player_inc_nb_key(player);
				move = 1;
				break;
			case 5 :				//If the next cell is a bonus
				player_move_to_bonus(player, map_get_cell_sub_type(map,x,y-1));
				map_set_cell_type(map,x,y-1,CELL_EMPTY);
				player->y--;
				move = 1;
				break;
			case 6 :				//If the next cell is a monster
				player->y--;
				player_dec_nb_pv(player);
				move = 1;
				break;
			case 7 :				//If the next cell is a bomb
				move=0;
				break;
			case 8 :				//If the next cell is an opened door
				break;
			default : break;		//If the next cell is a scenery
		}
		break;

	case SOUTH:
		switch (player_move_aux(player, map, x , y + 1)){
			case 1 :				//Empty
				player->y++;
				move=1;
				break;
			case 2 :				//Box
				if(player_move_aux(player,map,x,y+2)==1){
					map_set_cell_type(map, x,y+2,CELL_BOX);
					map_set_cell_type(map, x,y+1,CELL_EMPTY);
					player->y++;
					move = 1;
				}
				break;
			case 3 :				//Closed Door
				player->y++;
				if (player->nb_key>0){
					map_set_cell_type(map,x,y+1,CELL_OPENED);
					player_dec_nb_key(player);
				}
				move = 0;
				break;
			case 4 :				//Key
				map_set_cell_type(map, x, y+1, CELL_EMPTY);
				player->y++;
				player_inc_nb_key(player);
				move = 1;
				break;
			case 5 :				//Bonus
				player_move_to_bonus(player, map_get_cell_sub_type(map,x,y+1));
				map_set_cell_type(map,x,y+1,CELL_EMPTY);
				player->y++;
				move = 1;
				break;
			case 6 :				//Monster
				player->y++;
				player_dec_nb_pv(player);
				move = 1;
				break;
			case 7 :				//Bomb
				move=0;
				break;
			case 8 :				//Opened Door
				//map_free(map);
				break;
			default : break;		//Scenery
			}
		break;

	case WEST:
		switch (player_move_aux(player, map, x - 1 , y )){
			case 1 :				//Empty
				player->x--;
				move=1;
				break;
			case 2 :				//Box
				if(player_move_aux(player,map,x-2,y)==1){
					map_set_cell_type(map, x-2,y,CELL_BOX);
					map_set_cell_type(map, x-1,y,CELL_EMPTY);
					player->x--;
					move = 1;
					}
				break;
			case 3 :				//Closed Door
				player->x--;
				if (player->nb_key>0){
					map_set_cell_type(map,x-1,y,CELL_EMPTY);
					map_set_cell_type(map,x-1,y,CELL_OPENED);
					player_dec_nb_key(player);
				}
				move = 0;
				break;
			case 4 :				//Key
				map_set_cell_type(map, x-1, y, CELL_EMPTY);
				player->x--;
				player_inc_nb_key(player);
				move = 1;
				break;
			case 5 :				//Bonus
				player_move_to_bonus(player, map_get_cell_sub_type(map,x-1,y));
				map_set_cell_type(map,x-1,y,CELL_EMPTY);
				player->x--;
				move = 1;
				break;
			case 6 :				//Monster
				player->x--;
				player_dec_nb_pv(player);
				move = 1;
				break;
			case 7 :				//Bomb
				move=0;
				break;
			case 8 :				//Opened Door
				//map_free(map);
				break;
			default : break;		//Scenery
		}
		break;

	case EAST:
		switch (player_move_aux(player, map, x + 1 , y )){
			case 1 :				//Empty
				player->x++;
				move=1;
				break;
			case 2 :				//Box
				if(player_move_aux(player,map,x+2,y)==1){
					map_set_cell_type(map, x+2,y,CELL_BOX);
					map_set_cell_type(map, x+1,y,CELL_EMPTY);
					player->x++;
					move = 1;
					}
				break;
			case 3 :				//Closed Door
				player->x++;
				if (player->nb_key>0){
					map_set_cell_type(map,x+1,y,CELL_OPENED);
					player_dec_nb_key(player);
				}
				move = 0;
				break;
			case 4 :				//Key
				map_set_cell_type(map, x+1, y, CELL_EMPTY);
				player->x++;
				player_inc_nb_key(player);
				move = 1;
				break;
			case 5 :				//Bonus
				player_move_to_bonus(player, map_get_cell_sub_type(map,x+1,y));
				map_set_cell_type(map,x+1,y,CELL_EMPTY);
				player->x++;
				move = 1;
				break;
			case 6 :				//Monster
				player->x++;
				player_dec_nb_pv(player);
				move = 1;
				break;
			case 7 :				//Bomb
				move=0;
				break;
			case 8 :				//Opened Door
				//map_free(map);
				break;
			default : break;		//Scenery
		}
		break;
	}
	/*if (move)
		map_set_cell_type(map,x,y,CELL_EMPTY);*/
	return move;
}


void player_display(struct player* player) {
	assert(player);
	window_display_image(sprite_get_player(player->current_direction),
			player->x * SIZE_BLOC, player->y * SIZE_BLOC);
}

