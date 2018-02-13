#include <SDL/SDL_image.h>
#include <assert.h>

#include <bomb.h>
#include <misc.h>
struct bomb {
	int x,y;
	int t0;
	int state;
	struct player *player;
	struct bomb *next;


};

struct bomb* bomb_init(struct bomb *bomb,int x,int y,int t0, struct player* player) {
	bomb = malloc(sizeof(*bomb));
	if (!bomb)
		error("Memory error");
	bomb->x = x;
	bomb->y = y;
	bomb->t0 = t0;
	bomb->state = 4;
	bomb->player = player;
	bomb->next = NULL;
	return bomb;
}

int bomb_get_x(struct bomb* bomb) {
	assert(bomb != NULL);
	return bomb->x;
}

void bomb_set_x(struct bomb* bomb, int x){
	assert(bomb != NULL);
	bomb->x= x;
}

int bomb_get_y(struct bomb* bomb) {
	assert(bomb != NULL);
	return bomb->y;
}

void bomb_dec_state(struct bomb* bomb){
	bomb->state--;
}

int bomb_get_t0(struct bomb *bomb){
	assert(bomb != NULL);
	return bomb->t0;
}
void bomb_set_t0(struct bomb *bomb, int t0){
	assert(bomb != NULL);
	bomb->t0 = t0;
}

int bomb_get_state(struct bomb *bomb){
	assert(bomb != NULL);
	return bomb->state;
}
void bomb_set_state(struct bomb *bomb, int state){
	assert(bomb != NULL);
	bomb->state = state;
}

struct player* bomb_get_player(struct bomb* bomb) {
	assert(bomb != NULL);
	return bomb->player;
}

void bomb_set_player(struct bomb* bomb, struct player* player){
	assert(bomb != NULL);
	bomb->player= player;
}

static int explosion_move_aux( struct map* map, int x, int y) {

	if (!map_is_inside(map, x, y))
		return 0;

	switch (map_get_cell_type(map, x, y)) {
	case (CELL_EMPTY) :
		return 1;
		break;
	case CELL_BOX :
		return 2;
		break;
	case CELL_MONSTER :
		return 0;
		break;
	case CELL_BOMB :
		if (map_get_cell_sub_type(map,x,y)==CELL_BOMBFIRE)
			return -1;
		break;
	default : break;
	}
	return 0;
}

void bomb_explosion(struct bomb* bomb, struct map* map){
	int i=0, x=bomb->x , y=bomb->y,north=1,south=1,east=1,west=1;
	struct player* player=bomb->player;

	if ( (player_get_x(player)==x) && (player_get_y(player)==y) )
		player_dec_nb_pv(player);
	map_set_cell_type(map,x,y,CELL_BOMBFIRE);

	for (i=1;i<=player_get_bomb_range(player);i++){
		if (north){
		switch (explosion_move_aux(map, x,y+i)){
			case 1 :
				if ( (player_get_x(player)==x) && (player_get_y(player)==y+i) )
					player_dec_nb_pv(player);
				map_set_cell_type(map,x,y+i,CELL_BOMBFIRE);
				break;
			case 2:

				switch(map_get_cell_sub_type(map,x,y+i)){
				case CELL_BOX_RANGEINC :map_set_cell_type(map,x,y+i,CELL_BONUS_RANGEINC); break;
				case CELL_BOX_RANGEDEC :map_set_cell_type(map,x,y+i,CELL_BONUS_RANGEDEC); break;
				case CELL_BOX_BOMBINC  :map_set_cell_type(map,x,y+i,CELL_BONUS_BOMBINC); break;
				case CELL_BOX_BOMBDEC  :map_set_cell_type(map,x,y+i,CELL_BONUS_BOMBDEC); break;
				case CELL_BOX_LIFE     :map_set_cell_type(map,x,y+i,CELL_BONUS_LIFE); break;
				case CELL_BOX_MONSTER  :map_set_cell_type(map,x,y+i,CELL_MONSTER_N); break;
				default :map_set_cell_type(map,x,y+i,CELL_BOMBFIRE);break;
						}
				north--;
				break;

			default :north--;break;
			}
		}
		if (south){
				switch (explosion_move_aux(map, x,y-i)){
					case 1 :
						if ( (player_get_x(player)==x) && (player_get_y(player)==y-i) )
							player_dec_nb_pv(player);
						map_set_cell_type(map,x,y-i,CELL_BOMBFIRE);
						break;
					case 2:

						switch(map_get_cell_sub_type(map,x,y-i)){
						case CELL_BOX_RANGEINC :map_set_cell_type(map,x,y-i,CELL_BONUS_RANGEINC); break;
						case CELL_BOX_RANGEDEC :map_set_cell_type(map,x,y-i,CELL_BONUS_RANGEDEC); break;
						case CELL_BOX_BOMBINC  :map_set_cell_type(map,x,y-i,CELL_BONUS_BOMBINC); break;
						case CELL_BOX_BOMBDEC  :map_set_cell_type(map,x,y-i,CELL_BONUS_BOMBDEC); break;
						case CELL_BOX_LIFE     :map_set_cell_type(map,x,y-i,CELL_BONUS_LIFE); break;
						case CELL_BOX_MONSTER  :map_set_cell_type(map,x,y-i,CELL_MONSTER_N); break;
						default :map_set_cell_type(map,x,y-i,CELL_BOMBFIRE);break;
								}
						south--;
						break;

					default :south--;break;
					}
				}
		if (west){
				switch (explosion_move_aux(map, x-i,y)){
					case 1 :
						if ( (player_get_x(player)==x-i) && (player_get_y(player)==y) )
							player_dec_nb_pv(player);
						map_set_cell_type(map,x-i,y,CELL_BOMBFIRE);
						break;
					case 2:

						switch(map_get_cell_sub_type(map,x-i,y)){
						case CELL_BOX_RANGEINC :map_set_cell_type(map,x-i,y,CELL_BONUS_RANGEINC); break;
						case CELL_BOX_RANGEDEC :map_set_cell_type(map,x-i,y,CELL_BONUS_RANGEDEC); break;
						case CELL_BOX_BOMBINC  :map_set_cell_type(map,x-i,y,CELL_BONUS_BOMBINC); break;
						case CELL_BOX_BOMBDEC  :map_set_cell_type(map,x-i,y,CELL_BONUS_BOMBDEC); break;
						case CELL_BOX_LIFE     :map_set_cell_type(map,x-i,y,CELL_BONUS_LIFE); break;
						case CELL_BOX_MONSTER  :map_set_cell_type(map,x-i,y,CELL_MONSTER_N); break;
						default :map_set_cell_type(map,x-i,y,CELL_BOMBFIRE);break;
								}
						west--;
						break;

					default :west--;break;
					}
				}
		if (east){
				switch (explosion_move_aux(map, x+i,y)){
					case 1 :
						if ( (player_get_x(player)==x+i) && (player_get_y(player)==y) )
							player_dec_nb_pv(player);
						map_set_cell_type(map,x+i,y,CELL_BOMBFIRE);
						break;
					case 2:

						switch(map_get_cell_sub_type(map,x+i,y)){
						case CELL_BOX_RANGEINC :map_set_cell_type(map,x+i,y,CELL_BONUS_RANGEINC); break;
						case CELL_BOX_RANGEDEC :map_set_cell_type(map,x+i,y,CELL_BONUS_RANGEDEC); break;
						case CELL_BOX_BOMBINC  :map_set_cell_type(map,x+i,y,CELL_BONUS_BOMBINC); break;
						case CELL_BOX_BOMBDEC  :map_set_cell_type(map,x+i,y,CELL_BONUS_BOMBDEC); break;
						case CELL_BOX_LIFE     :map_set_cell_type(map,x+i,y,CELL_BONUS_LIFE); break;
						case CELL_BOX_MONSTER  :map_set_cell_type(map,x+i,y,CELL_MONSTER_N); break;
						default :map_set_cell_type(map,x+i,y,CELL_BOMBFIRE);break;
								}
						east--;
						break;

					default :east--;break;
					}
				}
	}
	player_inc_nb_bomb(player);
}

void end_explosion(struct bomb* bomb, struct map* map){
	int i=0, x=bomb->x , y=bomb->y;
	struct player* player=bomb->player;
	map_set_cell_type(map,x,y,CELL_EMPTY);
	for (i=1;i<=player_get_bomb_range(player);i++){
		if (explosion_move_aux(map, x,y+i)==-1){
			map_set_cell_type(map,x,y+i,CELL_EMPTY);
		}
		if (explosion_move_aux(map, x,y-i)==-1){
			map_set_cell_type(map,x,y-i,CELL_EMPTY);
		}
		if (explosion_move_aux(map, x+i,y)==-1){
			map_set_cell_type(map,x+i,y,CELL_EMPTY);
		}
		if (explosion_move_aux(map, x-i,y)==-1){
			map_set_cell_type(map,x-i,y,CELL_EMPTY);
		}
	}
}
