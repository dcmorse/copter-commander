/*
 * Copter Commander - A 2d network helicopter game
 * Copyright (C) 20001  David Morse
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "coco.h"
#include "rasters.h"

/* _Raster arrays__________________________________________________ */

coco_mini_point 
coco_tank_hardpoints[COCO_TANK_RASTER_COUNT] = {
#if (COCO_APPEARANCE == COCO_APPEARANCE_RESCUE_RAIDERS)
  COCO_MAKE_MINI_POINT( 19, 16 ),
  COCO_MAKE_MINI_POINT( 54, 16 ),
#elif (COCO_APPEARANCE == COCO_APPEARANCE_DOODLE)
  COCO_MAKE_MINI_POINT( 32, 28 ),
  COCO_MAKE_MINI_POINT( 40, 28 ),
#else
#error COCO_APPEARANCE is not a value known to gamesho.c
#endif
};

/* aa-gun and at-gun */
coco_mini_point 
coco_gun_hardpoints[COCO_GUN_RASTER_COUNT] = {
  COCO_MAKE_MINI_POINT( 12, 11 ), COCO_MAKE_MINI_POINT( 12, 11 ),
};


coco_mini_point 
coco_cobra_rotor_points[COCO_COPTER_RASTER_COUNT] = {
  COCO_MAKE_MINI_POINT( 35, 28 ),
  COCO_MAKE_MINI_POINT( 65, 28 ),
  COCO_MAKE_MINI_POINT( 35, 28 ),
  COCO_MAKE_MINI_POINT( 65, 28 ),
};

coco_mini_point 
coco_cobra_weapon_hardpoints[COCO_COPTER_RASTER_COUNT] = {
  COCO_MAKE_MINI_POINT( 30, 7 ),
  COCO_MAKE_MINI_POINT( 70, 7 ),
  COCO_MAKE_MINI_POINT( 30, 7 ),
  COCO_MAKE_MINI_POINT( 70, 7 ),
};

coco_mini_point 
coco_cobra_center_of_mass[COCO_COPTER_RASTER_COUNT] = {
  COCO_MAKE_MINI_POINT( 35, 12 ),
  COCO_MAKE_MINI_POINT( 65, 12 ),
  COCO_MAKE_MINI_POINT( 35, 12 ),
  COCO_MAKE_MINI_POINT( 65, 12 ),
};

struct coco_raster *coco_cobra_rasters[COCO_COPTER_RASTER_COUNT] = {
  &coco_cobra_green_w_raster,
  &coco_cobra_green_e_raster,
  &coco_cobra_grey_w_raster,
  &coco_cobra_grey_e_raster,
};

coco_mini_point 
coco_bubble_copter_rotor_points[COCO_COPTER_RASTER_COUNT] = {
  COCO_MAKE_MINI_POINT( 26, 28 ),
  COCO_MAKE_MINI_POINT( 57, 28 ),
  COCO_MAKE_MINI_POINT( 26, 28 ),
  COCO_MAKE_MINI_POINT( 57, 28 ),
};

coco_mini_point 
coco_bubble_copter_weapon_hardpoints[COCO_COPTER_RASTER_COUNT] = {
  COCO_MAKE_MINI_POINT( 28, 7 ),
  COCO_MAKE_MINI_POINT( 55, 7 ),
  COCO_MAKE_MINI_POINT( 28, 7 ),
  COCO_MAKE_MINI_POINT( 55, 7 ),
};

coco_mini_point 
coco_bubble_copter_center_of_mass[COCO_COPTER_RASTER_COUNT] = {
  COCO_MAKE_MINI_POINT( 26, 15 ),
  COCO_MAKE_MINI_POINT( 57, 15 ),
  COCO_MAKE_MINI_POINT( 26, 15 ),
  COCO_MAKE_MINI_POINT( 57, 15 ),
};

struct coco_raster *coco_bubble_copter_rasters[COCO_COPTER_RASTER_COUNT] = {
  &coco_bubble_green_w_raster,
  &coco_bubble_green_e_raster,
  &coco_bubble_grey_w_raster,
  &coco_bubble_grey_e_raster,
};

/*
struct coco_raster *coco_fireball_shrapnel_rasters[COCO_FIREBALL_SHRAPNEL_RASTER_COUNT] = {
  &coco_fireball_24a_raster,
};
*/

struct coco_raster *coco_small_metal_shrapnel_rasters[COCO_SMALL_METAL_RASTER_COUNT] = {
  &coco_shrapnel_16a_raster,
  &coco_shrapnel_16b_raster,
};

struct coco_raster *coco_med_metal_shrapnel_rasters[COCO_MED_METAL_RASTER_COUNT] = {
  &coco_shrapnel_24a_raster,
  &coco_shrapnel_24b_raster,
  &coco_shrapnel_24c_raster,
};

struct coco_raster *coco_big_metal_shrapnel_rasters[COCO_BIG_METAL_RASTER_COUNT] = {
  &coco_shrapnel_32a_raster,
  &coco_shrapnel_32b_raster,
  &coco_shrapnel_32c_raster,
  &coco_shrapnel_32d_raster,
};

struct coco_raster *coco_smoke_rasters[COCO_SMOKE_RASTER_COUNT] = {
  /* small to big order required for intro movie, and by
     coco_copter_smoke_maybe */
  &coco_smoke_small_raster,
  &coco_smoke_med_raster,
  &coco_smoke_big_raster,
};

struct coco_raster *coco_cloud_rasters[COCO_CLOUD_RASTER_COUNT] = {
  &coco_cloud_raster
};


struct coco_raster *coco_balloon_rasters[COCO_BALLOON_RASTER_COUNT] = {
  /* indicies carefully chosen to be nationalities. */
  &coco_balloon_center_raster,
  &coco_balloon_w_raster,
  &coco_balloon_e_raster,
};

struct coco_raster *coco_traffic_sign_rasters[COCO_TRAFFIC_SIGN_RASTER_COUNT] = {
  &coco_wrong_way_raster,

};


struct coco_raster *coco_hut_rasters[COCO_HUT_RASTER_COUNT] = {
  /* indicies carefully chosen to be nationalities. */
  &coco_hut_neutral_raster,
  &coco_hut_green_raster,
  &coco_hut_grey_raster,
};

struct coco_raster *coco_bunker_rasters[COCO_BUNKER_RASTER_COUNT] = {
  /* indicies carefully chosen to be nationalities - COCO_GOOD_GUYS. */
  &coco_bunker_green_raster,
  &coco_bunker_grey_raster,
};

struct coco_raster *coco_pillbox_rasters[COCO_PILLBOX_RASTER_COUNT] = {
  /* indicies carefully chosen to be nationalities - COCO_GOOD_GUYS. */
  &coco_pillbox_raster,
};


struct coco_raster *coco_landing_pad_rasters[COCO_LANDING_PAD_RASTER_COUNT] = {
  &coco_landing_pad_raster,
};

struct coco_raster *coco_headquarters_rasters[COCO_HEADQUARTERS_RASTER_COUNT] = {
  &coco_headquarters_green_raster,
  &coco_headquarters_grey_raster,
};

struct coco_raster *coco_gun_rasters[COCO_GUN_RASTER_COUNT] = {
  &coco_gun_green_raster,
  &coco_gun_grey_raster, 
};

struct coco_raster *coco_gun_rubble_rasters[COCO_GUN_RUBBLE_RASTER_COUNT] = {
  &coco_gun_rubble_raster,
};

/* See also  */
struct coco_raster *coco_explosion_rasters[COCO_EXPLOSION_RASTER_COUNT] = {
  &coco_explosion_airburst_1_raster,
  &coco_explosion_airburst_2_raster,
  &coco_explosion_airburst_1_raster,
  &coco_explosion_bomb_1_raster,
  &coco_explosion_bomb_2_raster,
  &coco_explosion_bomb_3_raster,
  &coco_explosion_bomb_4_raster,
  &coco_explosion_bomb_5_raster,
  &coco_explosion_napalm_0_raster,
  &coco_explosion_napalm_1_raster,
  &coco_explosion_napalm_2_raster,
  &coco_explosion_napalm_3_raster,
};


struct coco_raster *coco_demo_van_rasters[COCO_DEMO_VAN_RASTER_COUNT] = {
  &coco_van_green_raster,
  &coco_mystery_machine_raster,
  &coco_van_grey_raster,
  &coco_bas_van_raster,
};

struct coco_raster *coco_tank_rasters[COCO_TANK_RASTER_COUNT] = {
  &coco_tank_green_raster,
  &coco_tank_grey_raster,
};

struct coco_raster *coco_bomb_rasters[COCO_BOMB_RASTER_COUNT] = {
  &coco_bomb_west_raster,
  &coco_bomb_sw_raster,
  &coco_bomb_south_raster,
  &coco_bomb_se_raster,
  &coco_bomb_east_raster,
};

#ifdef COCO_MISSILES_AS_RASTERS

struct coco_raster *coco_missile_rasters[COCO_MISSILE_RASTER_COUNT] = {
  &coco_missile_e_raster,
  &coco_missile_en_raster,
  &coco_missile_ne_raster,
  &coco_missile_n_raster,
  &coco_missile_nw_raster,
  &coco_missile_wn_raster,
  &coco_missile_w_raster,
  &coco_missile_ws_raster,
  &coco_missile_sw_raster,
  &coco_missile_s_raster,
  &coco_missile_se_raster,
  &coco_missile_es_raster,
};

#endif /* COCO_MISSILES_AS_RASTERS */

/* Ordering is dentermined by ,INFANTRY_WALK_COUNT,INFANTRY_WALK_WEST_BASE,
   INFANTRY_WALK_EAST_BASE,INFANTRY_FREE_FALL_COUNT etc. */
struct coco_raster *coco_infantry_rasters[COCO_INFANTRY_RASTER_COUNT] = {
  &coco_infantry_w1_raster,
  &coco_infantry_w2_raster,
  &coco_infantry_w3_raster,
  &coco_infantry_w4_raster,
  &coco_infantry_w5_raster,
  &coco_infantry_e1_raster,
  &coco_infantry_e2_raster,
  &coco_infantry_e3_raster,
  &coco_infantry_e4_raster,
  &coco_infantry_e5_raster,
  &coco_infantry_free_fall1_raster,
  &coco_infantry_free_fall2_raster,
  &coco_infantry_free_fall3_raster,
  &coco_infantry_free_fall4_raster,
  &coco_infantry_chute_green_raster,
  &coco_infantry_chute_grey_raster,
};

struct coco_raster *coco_engineer_rasters[COCO_INFANTRY_RASTER_COUNT] = {
  &coco_eng_w1_raster,
  &coco_eng_w2_raster,
  &coco_eng_w3_raster,
  &coco_eng_w4_raster,
  &coco_eng_w5_raster,
  &coco_eng_e1_raster,
  &coco_eng_e2_raster,
  &coco_eng_e3_raster,
  &coco_eng_e4_raster,
  &coco_eng_e5_raster,
  &coco_eng_free_fall1_raster,
  &coco_eng_free_fall2_raster,
  &coco_eng_free_fall3_raster,
  &coco_eng_free_fall4_raster,
  &coco_eng_chute_green_raster,
  &coco_eng_chute_grey_raster,
};


struct coco_raster *coco_firetruck_rasters[COCO_FIRETRUCK_RASTER_COUNT] = {
  &coco_firetruck_green_raster,
  &coco_firetruck_green_empty_raster,
  &coco_firetruck_grey_raster,
  &coco_firetruck_grey_empty_raster,
};

/* _Frame rate_________________________________________________________ */

/* rate at which we'll try to run the screen, and thus the game: */
/* 60 fps */
/* struct timeval coco_ideal_frame_rate = { 0,  16666 }; */
/* 30 fps */
/* struct timeval coco_ideal_frame_rate = { 0,  33333 }; */
/* 20 fps */
/* struct timeval coco_ideal_frame_rate = { 0,  50000 }; */
/* 15 fps */
/* struct timeval coco_ideal_frame_rate = { 0,  66666 }; */
/* 14 fps */
struct timeval coco_ideal_frame_rate = { 0,  71429 };
/* 12 fps */
/* struct timeval coco_ideal_frame_rate = { 0,  83333 }; */
/* 10 fps */
/* struct timeval coco_ideal_frame_rate = { 0, 100000 }; */
/* 5 fps */
/* struct timeval coco_ideal_frame_rate = { 0, 200000 }; */

/* SEE ALSO "IDEAL_FRAMES_PER_SECOND" IN coco.h, WHICH MUST BE SYNCED */

/* _Player array_______________________________________________________ */

/* This variable is present on both the server and client side, and is
   kept synced through "GAME" messages.  There is also a server only
   struct coco_stream coco_streams[COCO_MAX_PLAYERS];
   which records the streams to talk directly to the players.  */
struct coco_player coco_players[COCO_MAX_PLAYERS];



/* returns the x and y of the weapon hardpoints of a unit for its current
   raster index.  In board coordinates. */
void
coco_sobject_weapon_hardpoint_coords( coco_sobject *sobject, 
                                      coco_gnum *x_return,
                                      coco_gnum *y_return )

{
  coco_mini_point *hps = COCO_SOBJECT_WEAPON_HARDPOINTS( sobject );
  coco_gnum rx, ry;
  if (hps)
    {
      unsigned char ri = COCO_SOBJECT_RASTER_INDEX( sobject );
      coco_mini_point hp = hps[ri];
      rx = COCO_MINI_POINT_X( hp );
      ry = COCO_MINI_POINT_Y( hp );
    }
  else
    {
      rx = COCO_SOBJECT_W( sobject )>>1;
      ry = COCO_SOBJECT_H( sobject )>>1;
    }
  /* For a while I was going to rotate this if COCO_SOBJECT_ROTATE_P, but
     my math was buggy, and its such a small effect anyway.  */
  *x_return = rx + COCO_SOBJECT_X( sobject );
  *y_return = ry + COCO_SOBJECT_Y( sobject );
}


/* the pallette of sounds used in the game */
char *coco_game_sound_names[COCO_GAME_SOUNDS_SIZE] = {
  "marty-airburst",
  "missile-f", /* "barrage-mans-rocket-f",*/
  "marty-copter-cannon-clip",
  "whang-f",
  "marty-rocket-hit",
  "marty-bomb-hit",
  "gnobots-die",
  "aaah-f",
  "aaah-f",
  "rocket-f",
  "big-boom",
  "rifle-f",
  "tank-gun-f",
  "take-off-f",
};

coco_gnum coco_game_fx_radius[COCO_GAME_SOUNDS_SIZE] = {
  1.8 * COCO_OFFICIAL_WINDOW_W,
  1.0 * COCO_OFFICIAL_WINDOW_W,
  0.8 * COCO_OFFICIAL_WINDOW_W,
  0.5 * COCO_OFFICIAL_WINDOW_W,
  1.0 * COCO_OFFICIAL_WINDOW_W,
  1.0 * COCO_OFFICIAL_WINDOW_W,
  0.5 * COCO_OFFICIAL_WINDOW_W,
  0.5 * COCO_OFFICIAL_WINDOW_W,
  1.0 * COCO_OFFICIAL_WINDOW_W,
  0.9 * COCO_OFFICIAL_WINDOW_W,
  2.4 * COCO_OFFICIAL_WINDOW_W,
  0.3 * COCO_OFFICIAL_WINDOW_W,
  1.0 * COCO_OFFICIAL_WINDOW_W,
  0.1 * COCO_OFFICIAL_WINDOW_W,
};



const coco_color coco_blip_to_color[/* <= 256 */] = {
  COCO_COLOR_TRANSPARENT, 
  COCO_COLOR_RED,         
  COCO_COLOR_RED,         
  COCO_COLOR_GREEN,       
  COCO_COLOR_ORANGE,      
  COCO_COLOR_GREY,        
  COCO_COLOR_BROWN,       
  COCO_COLOR_LIGHT_BLUE,  
  COCO_COLOR_ORANGE,      
  COCO_COLOR_WHITE,
  COCO_COLOR_YELLOW,      
  COCO_COLOR_GREY_C,      
  COCO_COLOR_DARK_YELLOW, 
  COCO_COLOR_DARK_RED,    
  COCO_COLOR_KHAKI,
  COCO_COLOR_GREY_C,      
  COCO_COLOR_KHAKI,
  COCO_COLOR_GREY_C,      
};

char *coco_nominate_action_descriptions[] = {
  "not in use",
  "adding a computer player to the Right team",
  "adding a computer player to the Left team",
  "removing a computer player from the Right team",
  "removing a computer player from the Left team",
};

char *coco_nominate_action_descriptions_other_tense[] = {
  "not in use",
  "add a computer player to the Right team",
  "add a computer player to the Left team",
  "remove a computer player the from Right team",
  "remove a computer player the from Left team",
};

const char *coco_comp_names[] = {
  "Cyber Chicken",
  "Damage Jackal",
  "Blaster Babe",
  "Armor Fiend",
  "Zippy",
  "Combat Wombat",
  "HotPoker",
  "Mr Funky",
  "Beastly Thing",
  "Foxy",
  "Colonel Flanders",
  "Spatula Squirrel",
  "BioBear",
  "ToastBoaster",
  "Punky Monkey",
  "Purple People Beater",
  "The Bamboozler",
  "Kopter Kat",
  "Admiral Smackbar",
  "Death Nugget"
};
const int coco_ncomp_names = ((sizeof( coco_comp_names ))/(sizeof( char * )));


const char *coco_major_missions[] = {
  "Welcome to Copter Boot Camp!",
  "Ferry men to checkpoint charlie.",
  "Enemy armor advancing on checkpoint charlie.  Take 'em out, quick!",
  "Knock out the enemy gun to the west.",          
  "Enemy Command Crawler sighted.  Take it out!",          
};

const char *coco_minor_missions[] = {
  "Practice manuvering your copter using the mouse.  Try not to crash.",
  "Order men with `M'.",
  "Let your men climb into your copter by landing on the ground in front of them.",
  "Fly to the field in from of Checkpoint Charlie.",
  "Press SPACE to drop your infantry.  Either be on the ground, or real high up.",
  "Wait for them to walk inside.",
  "Good Work!.",
  "Fly to your pillbox.",
  "Fly to the enemy.",
  "Use SHIFT for guns, `Z` for bombs.",
  "Well Done!",
  /* recycle fly to the enemy */
  "Ouch!  Maybe send in men to take it out on foot...",
  "Outstanding!",
  "Holy Moley!  Dodge those missiles!",
  "That armor's to strong for copters!  Order a Demolition `V'an instead.",
  "When a van reaches that crawler, we'll see just how thick that armor is...",
};

struct coco_tutorial_info coco_tutorial_infos[COCO_TUTORIAL_INFO_SIZE] =
{
  { "boot-camp", coco_setup_boot_camp, 0, 0, coco_write_boot_camp },
  { "first-blood", coco_setup_first_blood, 1, 0, NULL },
  { "hammer-blow", coco_setup_hammer_blow, 2, 0, NULL },
};

int coco_sky_option = -1; /* as interpreted by enum coco_sky_colors */

/* initialized by the background modules */
coco_color COCO_COLOR_FONT = COCO_COLOR_LIGHT_BLUE;
