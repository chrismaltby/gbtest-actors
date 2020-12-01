#ifndef ACTOR_H
#define ACTOR_H

#include <gb/gb.h>

#define MAX_ACTORS 20
#define MAX_ACTORS_ACTIVE 10

typedef struct actor_t
{
  UINT16 x;
  UINT16 y;
  UINT8 sprite_no;
  UINT8 pinned;
  UINT8 animate;
  UINT8 frame;
  UINT8 frame_start;
  UINT8 frame_end;
  UINT8 anim_tick;
  UINT8 flip_x;
  UINT8 rerender;

  // Collisions
  UBYTE collision_group;
  UBYTE collision_enabled;

  // Linked list
  struct actor_t *next;
  struct actor_t *prev;
} actor_t;

extern actor_t actors[MAX_ACTORS];
extern actor_t *actors_active_head;
extern actor_t *actors_inactive_head;
extern UBYTE actors_active_len;

void update_actors() __banked;
void deactivate_actor(actor_t *actor) __banked;
void activate_actor(actor_t *actor) __banked;
void actor_set_flip_x(actor_t *actor, UBYTE flip) __banked;
void actor_set_frames(actor_t *actor, UBYTE frame_start, UBYTE frame_end) __banked;
void actor_set_anim(actor_t *actor, UBYTE animate) __banked;
actor_t *actor_at_tile(UBYTE tx, UBYTE ty, UBYTE inc_noclip) __banked;

#endif
