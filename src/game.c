/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#include <assert.h>
#include <time.h>

#include <game.h>
#include <misc.h>
#include <window.h>
#include <sprite.h>


struct game {
	struct map** maps;       // the game's map
	short max_levels;        // nb maps of the game
	short current_level;
	struct player* player;
	struct monster* monster;
	struct bomb *bomb;
};

struct game*
game_new(void) {
	sprite_load(); // load sprites into process memory

	struct game* game = malloc(sizeof(*game));
	game->maps = malloc(sizeof(struct game));
	game->maps[0] = map_get_static();
	game->max_levels = 1;
	game->current_level = 0;
	//struct bomb *bomb;
	game->player = player_init(1);
	game->monster = monster_init();

	// Set default location of the player
	player_set_position(game->player, 1, 0);
	monster_set_position(game->monster,8,11);
	//init_position_monster();

	return game;
}

void game_free(struct game* game) {
	assert(game);

	player_free(game->player);
	//monster_free(game->monster);
	for (int i = 0; i < game->max_levels; i++)
		map_free(game->maps[i]);
}

struct map* game_get_current_map(struct game* game) {
	assert(game);
	return game->maps[game->current_level];
}


struct player* game_get_player(struct game* game) {
	assert(game);
	return game->player;
}

void game_banner_display(struct game* game) {
	assert(game);

	struct map* map = game_get_current_map(game);

	int y = (map_get_height(map)) * SIZE_BLOC;
	for (int i = 0; i < map_get_width(map); i++)
		window_display_image(sprite_get_banner_line(), i * SIZE_BLOC, y);

	int white_bloc = ((map_get_width(map) * SIZE_BLOC) - 11 * SIZE_BLOC) / 4;
	int x = white_bloc;
	y = (map_get_height(map) * SIZE_BLOC) + LINE_HEIGHT;
	window_display_image(sprite_get_banner_life(), x, y);

	x = white_bloc + SIZE_BLOC;
	window_display_image(
			sprite_get_number(player_get_nb_pv(game_get_player(game))), x, y);

	x = 2 * white_bloc + 2 * SIZE_BLOC;
	window_display_image(sprite_get_banner_bomb(), x, y);

	x = 2 * white_bloc + 3 * SIZE_BLOC;
	window_display_image(
			sprite_get_number(player_get_nb_bomb(game_get_player(game))), x, y);

	x = 3 * white_bloc + 4 * SIZE_BLOC;
	window_display_image(sprite_get_banner_range(), x, y);

	x = 3 * white_bloc + 5 * SIZE_BLOC;
	window_display_image(sprite_get_number(player_get_bomb_range(game_get_player(game))), x, y);

	x = 4 * white_bloc + 6 * SIZE_BLOC;
	window_display_image(sprite_get_key(), x, y);

	x = 4 * white_bloc + 7* SIZE_BLOC;
		window_display_image(
				sprite_get_number(player_get_nb_key(game_get_player(game))), x, y);

	x = 5 * white_bloc + 8 * SIZE_BLOC;
		window_display_image(sprite_get_door_closed(), x, y);

	x = 5 * white_bloc + 9* SIZE_BLOC;
			window_display_image(
					sprite_get_number(game->current_level), x, y);
}

void game_display(struct game* game) {
	assert(game);

	window_clear();

	game_banner_display(game);
	map_display(game_get_current_map(game));
	player_display(game->player);
	window_refresh();
}

void pause(){
	SDL_Event event;
	int continu=1;
	while (continu){
		SDL_WaitEvent(&event);
		switch (event.type) {
		case SDL_KEYDOWN :
			switch (event.key.keysym.sym) {
				case SDLK_p :
					continu=0;
					break;
				default : break;
			}
			break;
		default : break;
		}
	}
}
static short input_keyboard(struct game* game) {
	SDL_Event event;
	struct player* player = game_get_player(game);
	struct map* map = game_get_current_map(game);

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			return 1;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				game_free(game);
				return 1;
			case SDLK_UP:
				player_set_current_way(player, NORTH);
				player_move(player, map);
				break;
			case SDLK_DOWN:
				player_set_current_way(player, SOUTH);
				player_move(player, map);
				break;
			case SDLK_RIGHT:
				player_set_current_way(player, EAST);
				player_move(player, map);
				break;
			case SDLK_LEFT:
				player_set_current_way(player, WEST);
				player_move(player, map);
				break;
			case SDLK_SPACE:
				if ((player_get_nb_bomb(player)>0) &&( map_get_cell_type(map , player_get_x(player) , player_get_y (player))==CELL_EMPTY)){
				player_dec_nb_bomb(player);
				game->bomb = bomb_init(game->bomb,player_get_x(player),player_get_y(player),SDL_GetTicks(),player);
				map_set_cell_type(map,player_get_x(player),player_get_y(player),CELL_BOMB4);
				}
				break;
			case SDLK_p :
				pause();
				break;
			default : break;
			}

			break;
		}
	}
	return 0;
}

int game_update(struct game* game) {
	struct bomb *tmp = game->bomb;
	struct map* map = game_get_current_map(game);
	if ((tmp!=NULL) && (SDL_GetTicks()-bomb_get_t0(tmp))>1000){
		bomb_set_t0(tmp,SDL_GetTicks());

		bomb_dec_state(tmp);
		switch (bomb_get_state(tmp)){
		case -1 :
			end_explosion(tmp,map);

			break;
		case 0 :
			bomb_explosion(tmp,map);
			break;
		case 1 :
			map_set_cell_type(map,bomb_get_x(tmp),bomb_get_y(tmp),CELL_BOMB1);
			break;
		case 2 :
			map_set_cell_type(map,bomb_get_x(tmp),bomb_get_y(tmp),CELL_BOMB2);
			break;
		case 3 :
			map_set_cell_type(map,bomb_get_x(tmp),bomb_get_y(tmp),CELL_BOMB3);
			break;
		default : break;
		}
		game->bomb = tmp;
		}


	if (input_keyboard(game))	//Player
		return 1; // exit game

	return 0;
}
