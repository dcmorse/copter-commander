/*
 * Copter Commander - A 2d network helicopter game
 * Copyright (C) 2001  David Morse
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

#include <assert.h>
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

struct coco_raster *coco_plasma_rasters[COCO_PLASMA_RASTER_COUNT] = {
  &coco_plasma_yellow_raster,
  &coco_plasma_pink_raster,
  &coco_plasma_green_raster,
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


coco_mini_point 
coco_cargo_copter_rotor_points[COCO_COPTER_RASTER_COUNT] = {
  COCO_MAKE_MINI_POINT( 35, 47 ),
  COCO_MAKE_MINI_POINT( 85, 47 ),
  COCO_MAKE_MINI_POINT( 35, 47 ),
  COCO_MAKE_MINI_POINT( 85, 47 ),
};

coco_mini_point 
coco_cargo_copter_weapon_hardpoints[COCO_COPTER_RASTER_COUNT] = {
  COCO_MAKE_MINI_POINT( 35, 29 ),
  COCO_MAKE_MINI_POINT( 89, 29 ),
  COCO_MAKE_MINI_POINT( 35, 29 ),
  COCO_MAKE_MINI_POINT( 89, 29 ),
};

coco_mini_point 
coco_cargo_copter_center_of_mass[COCO_COPTER_RASTER_COUNT] = {
  COCO_MAKE_MINI_POINT( 35, 39 ),
  COCO_MAKE_MINI_POINT( 89, 39 ),
  COCO_MAKE_MINI_POINT( 35, 39 ),
  COCO_MAKE_MINI_POINT( 89, 39 ),
};

struct coco_raster *coco_cargo_copter_rasters[COCO_COPTER_RASTER_COUNT] = {
  &coco_choctaw_green_w_raster,
  &coco_choctaw_green_e_raster,
  &coco_choctaw_grey_w_raster,
  &coco_choctaw_grey_e_raster,
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
  &coco_cloud1_raster,
  &coco_cloud2_raster,
  &coco_cloud3_raster,
};


struct coco_raster *coco_balloon_rasters[COCO_BALLOON_RASTER_COUNT] = {
  /* indicies carefully chosen to be nationalities. */
  &coco_balloon_center_raster,
  &coco_balloon_w_raster,
  &coco_balloon_e_raster,
};

struct coco_raster *coco_traffic_sign_rasters[COCO_TRAFFIC_SIGN_RASTER_COUNT] = {
  &coco_wrong_way_raster,
  &coco_tree1_raster,
  &coco_cactus_raster
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


struct coco_raster *coco_balloon_factory_rasters[COCO_BALLOON_FACTORY_RASTER_COUNT] = {
  /* indicies carefully chosen to be nationalities - COCO_GOOD_GUYS. */
  &coco_balloon_factory_base_raster, 
  &coco_balloon_factory_stack_raster,
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
  &coco_jet_exhaust_ww_raster,
  &coco_jet_exhaust_ws_raster,
  &coco_jet_exhaust_ss_raster,
  &coco_jet_exhaust_es_raster,
  &coco_jet_exhaust_ee_raster,
  &coco_jet_exhaust_w_raster,
  &coco_jet_exhaust_s_raster,
  &coco_jet_exhaust_e_raster,
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
  "marty-aa-gun",
  "abuse-whang",
  "marty-rocket-hit",
  "marty-bomb-hit",
  "gnobots-die",
  "abuse-scream",
  "aaah-f",
  "rocket-f",
  "big-boom",
  "rifle-f",
  "tank-gun-f",
  "take-off-f",
  "abuse-mech-warm-up",
  "openal-plasma",
  "pop",
  "abuse-whang2",
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
  0.1 * COCO_OFFICIAL_WINDOW_W,
  1.0 * COCO_OFFICIAL_WINDOW_W,
  COCO_GNUM_MAX,
  0.5 * COCO_OFFICIAL_WINDOW_W,
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
  "Ferry men to Checkpoint Charlie.",
  "Enemy armor advancing on Checkpoint Charlie.  Take 'em out, quick!",
  "Enemy Command Crawler sighted.  Take it out!",          
};

const char *coco_minor_missions[] = {
  "Practice manuvering your copter using the mouse.  Try not to crash.",
  "Order a squad of men by holding down `M'.",
  "Let your men climb into your copter by landing on the ground in front of them.",
  "Fly to the field in front of Checkpoint Charlie.",
  "Press SPACE to drop your infantry.  Either be on the ground, or real high up.",
  "Wait for them to walk inside.",
  "Good Work!",
  "Fly to your pillbox.",
  "Fly to the enemy.",
  "DODGE !!",
  "Use SHIFT for guns, `Z` for bombs, LEFT-CLICK to turn around.",
  "Blow it up and you graduate from Boot with full honors!",
  "That armor's to strong for copters!  Order a Demolition `V'an instead.",
  "When a van reaches that crawler, we'll see just how thick that armor is...",
  "Victory is ours !!!  Check the help menu for advanced stuff.",
};

struct coco_tutorial_info coco_tutorial_infos[COCO_TUTORIAL_INFO_SIZE] =
{
  { "boot-camp", 0, 0, coco_update_boot_camp },
  /* { "first-blood", 1, 0, NULL },
     { "hammer-blow", 2, 0, NULL }, */
};

int coco_sky_option = -1; /* as interpreted by enum coco_sky_colors */

/* initialized by the background modules */
coco_color COCO_COLOR_FONT = COCO_COLOR_LIGHT_BLUE;


/* moved to coco.h
   #define COCO_MAX_TORSO_PARTS 24
   #define COCO_MAX_LEG_PARTS 38
   #define COCO_LEFTY_LEG_PART 19
   #define COCO_MIN_IDLE_PARTS 0
   #define COCO_MAX_IDLE_PARTS 6
   #define COCO_MIN_SQUAT_PARTS 6
   #define COCO_MAX_SQUAT_PARTS 10
   #define COCO_MIN_WALK_PARTS 10
   #define COCO_MAX_WALK_PARTS 18
   #define COCO_AKIMBO_PART 18
*/

/* One day should maybe crop these guys down to the bare minimum,
   but for now many rasters are generous.  */
struct coco_mech_part coco_torso_parts[COCO_MAX_TORSO_PARTS] = {
  { &coco_torso00_raster,  0,  0, 0 },
  { &coco_torso01_raster,  0,  0, 0 },
  { &coco_torso02_raster,  2,  0, 0 },
  { &coco_torso03_raster,  8,  0, 0 },
  { &coco_torso04_raster,  6,  0, 0 },
  { &coco_torso05_raster,  4,  0, 0 },
  { &coco_torso06_raster,  0,  0, 0 },
  { &coco_torso07_raster, -4,  0, 0 },
  { &coco_torso08_raster, -6,  0, 0 },
  { &coco_torso09_raster, -8,  0, 0 },
  { &coco_torso10_raster, -7,  0, 0 },
  { &coco_torso11_raster, -6,  0, 0 },
  { &coco_torso12_raster, -6,  0, 0 },
  { &coco_torso13_raster, -4, -8, 0 },
  { &coco_torso14_raster, -2, -11, 0 },
  { &coco_torso15_raster, -2, -13, 0 },
  { &coco_torso16_raster, -2, -14, 0 },
  { &coco_torso17_raster,  0, -14, 0 },
  { &coco_torso18_raster,  0, -15, 0 },
  { &coco_torso19_raster,  2, -15, 0 },
  { &coco_torso20_raster,  2, -14, 0 },
  { &coco_torso21_raster,  4, -12, 0 },
  { &coco_torso22_raster,  4, -9, 0 },
  { &coco_torso23_raster,  2, -4, 0 },
};

struct coco_mech_part coco_leg_parts[COCO_MAX_LEG_PARTS] = {
  /* these guys' integers mirror their lefty equivalents */
  { &coco_legs_idle_0_raster  },
  { &coco_legs_idle_0_raster  },
  { &coco_legs_idle_0_raster  },
  { &coco_legs_idle_5_raster  },
  { &coco_legs_idle_5_raster  },
  { &coco_legs_idle_5_raster  },
  { &coco_legs_squat_0_raster },
  { &coco_legs_squat_1_raster },
  { &coco_legs_squat_2_raster },
  { &coco_legs_squat_3_raster },
  { &coco_legs_walk_0_raster  },
  { &coco_legs_walk_1_raster  },
  { &coco_legs_walk_2_raster  },
  { &coco_legs_walk_3_raster  },
  { &coco_legs_walk_4_raster  },
  { &coco_legs_walk_5_raster  },
  { &coco_legs_walk_6_raster  },
  { &coco_legs_walk_7_raster  },
  { &coco_legs_walk_8_raster  },
  { &coco_legs_akimbo_raster  },
  { &coco_legs_left_idle_0_raster,  4, -2, 32 },
  { &coco_legs_left_idle_0_raster,  4, -2, 32 },
  { &coco_legs_left_idle_0_raster,  4, -2, 32 },
  { &coco_legs_left_idle_5_raster,  4, -2, 30 },
  { &coco_legs_left_idle_5_raster,  4, -2, 30 },
  { &coco_legs_left_idle_5_raster,  4, -2, 30 },
  { &coco_legs_left_squat_0_raster, 8, -2, 38 },
  { &coco_legs_left_squat_1_raster, 8, -2, 34 },
  { &coco_legs_left_squat_2_raster, 4, -2, 28 },
  { &coco_legs_left_squat_3_raster, 0, -2, 26 },
  { &coco_legs_left_walk_0_raster, 10,  0, 30 },
  { &coco_legs_left_walk_1_raster, 10,  0, 30 },
  { &coco_legs_left_walk_2_raster, 16,  0, 30 },
  { &coco_legs_left_walk_3_raster,  6,  0, 30 },
  { &coco_legs_left_walk_4_raster, 12,  0, 30 },
  { &coco_legs_left_walk_5_raster, 10,  0, 30 },
  { &coco_legs_left_walk_6_raster, 10,  0, 30 },
  { &coco_legs_left_walk_7_raster, 16,  0, 30 },
  { &coco_legs_left_walk_8_raster, 16,  0, 30 },
  { &coco_legs_left_akimbo_raster,  8,  3, 30 },
};

/*  ORIGINAL
struct coco_mech_part coco_leg_parts[COCO_MAX_LEG_PARTS] = {
  { &coco_legs_idle_0_raster,  0, -1, 16 },
  { &coco_legs_idle_0_raster,  0, -1, 16 },
  { &coco_legs_idle_0_raster,  0, -1, 16 },
  { &coco_legs_idle_5_raster,  0, -1, 15 },
  { &coco_legs_idle_5_raster,  0, -1, 15 },
  { &coco_legs_idle_5_raster,  0, -1, 15 },
  { &coco_legs_squat_0_raster, 0, -1, 19 },
  { &coco_legs_squat_1_raster, 0, -1, 17 },
  { &coco_legs_squat_2_raster, 0, -1, 14 },
  { &coco_legs_squat_3_raster, 0, -1, 13 },
  { &coco_legs_walk_0_raster,  0, -1, 15 },
  { &coco_legs_walk_1_raster,  0, -1, 15 },
  { &coco_legs_walk_2_raster,  0, -1, 15 },
  { &coco_legs_walk_3_raster,  0, -1, 15 },
  { &coco_legs_walk_4_raster,  0, -1, 15 },
  { &coco_legs_walk_5_raster,  0, -1, 15 },
  { &coco_legs_walk_6_raster,  0, -1, 15 },
  { &coco_legs_walk_7_raster,  0, -1, 15 },
  { &coco_legs_akimbo_raster,  0, -1, 15 },
  /+ mirror that of above +/
  { &coco_legs_left_idle_0_raster,  0, -1, 16 },
  { &coco_legs_left_idle_0_raster,  0, -1, 16 },
  { &coco_legs_left_idle_0_raster,  0, -1, 16 },
  { &coco_legs_left_idle_5_raster,  0, -1, 15 },
  { &coco_legs_left_idle_5_raster,  0, -1, 15 },
  { &coco_legs_left_idle_5_raster,  0, -1, 15 },
  { &coco_legs_left_squat_0_raster, 0, -1, 19 },
  { &coco_legs_left_squat_1_raster, 0, -1, 17 },
  { &coco_legs_left_squat_2_raster, 0, -1, 14 },
  { &coco_legs_left_squat_3_raster, 0, -1, 13 },
  { &coco_legs_left_walk_0_raster,  0, -1, 15 },
  { &coco_legs_left_walk_1_raster,  0, -1, 15 },
  { &coco_legs_left_walk_2_raster,  0, -1, 15 },
  { &coco_legs_left_walk_3_raster,  0, -1, 15 },
  { &coco_legs_left_walk_4_raster,  0, -1, 15 },
  { &coco_legs_left_walk_5_raster,  0, -1, 15 },
  { &coco_legs_left_walk_6_raster,  0, -1, 15 },
  { &coco_legs_left_walk_7_raster,  0, -1, 15 },
  { &coco_legs_left_akimbo_raster,  0, -1, 15 },
};

p

 */

/* this function could be done at compile time, but its very fast in any
   case. */
void 
coco_mech_raster_setup( void )
{
  int i, j;
  coco_gnum cw = COCO_MECH_CLASS->canonical_w;
  coco_gnum cw2 = cw >> 1;
  for (i=0, j=COCO_LEFTY_LEG_PART; i<COCO_LEFTY_LEG_PART; i++, j++)
    {
      coco_gnum rw = coco_leg_parts[j].r->w;
      coco_gnum dx = coco_leg_parts[j].dx;
      coco_gnum dy = coco_leg_parts[j].dy;
      coco_gnum dh = coco_leg_parts[j].dh;
      /* center mech on legs */
      dx = coco_leg_parts[j].dx = dx - cw2 + 6;
      coco_leg_parts[i].dx = (- rw) + cw - dx;
      assert( abs( coco_leg_parts[j].dx ) < 1000 );
      assert( abs( coco_leg_parts[i].dx ) < 1000 );
      /* copy other values */
      coco_leg_parts[i].dy = dy;
      coco_leg_parts[i].dh = dh;
    }
  for (i=0; i<COCO_MAX_TORSO_PARTS; i++)
    {
      coco_torso_parts[i].dx -= cw2;
    }
}




/* if adding to this be sure to change the count constant in sobject.lisp */
coco_sobject_cl *coco_aa_targetable_classes[] = {
  COCO_COPTER_CLASS, COCO_MECH_CLASS
};

coco_sobject_cl *coco_at_targetable_classes[] = {
  COCO_MECH_CLASS, COCO_GROUND_VEHICLE_CLASS
};

coco_bool coco_key_special( coco_key key )
{
  return (key<COCO_KEY_SPACE) || (key>= COCO_KEY_DELETE)
    || ((key >='A') && (key <= 'Z'));
}

