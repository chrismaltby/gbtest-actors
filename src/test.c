#include <gb/gb.h>
#include "gfx.h"
#include "GameTime.h"
#include "Actor.h"
#include "Camera.h"
#include "LinkedList.h"
#include "Sprite.h"
#include <stdio.h>
#include <gb/bgb_emu.h>
#include <rand.h>

UBYTE prev_j = 0;

void main()
{
  UBYTE i;

  // Init LCD
  LCDC_REG = 0x67;

  SPRITES_8x16;

  // Set palettes
  BGP_REG = OBP0_REG = 0xE4U;
  OBP1_REG = 0xD2U;

  WX_REG = 96;
  WY_REG = 96;

  SWITCH_ROM_MBC1(2);
  set_bkg_data(0, 186, tile_data);
  set_bkg_tiles(0, 0, 20, 18, bg_data);
  set_sprite_data(0, 24, sprite_1_data);

  init_sprite_pool();

  actors_active_len = 0;
  actors_active_head = 0;
  actors_inactive_head = 0;

  for (i = 0; i != MAX_ACTORS; i++)
  {
    actors[i].x = 32 + ((i % 4) * 16);
    actors[i].y = 32 + ((i / 4) * 16);
    actors[i].sprite_no = 0;
    actors[i].frame = 0;
    actors[i].frame_start = 0;
    actors[i].frame_end = 8;
    actors[i].anim_tick = 0x07;
    actors[i].rerender = TRUE;
    actors[i].flip_x = FALSE;
    actors[i].pinned = FALSE;
    actors[i].animate = TRUE;
    actors[i].collision_enabled = TRUE;

    DL_PUSH_HEAD(actors_inactive_head, &actors[i]);
  }

  activate_actor(&actors[0]);
  activate_actor(actors_inactive_head);
  activate_actor(actors_inactive_head);

  actors[0].x = 72;
  actors[0].y = 48;

  DISPLAY_ON;

  while (1)
  {
    UINT8 j = joypad();

    if (j & J_LEFT)
    {
      actor_set_frames(&actors[0], 16, 24);
      actor_set_flip_x(&actors[0], TRUE);
      actor_set_anim(&actors[0], TRUE);
      &actors[0].x--;
    }
    else if (j & J_RIGHT)
    {
      actor_set_frames(&actors[0], 16, 24);
      actor_set_flip_x(&actors[0], FALSE);
      actor_set_anim(&actors[0], TRUE);
      &actors[0].x++;
    }
    else if (j & J_UP)
    {
      actor_set_frames(&actors[0], 8, 16);
      actor_set_flip_x(&actors[0], FALSE);
      actor_set_anim(&actors[0], TRUE);
      &actors[0].y--;
    }
    else if (j & J_DOWN)
    {
      actor_set_frames(&actors[0], 0, 8);
      actor_set_flip_x(&actors[0], FALSE);
      actor_set_anim(&actors[0], TRUE);
      &actors[0].y++;
    }
    else
    {
      actor_set_anim(&actors[0], FALSE);
    }

    if (j & J_A && !(prev_j & J_A))
    {
      if (actors_inactive_head)
      {
        activate_actor(actors_inactive_head);
      }
    }

    if (j & J_B && !(prev_j & J_B))
    {
      actor_t *found = actor_at_tile(0, 0, FALSE);
      if (found)
      {
        found->y = 64;
      }
    }

    if (j & J_START && !(prev_j & J_START))
    {
      if (actors_inactive_head)
      {
        activate_actor(actors_inactive_head);
      }
    }

    if (j & J_SELECT && !(prev_j & J_SELECT))
    {
      if (actors_active_head)
      {
        deactivate_actor(actors_active_head);
      }
    }

    // BGB_PROFILE_BEGIN()
    update_actors();
    // BGB_PROFILE_END(UPDATE_ACTORS)

    game_time++;

    prev_j = j;

    wait_vbl_done();
  }
}
