#pragma bank 1

#include "Actor.h"
#include "GameTime.h"
#include "Camera.h"
#include "LinkedList.h"
#include "Math.h"
#include "Sprite.h"
#include <gb/gb.h>
#ifdef STRICT
#include <gb/bgb_emu.h>
#include <gb/crash_handler.h>
#endif

actor_t actors[MAX_ACTORS];
actor_t *actors_active_head;
actor_t *actors_inactive_head;
UBYTE actors_active_len;

actor_t *actor;
INT8 screen_x, screen_y;
actor_t *invalid;

void update_actors() __banked
{
  actor = actors_active_head;

  while (actor)
  {
    // Handle pinned position
    if (actor->pinned)
    {
      screen_x = actor->x;
      screen_y = actor->y;
      goto move_actor;
    }

    // Translate position to screen coordinates
    screen_x = actor->x - camera_x + 8;
    screen_y = actor->y - camera_y;

    // Check if offscreen
    if ((UINT8)(screen_x + 8) > 176 || (UINT8)(screen_y) > 160)
    {
      // Deactivate if offscreen
      actor_t *next = actor->next;
      deactivate_actor(actor);
      actor = next;
      continue;
    }
    else
    {
      if (WX_REG != 7 && WX_REG < (UINT8)screen_x + 8 && WY_REG < (UINT8)(screen_y)-8)
      {
        // Hide if under window (don't deactivate)
        move_sprite(actor->sprite_no, 0, 0);
        move_sprite(actor->sprite_no + 1, 0, 0);
        actor = actor->next;
        continue;
      }
    move_actor:
      // Position onscreen
      move_sprite(actor->sprite_no, screen_x, screen_y);
      move_sprite(actor->sprite_no + 1, screen_x + 8, screen_y);
    }

    // Check if should animate
    if ((game_time & 0x3) == 0 && actor->animate)
    {
      // Check reached animation tick frame
      if ((game_time & actor->anim_tick) == 0)
      {
        actor->frame += 4;

        // Check reached end of animation
        if (actor->frame == actor->frame_end)
        {
          actor->frame = actor->frame_start;
        }

        goto rerender;
      }
    }

    // Check for forced rerender
    if (actor->rerender)
    {
      rerender:
        actor->rerender = FALSE;

        if (actor->flip_x)
        {
          set_sprite_tile(actor->sprite_no + 1, actor->frame);
          set_sprite_tile(actor->sprite_no, actor->frame + 2);
        }
        else
        {
          set_sprite_tile(actor->sprite_no, actor->frame);
          set_sprite_tile(actor->sprite_no + 1, actor->frame + 2);
        }      
    }

    actor = actor->next;
  }
}

void deactivate_actor(actor_t *actor) __banked
{
#ifdef STRICT
  // Check exists in inactive list
  UBYTE found = 0;
  actor_t *current = actors_active_head;
  DL_CONTAINS(current, actor, found);
  if (!found)
  {
    BGB_MESSAGE("Deactivated non active actor\n");
    __HandleCrash();
    return;
  }
#endif
  actors_active_len--;
  DL_REMOVE_ITEM(actors_active_head, actor);
  move_sprite(actor->sprite_no, 0, 0);
  move_sprite(actor->sprite_no + 1, 0, 0);
  release_sprite(actor->sprite_no);
  DL_PUSH_HEAD(actors_inactive_head, actor);
}

void activate_actor(actor_t *actor) __banked
{
#ifdef STRICT
  // Check exists in inactive list
  UBYTE found = 0;
  actor_t *current = actors_inactive_head;
  DL_CONTAINS(current, actor, found);
  if (!found)
  {
    BGB_MESSAGE("Activated non inactive actor\n");
    __HandleCrash();
    return;
  }
#endif
  if (actors_active_len == MAX_ACTORS_ACTIVE)
  {
    return;
  }
  actors_active_len++;
  actor->sprite_no = get_free_sprite();
  DL_REMOVE_ITEM(actors_inactive_head, actor);
  DL_PUSH_HEAD(actors_active_head, actor);
}

void actor_set_flip_x(actor_t *actor, UBYTE flip) __banked
{
  if (flip)
  {
    set_sprite_prop(actor->sprite_no, S_FLIPX);
    set_sprite_prop(actor->sprite_no + 1, S_FLIPX);
  }
  else
  {
    set_sprite_prop(actor->sprite_no, 0);
    set_sprite_prop(actor->sprite_no + 1, 0);
  }
  actor->flip_x = flip;
  actor->rerender = TRUE;
}

void actor_set_frames(actor_t *actor, UBYTE frame_start, UBYTE frame_end) __banked
{
  if (actor->frame_start != frame_start || actor->frame_end != frame_end)
  {
    actor->frame = frame_start;
    actor->frame_start = frame_start;
    actor->frame_end = frame_end;
    actor->rerender = TRUE;
  }
}

void actor_set_anim(actor_t *actor, UBYTE animate) __banked
{
  actor->animate = animate;
}

actor_t *actor_at_tile(UBYTE tx, UBYTE ty, UBYTE inc_noclip) __banked
{
  actor_t *actor = actors_active_head;

  while (actor)
  {
    UBYTE a_tx, a_ty;

    if ((!inc_noclip && !actor->collision_enabled))
    {
      continue;
    }

    a_tx = DIV_8(actor->x);
    a_ty = DIV_8(actor->y);

    if ((ty == a_ty || ty == a_ty + 1) && (tx == a_tx || tx == a_tx + 1 || tx == a_tx - 1))
    {
      return actor;
    }

    actor = actor->next;
  }

  return NULL;
}
