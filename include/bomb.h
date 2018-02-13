/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#ifndef BOMB_H_
#define BOMB_H_

#include<player.h>
#include <map.h>

struct bomb;

struct bomb* bomb_init(struct bomb *bomb,int x,int y,int t0, struct player* player);

int bomb_get_x(struct bomb* bomb);

void bomb_set_x(struct bomb* bomb, int x);

int bomb_get_y(struct bomb* bomb);

void bomb_set_y(struct bomb* bomb, int y);

void bomb_dec_state(struct bomb* bomb);

int bomb_get_t0(struct bomb *bomb);

void bomb_set_t0(struct bomb *bomb, int t0);

int bomb_get_state(struct bomb *bomb);

void bomb_set_state(struct bomb *bomb, int state);

struct player* bomb_get_player(struct bomb* bomb);

void bomb_set_player(struct bomb* bomb, struct player* player);

void bomb_explosion(struct bomb* bomb, struct map* map);

void end_explosion(struct bomb* bomb, struct map* map);

#endif /* BOMB_H_ */
