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


struct player {
	int x, y;
	enum direction current_direction;
	int nb_bombs;
	int nb_pv;
	int nb_key;
};

struct player* player_init(int bomb_number) {
	struct player* player = malloc(sizeof(*player));
	if (!player)
		error("Memory error");

	player->current_direction = SOUTH;
	player->nb_bombs = bomb_number;
	player->nb_pv = 2;
	player->nb_key=0;

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
	if (player->nb_pv==0){
		sprite_free();
	}
}

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
static int player_move_aux(struct player* player, struct map* map, int x, int y) {

	if (!map_is_inside(map, x, y))
		return 0;

	switch (map_get_cell_type(map, x, y)) {
	case CELL_SCENERY :
		return 0;
		break;
	case CELL_BOX :
		return 2;
		break;
	case CELL_CLOSED:
		return 3;
		break;
	case CELL_OPENED:
		return 8;
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
			case 1 :
				player->y--;
				move = 1;
				break;
			case 2 :
				if(player_move_aux(player,map,x,y-2)==1){
					map_set_cell_type(map, x,y-2,CELL_BOX);
					map_set_cell_type(map, x,y-1,CELL_EMPTY);
					player->y--;
					move = 1;
				}
				break;
			case 3 :
				player->y--;
				if (player->nb_key>0){
					map_set_cell_type(map,x,y-1,CELL_OPENED);
					player_dec_nb_key(player);
				}
				move = 0;
				break;
			case 4 :
				player->y--;
				map_set_cell_type(map, x, y-1, CELL_EMPTY);
				player_inc_nb_key(player);
				move = 1;
				break;
			case 5 :
				player->y--;
				move = 1;
				break;
			case 6 :
				player->y--;
				player_dec_nb_pv(player);
				move = 1;
				break;
			case 7 :
				move=0;
				break;
			case 8 : break;
			default : break;
		}
		break;

	case SOUTH:
		switch (player_move_aux(player, map, x , y + 1)){
			case 1 :
				player->y++;
				move=1;
				break;
			case 2 :
				if(player_move_aux(player,map,x,y+2)==1){
					map_set_cell_type(map, x,y+2,CELL_BOX);
					map_set_cell_type(map, x,y+1,CELL_EMPTY);
					player->y++;
					move = 1;
				}
				break;
			case 3 :
				player->y++;
				if (player->nb_key>0){
					map_set_cell_type(map,x,y+1,CELL_OPENED);
					player_dec_nb_key(player);
				}
				move = 0;
				break;
			case 4 :
				map_set_cell_type(map, x, y+1, CELL_EMPTY);
				player->y++;
				player_inc_nb_key(player);
				move = 1;
				break;
			case 5 :
				player->y++;
				move = 1;
				break;
			case 6 :
				player->y++;
				player_dec_nb_pv(player);
				move = 1;
				break;
			case 7 :
				move=0;
				break;
			case 8 : break;
			default : break;
			}
		break;

	case WEST:
		switch (player_move_aux(player, map, x - 1 , y )){
			case 1 :
				player->x--;
				move=1;
				break;
			case 2 :
				if(player_move_aux(player,map,x-2,y)==1){
					map_set_cell_type(map, x-2,y,CELL_BOX);
					map_set_cell_type(map, x-1,y,CELL_EMPTY);
					player->x--;
					move = 1;
					}
				break;
			case 3 :
				player->x--;
				if (player->nb_key>0){
					map_set_cell_type(map,x-1,y,CELL_EMPTY);
					map_set_cell_type(map,x-1,y,CELL_OPENED);
					player_dec_nb_key(player);
				}
				move = 0;
				break;
			case 4 :
				map_set_cell_type(map, x-1, y, CELL_EMPTY);
				player->x--;
				player_inc_nb_key(player);
				move = 1;
				break;
			case 5 :
				player->x--;
				move = 1;
				break;
			case 6 :
				player->x--;
				player_dec_nb_pv(player);
				move = 1;
				break;
			case 7 :
				move=0;
				break;
			case 8 : break;
			default : break;
		}
		break;

	case EAST:
		switch (player_move_aux(player, map, x + 1 , y )){
			case 1 :
				player->x++;
				move=1;
				break;
			case 2 :
				if(player_move_aux(player,map,x+2,y)==1){
					map_set_cell_type(map, x+2,y,CELL_BOX);
					map_set_cell_type(map, x+1,y,CELL_EMPTY);
					player->x++;
					move = 1;
					}
				break;
			case 3 :
				player->x++;
				if (player->nb_key>0){
					map_set_cell_type(map,x+1,y,CELL_OPENED);
					player_dec_nb_key(player);
				}
				move = 0;
				break;
			case 4 :
				map_set_cell_type(map, x+1, y, CELL_EMPTY);
				player->x++;
				player_inc_nb_key(player);

				move = 1;
				break;
			case 5 :
				player->x++;
				move = 1;
				break;
			case 6 :
				player->x++;
				player_dec_nb_pv(player);
				move = 1;
				break;
			case 7 :
				move=0;
				break;
			case 8 : break;
			default : break;
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

