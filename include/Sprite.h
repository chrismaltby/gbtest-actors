#ifndef SPRITE_H
#define SPRITE_H

#include <gb/gb.h>

#define MAX_SPRITES 20

extern UINT8 sprites_pool[MAX_SPRITES];
extern UINT8 sprites_pool_next;

void init_sprite_pool() __banked;
UINT8 get_free_sprite() __banked;
void release_sprite(UINT8 sprite_no) __banked;

#endif
