/* This file has been automatically generated.  Do not edit it directly. */

/* Class structure definition */

struct coco_sobject_class_ {
  unsigned char cl_index;
  int init_by_list_count;
  coco_sobject *(* init_by_list_fn )(coco_sobject *, coco_sobject_cl *, coco_lnum *);
  char * name;
  coco_sobject_cl * super;
  coco_phase phase;
  coco_bool rotate_p;
  coco_gnum dx_max;
  coco_gnum dy_max;
  coco_gnum x_force;
  coco_gnum y_force;
  coco_health health_max;
  coco_health damage;
  coco_bool clip_to_board;
  coco_blip_triplet default_blip_color_index;
  coco_gnum canonical_w;
  coco_gnum canonical_h;
  coco_znum z;
  unsigned int raster_count;
  struct coco_raster ** rasters;
  coco_cnum (* cobject_y_fn )(coco_cobject *);
  unsigned char (* cobject_raster_index_fn )(coco_cobject *);
  int (* cobject_guess_nationality_fn )(coco_cobject *);
  coco_sobject_cl * bullet_class;
  void (* cobject_ntoh )(coco_cobject *);
  void (* cobject_draw )(coco_cobject *, coco_sobject_cl *);
  unsigned char shrapnel_count;
  unsigned char refund;
  unsigned char cost;
  unsigned char collision_priority;
  coco_mini_rectangle collision_rectangle;
  coco_gnum rotor_radius;
  unsigned short max_bullets;
  unsigned char max_bombs;
  unsigned char max_rockets;
  unsigned char max_missiles;
  unsigned char max_internal_infantry;
  unsigned short max_fuel;
  coco_mini_point * rotor_points;
  coco_mini_point * weapon_hardpoints;
  coco_mini_point * center_of_mass;
  unsigned char gun_barrel_length;
  coco_bool infantry_take_buildings_p;
  unsigned char spark_count;
  unsigned char thickness;
  coco_gnum muzzle_velocity;
  coco_u32 lifespan;
  unsigned char roof_tab;
  coco_sound_type airburst_sound_type;
  coco_sound_type fire_bullet_sound_type;
  unsigned char targetable_classes_count;
  coco_sobject_cl * targetable_classes;
  coco_sobject * (* drop_paratrooper)( coco_sobject *, coco_sobject_cl * , coco_sstate * );
  coco_sobject * (* launch_missile)( coco_sobject *, coco_sobject_cl * , coco_sstate * , coco_sobject * );
  coco_sobject * (* fire_bullet)( coco_sobject *, coco_sobject_cl * , coco_sstate * , coco_sobject * );
  void (* fire_bullet_coords)( coco_sobject *, coco_sobject_cl * , coco_sobject * , coco_gnum * , coco_gnum * , coco_gnum * , coco_gnum * );
  void (* toggle_facing)( coco_sobject *, coco_sobject_cl * );
  unsigned char (* raster_flip_index)( coco_sobject *, coco_sobject_cl * , unsigned char );
  void (* enter_friendly_bunker)( coco_sobject *, coco_sobject_cl * , coco_sobject * , coco_sstate * );
  coco_bool (* infantry_enter_hut)( coco_sobject *, coco_sobject_cl * , coco_sobject * , coco_sstate * , coco_bool * );
  coco_gnum (* camera_velocity_offset)( coco_sobject *, coco_sobject_cl * );
  coco_bool (* sobject_is_visible)( coco_sobject *, coco_sobject_cl * , coco_gnum , coco_gnum , coco_sobject * );
  coco_blip_index (* sobject_blip_color_index)( coco_sobject *, coco_sobject_cl * , int , coco_bool , coco_sstate * );
  coco_bool (* ground_trooper_target_p)( coco_sobject *, coco_sobject_cl * , coco_sobject * );
  void (* sobject_neutral_building_collide)( coco_sobject *, coco_sobject_cl * , coco_sobject * );
  void (* sobject_to_cobject)( coco_sobject *, coco_sobject_cl * , coco_cobject * , coco_gnum , coco_bool );
  void (* ground_collide)( coco_sobject *, coco_sobject_cl * , coco_sstate * );
  coco_health (* effective_damage)( coco_sobject *, coco_sobject_cl * , coco_sobject * );
  void (* friendly_collision)( coco_sobject *, coco_sobject_cl * , coco_sobject * , coco_sstate * , coco_bool * , coco_bool * );
  void (* hostile_collision)( coco_sobject *, coco_sobject_cl * , coco_sobject * , coco_sstate * , coco_bool * , coco_bool * );
  void (* take_damage)( coco_sobject *, coco_sobject_cl * , coco_health , coco_sstate * );
  void (* sobject_delete)( coco_sobject *, coco_sobject_cl * , coco_sstate * );
  void (* kaboom)( coco_sobject *, coco_sobject_cl * , coco_sstate * );
  coco_bool (* sobject_update_state)( coco_sobject *, coco_sobject_cl * , coco_sstate * );
  void (* sobject_unlink)( coco_sobject *, coco_sobject_cl * , int , coco_sstate * );
};

/* Class structures themselves */

extern coco_sobject_cl coco_sobject_classes[];
#define COCO_SOBJECT_CLASS_INDEX 0
#define COCO_SNDOBJECT_CLASS_INDEX 1
#define COCO_CAMERA_CLASS_INDEX 2
#define COCO_INVISO_CAMERA_CLASS_INDEX 3
#define COCO_COPTER_CLASS_INDEX 4
#define COCO_COBRA_CLASS_INDEX 5
#define COCO_BUBBLE_COPTER_CLASS_INDEX 6
#define COCO_SMOKE_CLASS_INDEX 7
#define COCO_CLOUD_CLASS_INDEX 8
#define COCO_BALLOON_PART_CLASS_INDEX 9
#define COCO_BALLOON_CLASS_INDEX 10
#define COCO_BALLOON_LINE_CLASS_INDEX 11
#define COCO_BUILDING_CLASS_INDEX 12
#define COCO_TRAFFIC_SIGN_CLASS_INDEX 13
#define COCO_HUT_CLASS_INDEX 14
#define COCO_BUNKER_CLASS_INDEX 15
#define COCO_PILLBOX_CLASS_INDEX 16
#define COCO_LANDING_PAD_CLASS_INDEX 17
#define COCO_HEADQUARTERS_CLASS_INDEX 18
#define COCO_GUN_CLASS_INDEX 19
#define COCO_AA_GUN_CLASS_INDEX 20
#define COCO_AT_GUN_CLASS_INDEX 21
#define COCO_GUN_RUBBLE_CLASS_INDEX 22
#define COCO_GROUND_UNIT_CLASS_INDEX 23
#define COCO_GROUND_VEHICLE_CLASS_INDEX 24
#define COCO_DEMO_VAN_CLASS_INDEX 25
#define COCO_TANK_CLASS_INDEX 26
#define COCO_FIRETRUCK_CLASS_INDEX 27
#define COCO_INFANTRY_CLASS_INDEX 28
#define COCO_ENGINEER_CLASS_INDEX 29
#define COCO_MUNITION_CLASS_INDEX 30
#define COCO_BULLET_CLASS_INDEX 31
#define COCO_RIFLE_BULLET_CLASS_INDEX 32
#define COCO_TANK_BULLET_CLASS_INDEX 33
#define COCO_BOMB_CLASS_INDEX 34
#define COCO_ABSTRACT_MISSILE_CLASS_INDEX 35
#define COCO_ROCKET_CLASS_INDEX 36
#define COCO_MISSILE_CLASS_INDEX 37
#define COCO_SPARK_CLASS_INDEX 38
#define COCO_SHRAPNEL_CLASS_INDEX 39
#define COCO_METAL_SHRAPNEL_CLASS_INDEX 40
#define COCO_BIG_METAL_SHRAPNEL_CLASS_INDEX 41
#define COCO_MED_METAL_SHRAPNEL_CLASS_INDEX 42
#define COCO_SMALL_METAL_SHRAPNEL_CLASS_INDEX 43
#define COCO_EXPLOSION_CLASS_INDEX 44
#define COCO_SOBJECT_CLASS_COUNT 45


#define COCO_SOBJECT_CLASS (coco_sobject_classes + COCO_SOBJECT_CLASS_INDEX)
#define COCO_SNDOBJECT_CLASS (coco_sobject_classes + COCO_SNDOBJECT_CLASS_INDEX)
#define COCO_CAMERA_CLASS (coco_sobject_classes + COCO_CAMERA_CLASS_INDEX)
#define COCO_INVISO_CAMERA_CLASS (coco_sobject_classes + COCO_INVISO_CAMERA_CLASS_INDEX)
#define COCO_COPTER_CLASS (coco_sobject_classes + COCO_COPTER_CLASS_INDEX)
#define COCO_COBRA_CLASS (coco_sobject_classes + COCO_COBRA_CLASS_INDEX)
#define COCO_BUBBLE_COPTER_CLASS (coco_sobject_classes + COCO_BUBBLE_COPTER_CLASS_INDEX)
#define COCO_SMOKE_CLASS (coco_sobject_classes + COCO_SMOKE_CLASS_INDEX)
#define COCO_CLOUD_CLASS (coco_sobject_classes + COCO_CLOUD_CLASS_INDEX)
#define COCO_BALLOON_PART_CLASS (coco_sobject_classes + COCO_BALLOON_PART_CLASS_INDEX)
#define COCO_BALLOON_CLASS (coco_sobject_classes + COCO_BALLOON_CLASS_INDEX)
#define COCO_BALLOON_LINE_CLASS (coco_sobject_classes + COCO_BALLOON_LINE_CLASS_INDEX)
#define COCO_BUILDING_CLASS (coco_sobject_classes + COCO_BUILDING_CLASS_INDEX)
#define COCO_TRAFFIC_SIGN_CLASS (coco_sobject_classes + COCO_TRAFFIC_SIGN_CLASS_INDEX)
#define COCO_HUT_CLASS (coco_sobject_classes + COCO_HUT_CLASS_INDEX)
#define COCO_BUNKER_CLASS (coco_sobject_classes + COCO_BUNKER_CLASS_INDEX)
#define COCO_PILLBOX_CLASS (coco_sobject_classes + COCO_PILLBOX_CLASS_INDEX)
#define COCO_LANDING_PAD_CLASS (coco_sobject_classes + COCO_LANDING_PAD_CLASS_INDEX)
#define COCO_HEADQUARTERS_CLASS (coco_sobject_classes + COCO_HEADQUARTERS_CLASS_INDEX)
#define COCO_GUN_CLASS (coco_sobject_classes + COCO_GUN_CLASS_INDEX)
#define COCO_AA_GUN_CLASS (coco_sobject_classes + COCO_AA_GUN_CLASS_INDEX)
#define COCO_AT_GUN_CLASS (coco_sobject_classes + COCO_AT_GUN_CLASS_INDEX)
#define COCO_GUN_RUBBLE_CLASS (coco_sobject_classes + COCO_GUN_RUBBLE_CLASS_INDEX)
#define COCO_GROUND_UNIT_CLASS (coco_sobject_classes + COCO_GROUND_UNIT_CLASS_INDEX)
#define COCO_GROUND_VEHICLE_CLASS (coco_sobject_classes + COCO_GROUND_VEHICLE_CLASS_INDEX)
#define COCO_DEMO_VAN_CLASS (coco_sobject_classes + COCO_DEMO_VAN_CLASS_INDEX)
#define COCO_TANK_CLASS (coco_sobject_classes + COCO_TANK_CLASS_INDEX)
#define COCO_FIRETRUCK_CLASS (coco_sobject_classes + COCO_FIRETRUCK_CLASS_INDEX)
#define COCO_INFANTRY_CLASS (coco_sobject_classes + COCO_INFANTRY_CLASS_INDEX)
#define COCO_ENGINEER_CLASS (coco_sobject_classes + COCO_ENGINEER_CLASS_INDEX)
#define COCO_MUNITION_CLASS (coco_sobject_classes + COCO_MUNITION_CLASS_INDEX)
#define COCO_BULLET_CLASS (coco_sobject_classes + COCO_BULLET_CLASS_INDEX)
#define COCO_RIFLE_BULLET_CLASS (coco_sobject_classes + COCO_RIFLE_BULLET_CLASS_INDEX)
#define COCO_TANK_BULLET_CLASS (coco_sobject_classes + COCO_TANK_BULLET_CLASS_INDEX)
#define COCO_BOMB_CLASS (coco_sobject_classes + COCO_BOMB_CLASS_INDEX)
#define COCO_ABSTRACT_MISSILE_CLASS (coco_sobject_classes + COCO_ABSTRACT_MISSILE_CLASS_INDEX)
#define COCO_ROCKET_CLASS (coco_sobject_classes + COCO_ROCKET_CLASS_INDEX)
#define COCO_MISSILE_CLASS (coco_sobject_classes + COCO_MISSILE_CLASS_INDEX)
#define COCO_SPARK_CLASS (coco_sobject_classes + COCO_SPARK_CLASS_INDEX)
#define COCO_SHRAPNEL_CLASS (coco_sobject_classes + COCO_SHRAPNEL_CLASS_INDEX)
#define COCO_METAL_SHRAPNEL_CLASS (coco_sobject_classes + COCO_METAL_SHRAPNEL_CLASS_INDEX)
#define COCO_BIG_METAL_SHRAPNEL_CLASS (coco_sobject_classes + COCO_BIG_METAL_SHRAPNEL_CLASS_INDEX)
#define COCO_MED_METAL_SHRAPNEL_CLASS (coco_sobject_classes + COCO_MED_METAL_SHRAPNEL_CLASS_INDEX)
#define COCO_SMALL_METAL_SHRAPNEL_CLASS (coco_sobject_classes + COCO_SMALL_METAL_SHRAPNEL_CLASS_INDEX)
#define COCO_EXPLOSION_CLASS (coco_sobject_classes + COCO_EXPLOSION_CLASS_INDEX)

/* Instance Definitions */

struct coco_explosion {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
};

struct coco_small_metal_shrapnel {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
  coco_color_index color_index;
};

struct coco_med_metal_shrapnel {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
  coco_color_index color_index;
};

struct coco_big_metal_shrapnel {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
  coco_color_index color_index;
};

struct coco_metal_shrapnel {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
  coco_color_index color_index;
};

struct coco_shrapnel {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
};

struct coco_spark {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
  signed char spark_dx;
  signed char spark_dy;
};

struct coco_missile {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
  unsigned int fuel;
  int target_index;
};

struct coco_rocket {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
};

struct coco_abstract_missile {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
};

struct coco_bomb {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
  unsigned char horiz_ticks;
};

struct coco_tank_bullet {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
  coco_u32 expiration_date;
};

struct coco_rifle_bullet {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
  coco_u32 expiration_date;
};

struct coco_bullet {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
  coco_u32 expiration_date;
};

struct coco_munition {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
};

struct coco_engineer {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
  unsigned char parachutes;
  coco_gnum parachute_height;
};

struct coco_infantry {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
  unsigned char parachutes;
  coco_gnum parachute_height;
};

struct coco_firetruck {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
  unsigned char smoke_countdown;
};

struct coco_tank {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
};

struct coco_demo_van {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
};

struct coco_ground_vehicle {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
};

struct coco_ground_unit {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
};

struct coco_gun_rubble {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
  unsigned char reincarnation_class_index;
  int points_till_reincarnation;
  coco_nationality reincarnation_nationality;
};

struct coco_at_gun {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
  unsigned short cooldown;
};

struct coco_aa_gun {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
  unsigned short cooldown;
};

struct coco_gun {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
  unsigned short cooldown;
};

struct coco_headquarters {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
  unsigned char smoke_countdown;
  int pillbox_index;
};

struct coco_landing_pad {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
};

struct coco_pillbox {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
  unsigned char defenders;
  int headquarters_index;
};

struct coco_bunker {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
  unsigned char defenders;
};

struct coco_hut {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
  int line_index;
  int points_till_reincarnation;
};

struct coco_traffic_sign {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
};

struct coco_building {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
};

struct coco_balloon_line {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
  int hut_index;
  int balloon_index;
};

struct coco_balloon {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
  int line_index;
  coco_u32 balloon_time_phase;
};

struct coco_balloon_part {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
};

struct coco_cloud {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
};

struct coco_smoke {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
};

struct coco_bubble_copter {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
  unsigned short bullets;
  unsigned char bombs;
  unsigned char missiles;
  unsigned char rockets;
  unsigned char paratroopers;
  unsigned short engitroopers;
  unsigned short fuel;
  coco_bool missile_cooldown;
  coco_u32 missile_cooldown_time;
  coco_u32 next_missilebeat_on_pad;
  coco_u32 last_bomb_drop_time;
  coco_u32 last_rocket_launch_time;
  coco_bool landed_p;
  coco_bool landed_on_pad_p;
};

struct coco_cobra {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
  unsigned short bullets;
  unsigned char bombs;
  unsigned char missiles;
  unsigned char rockets;
  unsigned char paratroopers;
  unsigned short engitroopers;
  unsigned short fuel;
  coco_bool missile_cooldown;
  coco_u32 missile_cooldown_time;
  coco_u32 next_missilebeat_on_pad;
  coco_u32 last_bomb_drop_time;
  coco_u32 last_rocket_launch_time;
  coco_bool landed_p;
  coco_bool landed_on_pad_p;
};

struct coco_copter {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
  unsigned short bullets;
  unsigned char bombs;
  unsigned char missiles;
  unsigned char rockets;
  unsigned char paratroopers;
  unsigned short engitroopers;
  unsigned short fuel;
  coco_bool missile_cooldown;
  coco_u32 missile_cooldown_time;
  coco_u32 next_missilebeat_on_pad;
  coco_u32 last_bomb_drop_time;
  coco_u32 last_rocket_launch_time;
  coco_bool landed_p;
  coco_bool landed_on_pad_p;
};

struct coco_inviso_camera {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
};

struct coco_camera {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
};

struct coco_sndobject {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
  coco_sound_type sound;
};

struct coco_every_sobject {
  coco_sobject_cl * cl;
  coco_bool in_use;
  signed char camera_index;
  signed char comp_index;
  coco_facing facing;
  coco_bool new_flag;
  unsigned int index;
  coco_gnum x;
  coco_gnum y;
  coco_gnum w;
  coco_gnum h;
  coco_gnum dx;
  coco_gnum dy;
  coco_gnum dx_public;
  coco_gnum dy_public;
  coco_u32 locked_on_time;
  unsigned char raster_index;
  coco_nationality nationality;
  coco_health health;
  coco_angle angle;
  coco_blip_index scratch_blip;
};

union coco_sobject_inst {
  struct coco_explosion explosion;
  struct coco_small_metal_shrapnel small_metal_shrapnel;
  struct coco_med_metal_shrapnel med_metal_shrapnel;
  struct coco_big_metal_shrapnel big_metal_shrapnel;
  struct coco_metal_shrapnel metal_shrapnel;
  struct coco_shrapnel shrapnel;
  struct coco_spark spark;
  struct coco_missile missile;
  struct coco_rocket rocket;
  struct coco_abstract_missile abstract_missile;
  struct coco_bomb bomb;
  struct coco_tank_bullet tank_bullet;
  struct coco_rifle_bullet rifle_bullet;
  struct coco_bullet bullet;
  struct coco_munition munition;
  struct coco_engineer engineer;
  struct coco_infantry infantry;
  struct coco_firetruck firetruck;
  struct coco_tank tank;
  struct coco_demo_van demo_van;
  struct coco_ground_vehicle ground_vehicle;
  struct coco_ground_unit ground_unit;
  struct coco_gun_rubble gun_rubble;
  struct coco_at_gun at_gun;
  struct coco_aa_gun aa_gun;
  struct coco_gun gun;
  struct coco_headquarters headquarters;
  struct coco_landing_pad landing_pad;
  struct coco_pillbox pillbox;
  struct coco_bunker bunker;
  struct coco_hut hut;
  struct coco_traffic_sign traffic_sign;
  struct coco_building building;
  struct coco_balloon_line balloon_line;
  struct coco_balloon balloon;
  struct coco_balloon_part balloon_part;
  struct coco_cloud cloud;
  struct coco_smoke smoke;
  struct coco_bubble_copter bubble_copter;
  struct coco_cobra cobra;
  struct coco_copter copter;
  struct coco_inviso_camera inviso_camera;
  struct coco_camera camera;
  struct coco_sndobject sndobject;
  struct coco_every_sobject every;
};

int coco_sobject_subtype_p( coco_sobject_cl *sub, coco_sobject_cl *super );
/* Generic Functions */

coco_sobject *
coco_drop_paratrooper( coco_sobject * inst, coco_sobject_cl * cl, coco_sstate * sstate );
coco_sobject *
coco_launch_missile( coco_sobject * inst, coco_sobject_cl * cl, coco_sstate * sstate, coco_sobject * target );
coco_sobject *
coco_fire_bullet( coco_sobject * inst, coco_sobject_cl * cl, coco_sstate * sstate, coco_sobject * target_or_null );
void
coco_fire_bullet_coords( coco_sobject * inst, coco_sobject_cl * cl, coco_sobject * target, coco_gnum * dx_return, coco_gnum * dy_return, coco_gnum * x_return, coco_gnum * y_return );
void
coco_toggle_facing( coco_sobject * inst, coco_sobject_cl * cl );
unsigned char
coco_raster_flip_index( coco_sobject * inst, coco_sobject_cl * cl, unsigned char index );
void
coco_enter_friendly_bunker( coco_sobject * inst, coco_sobject_cl * cl, coco_sobject * bunker, coco_sstate * sstate );
coco_bool
coco_infantry_enter_hut( coco_sobject * inst, coco_sobject_cl * cl, coco_sobject * hut, coco_sstate * sstate, coco_bool * stand_still_p );
coco_gnum
coco_camera_velocity_offset( coco_sobject * inst, coco_sobject_cl * cl );
coco_bool
coco_sobject_is_visible( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum cam_left, coco_gnum cam_right, coco_sobject * cam_or_null );
coco_blip_index
coco_sobject_blip_color_index( coco_sobject * inst, coco_sobject_cl * cl, int p_or_c_index, coco_bool is_comp, coco_sstate * sstate );
coco_bool
coco_ground_trooper_target_p( coco_sobject * inst, coco_sobject_cl * cl, coco_sobject * trooper );
void
coco_sobject_neutral_building_collide( coco_sobject * inst, coco_sobject_cl * cl, coco_sobject * building );
void
coco_sobject_to_cobject( coco_sobject * inst, coco_sobject_cl * cl, coco_cobject * cobject, coco_gnum cam_left, coco_bool network_byte_order );
void
coco_ground_collide( coco_sobject * inst, coco_sobject_cl * cl, coco_sstate * sstate );
coco_health
coco_effective_damage( coco_sobject * inst, coco_sobject_cl * cl, coco_sobject * damager );
void
coco_friendly_collision( coco_sobject * inst, coco_sobject_cl * cl, coco_sobject * obj2, coco_sstate * sstate, coco_bool * stat1, coco_bool * stat2 );
void
coco_hostile_collision( coco_sobject * inst, coco_sobject_cl * cl, coco_sobject * obj2, coco_sstate * sstate, coco_bool * stat1, coco_bool * stat2 );
void
coco_take_damage( coco_sobject * inst, coco_sobject_cl * cl, coco_health health, coco_sstate * sstate );
void
coco_sobject_delete( coco_sobject * inst, coco_sobject_cl * cl, coco_sstate * sstate );
void
coco_kaboom( coco_sobject * inst, coco_sobject_cl * cl, coco_sstate * sstate );
coco_bool
coco_sobject_update_state( coco_sobject * inst, coco_sobject_cl * cl, coco_sstate * sstate );
void
coco_sobject_unlink( coco_sobject * inst, coco_sobject_cl * cl, int link_index, coco_sstate * sstate );

/* Instance Initializers */

coco_sobject *
coco_init_sobject( coco_sobject * inst, coco_sobject_cl * cl, coco_facing facing, coco_gnum x, coco_gnum y, coco_nationality nationality );
coco_sobject *
coco_init_sndobject( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_gnum y, coco_sound_type sound );
coco_sobject *
coco_init_camera( coco_sobject * inst, coco_sobject_cl * cl, signed char camera_index, coco_facing facing, coco_gnum x, coco_gnum y, coco_nationality nationality );
coco_sobject *
coco_init_inviso_camera( coco_sobject * inst, coco_sobject_cl * cl, signed char camera_index, coco_gnum x, coco_nationality nationality );
coco_sobject *
coco_init_copter( coco_sobject * inst, coco_sobject_cl * cl, signed char camera_index, coco_facing facing, coco_gnum x, coco_gnum y, coco_nationality nationality );
coco_sobject *
coco_init_cobra( coco_sobject * inst, coco_sobject_cl * cl, signed char camera_index, coco_facing facing, coco_gnum x, coco_gnum y, coco_nationality nationality );
coco_sobject *
coco_init_bubble_copter( coco_sobject * inst, coco_sobject_cl * cl, signed char camera_index, coco_facing facing, coco_gnum x, coco_gnum y, coco_nationality nationality );
coco_sobject *
coco_init_smoke( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_gnum y, unsigned char raster_index );
coco_sobject *
coco_init_cloud( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_gnum y );
coco_sobject *
coco_init_balloon_part( coco_sobject * inst, coco_sobject_cl * cl, coco_facing facing, coco_gnum x, coco_gnum y, coco_nationality nationality );
coco_sobject *
coco_init_balloon( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_gnum y, unsigned char raster_index, coco_nationality nationality );
coco_sobject *
coco_init_balloon_line( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_gnum y, coco_gnum h, coco_nationality nationality, int hut_index, int balloon_index );
coco_sobject *
coco_init_building( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_nationality nationality );
coco_sobject *
coco_init_traffic_sign( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, unsigned char raster_index );
coco_sobject *
coco_init_hut( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_nationality nationality );
coco_sobject *
coco_init_bunker( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_nationality nationality, unsigned char defenders );
coco_sobject *
coco_init_pillbox( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_nationality nationality, unsigned char defenders, int headquarters_index );
coco_sobject *
coco_init_landing_pad( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x );
coco_sobject *
coco_init_headquarters( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_nationality nationality, int pillbox_index );
coco_sobject *
coco_init_gun( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_nationality nationality );
coco_sobject *
coco_init_aa_gun( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_nationality nationality );
coco_sobject *
coco_init_at_gun( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_nationality nationality );
coco_sobject *
coco_init_gun_rubble( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, unsigned char reincarnation_class_index );
coco_sobject *
coco_init_ground_unit( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_nationality nationality );
coco_sobject *
coco_init_ground_vehicle( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_nationality nationality );
coco_sobject *
coco_init_demo_van( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_nationality nationality );
coco_sobject *
coco_init_tank( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_nationality nationality );
coco_sobject *
coco_init_firetruck( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_nationality nationality );
coco_sobject *
coco_init_infantry( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_gnum y, coco_gnum dx, coco_gnum dy, unsigned char raster_index, coco_nationality nationality );
coco_sobject *
coco_init_engineer( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_gnum y, coco_gnum dx, coco_gnum dy, unsigned char raster_index, coco_nationality nationality );
coco_sobject *
coco_init_munition( coco_sobject * inst, coco_sobject_cl * cl, coco_facing facing, coco_gnum x, coco_gnum y, coco_gnum dx, coco_gnum dy, coco_nationality nationality );
coco_sobject *
coco_init_bullet( coco_sobject * inst, coco_sobject_cl * cl, coco_facing facing, coco_gnum x, coco_gnum y, coco_gnum dx, coco_gnum dy, coco_nationality nationality, coco_u32 expiration_date );
coco_sobject *
coco_init_rifle_bullet( coco_sobject * inst, coco_sobject_cl * cl, coco_facing facing, coco_gnum x, coco_gnum y, coco_gnum dx, coco_gnum dy, coco_nationality nationality, coco_u32 expiration_date );
coco_sobject *
coco_init_tank_bullet( coco_sobject * inst, coco_sobject_cl * cl, coco_facing facing, coco_gnum x, coco_gnum y, coco_gnum dx, coco_gnum dy, coco_nationality nationality, coco_u32 expiration_date );
coco_sobject *
coco_init_bomb( coco_sobject * inst, coco_sobject_cl * cl, coco_facing facing, coco_gnum x, coco_gnum y, coco_gnum dx, coco_gnum dy, coco_nationality nationality );
coco_sobject *
coco_init_abstract_missile( coco_sobject * inst, coco_sobject_cl * cl, coco_facing facing, coco_gnum x, coco_gnum y, coco_gnum dx, coco_gnum dy, coco_nationality nationality, coco_angle angle );
coco_sobject *
coco_init_rocket( coco_sobject * inst, coco_sobject_cl * cl, coco_facing facing, coco_gnum x, coco_gnum y, coco_gnum dx, coco_gnum dy, coco_nationality nationality, coco_angle angle );
coco_sobject *
coco_init_missile( coco_sobject * inst, coco_sobject_cl * cl, coco_facing facing, coco_gnum x, coco_gnum y, coco_gnum dx, coco_gnum dy, coco_nationality nationality, coco_angle angle, int target_index );
coco_sobject *
coco_init_spark( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_gnum y, signed char spark_dx, signed char spark_dy );
coco_sobject *
coco_init_shrapnel( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_gnum y, coco_gnum dx, coco_gnum dy );
coco_sobject *
coco_init_metal_shrapnel( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_gnum y, coco_gnum dx, coco_gnum dy );
coco_sobject *
coco_init_big_metal_shrapnel( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_gnum y, coco_gnum dx, coco_gnum dy );
coco_sobject *
coco_init_med_metal_shrapnel( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_gnum y, coco_gnum dx, coco_gnum dy );
coco_sobject *
coco_init_small_metal_shrapnel( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_gnum y, coco_gnum dx, coco_gnum dy );
coco_sobject *
coco_init_explosion( coco_sobject * inst, coco_sobject_cl * cl, coco_facing facing, coco_gnum x, coco_gnum y, coco_gnum dx, coco_gnum dy, unsigned char raster_index );

/* Init-by-list function defaults */

coco_sobject *coco_cloud_automatic_init_by_list( coco_sobject *inst, coco_sobject_cl *cla, coco_lnum *arguments );
coco_sobject *coco_balloon_automatic_init_by_list( coco_sobject *inst, coco_sobject_cl *cla, coco_lnum *arguments );
coco_sobject *coco_balloon_line_automatic_init_by_list( coco_sobject *inst, coco_sobject_cl *cla, coco_lnum *arguments );
coco_sobject *coco_traffic_sign_automatic_init_by_list( coco_sobject *inst, coco_sobject_cl *cla, coco_lnum *arguments );
coco_sobject *coco_bunker_automatic_init_by_list( coco_sobject *inst, coco_sobject_cl *cla, coco_lnum *arguments );
coco_sobject *coco_pillbox_automatic_init_by_list( coco_sobject *inst, coco_sobject_cl *cla, coco_lnum *arguments );
coco_sobject *coco_landing_pad_automatic_init_by_list( coco_sobject *inst, coco_sobject_cl *cla, coco_lnum *arguments );
coco_sobject *coco_headquarters_automatic_init_by_list( coco_sobject *inst, coco_sobject_cl *cla, coco_lnum *arguments );
coco_sobject *coco_aa_gun_automatic_init_by_list( coco_sobject *inst, coco_sobject_cl *cla, coco_lnum *arguments );
coco_sobject *coco_at_gun_automatic_init_by_list( coco_sobject *inst, coco_sobject_cl *cla, coco_lnum *arguments );
coco_sobject *coco_gun_rubble_automatic_init_by_list( coco_sobject *inst, coco_sobject_cl *cla, coco_lnum *arguments );
coco_sobject *coco_ground_vehicle_automatic_init_by_list( coco_sobject *inst, coco_sobject_cl *cla, coco_lnum *arguments );
coco_sobject *coco_demo_van_automatic_init_by_list( coco_sobject *inst, coco_sobject_cl *cla, coco_lnum *arguments );
coco_sobject *coco_firetruck_automatic_init_by_list( coco_sobject *inst, coco_sobject_cl *cla, coco_lnum *arguments );
/* Accessors */

#define COCO_METAL_SHRAPNEL_COLOR_INDEX( clss ) ((clss)->metal_shrapnel.color_index)
#define COCO_SPARK_SPARK_DX( clss ) ((clss)->spark.spark_dx)
#define COCO_SPARK_SPARK_DY( clss ) ((clss)->spark.spark_dy)
#define COCO_MISSILE_FUEL( clss ) ((clss)->missile.fuel)
#define COCO_MISSILE_TARGET_INDEX( clss ) ((clss)->missile.target_index)
#define COCO_BOMB_HORIZ_TICKS( clss ) ((clss)->bomb.horiz_ticks)
#define COCO_BULLET_EXPIRATION_DATE( clss ) ((clss)->bullet.expiration_date)
#define COCO_INFANTRY_PARACHUTES( clss ) ((clss)->infantry.parachutes)
#define COCO_INFANTRY_PARACHUTE_HEIGHT( clss ) ((clss)->infantry.parachute_height)
#define COCO_FIRETRUCK_SMOKE_COUNTDOWN( clss ) ((clss)->firetruck.smoke_countdown)
#define COCO_GUN_RUBBLE_REINCARNATION_CLASS_INDEX( clss ) ((clss)->gun_rubble.reincarnation_class_index)
#define COCO_GUN_RUBBLE_POINTS_TILL_REINCARNATION( clss ) ((clss)->gun_rubble.points_till_reincarnation)
#define COCO_GUN_RUBBLE_REINCARNATION_NATIONALITY( clss ) ((clss)->gun_rubble.reincarnation_nationality)
#define COCO_GUN_COOLDOWN( clss ) ((clss)->gun.cooldown)
#define COCO_HEADQUARTERS_SMOKE_COUNTDOWN( clss ) ((clss)->headquarters.smoke_countdown)
#define COCO_HEADQUARTERS_PILLBOX_INDEX( clss ) ((clss)->headquarters.pillbox_index)
#define COCO_PILLBOX_HEADQUARTERS_INDEX( clss ) ((clss)->pillbox.headquarters_index)
#define COCO_BUNKER_DEFENDERS( clss ) ((clss)->bunker.defenders)
#define COCO_HUT_LINE_INDEX( clss ) ((clss)->hut.line_index)
#define COCO_HUT_POINTS_TILL_REINCARNATION( clss ) ((clss)->hut.points_till_reincarnation)
#define COCO_BALLOON_LINE_HUT_INDEX( clss ) ((clss)->balloon_line.hut_index)
#define COCO_BALLOON_LINE_BALLOON_INDEX( clss ) ((clss)->balloon_line.balloon_index)
#define COCO_BALLOON_LINE_INDEX( clss ) ((clss)->balloon.line_index)
#define COCO_BALLOON_BALLOON_TIME_PHASE( clss ) ((clss)->balloon.balloon_time_phase)
#define COCO_COPTER_BULLETS( clss ) ((clss)->copter.bullets)
#define COCO_COPTER_BOMBS( clss ) ((clss)->copter.bombs)
#define COCO_COPTER_MISSILES( clss ) ((clss)->copter.missiles)
#define COCO_COPTER_ROCKETS( clss ) ((clss)->copter.rockets)
#define COCO_COPTER_PARATROOPERS( clss ) ((clss)->copter.paratroopers)
#define COCO_COPTER_ENGITROOPERS( clss ) ((clss)->copter.engitroopers)
#define COCO_COPTER_FUEL( clss ) ((clss)->copter.fuel)
#define COCO_COPTER_MISSILE_COOLDOWN( clss ) ((clss)->copter.missile_cooldown)
#define COCO_COPTER_MISSILE_COOLDOWN_TIME( clss ) ((clss)->copter.missile_cooldown_time)
#define COCO_COPTER_NEXT_MISSILEBEAT_ON_PAD( clss ) ((clss)->copter.next_missilebeat_on_pad)
#define COCO_COPTER_LAST_BOMB_DROP_TIME( clss ) ((clss)->copter.last_bomb_drop_time)
#define COCO_COPTER_LAST_ROCKET_LAUNCH_TIME( clss ) ((clss)->copter.last_rocket_launch_time)
#define COCO_COPTER_LANDED_P( clss ) ((clss)->copter.landed_p)
#define COCO_COPTER_LANDED_ON_PAD_P( clss ) ((clss)->copter.landed_on_pad_p)
#define COCO_SNDOBJECT_SOUND( clss ) ((clss)->sndobject.sound)
#define COCO_SOBJECT_CL( clss ) ((clss)->every.cl)
#define COCO_SOBJECT_IN_USE( clss ) ((clss)->every.in_use)
#define COCO_SOBJECT_CAMERA_INDEX( clss ) ((clss)->every.camera_index)
#define COCO_SOBJECT_COMP_INDEX( clss ) ((clss)->every.comp_index)
#define COCO_SOBJECT_FACING( clss ) ((clss)->every.facing)
#define COCO_SOBJECT_NEW_FLAG( clss ) ((clss)->every.new_flag)
#define COCO_SOBJECT_INDEX( clss ) ((clss)->every.index)
#define COCO_SOBJECT_X( clss ) ((clss)->every.x)
#define COCO_SOBJECT_Y( clss ) ((clss)->every.y)
#define COCO_SOBJECT_W( clss ) ((clss)->every.w)
#define COCO_SOBJECT_H( clss ) ((clss)->every.h)
#define COCO_SOBJECT_DX( clss ) ((clss)->every.dx)
#define COCO_SOBJECT_DY( clss ) ((clss)->every.dy)
#define COCO_SOBJECT_DX_PUBLIC( clss ) ((clss)->every.dx_public)
#define COCO_SOBJECT_DY_PUBLIC( clss ) ((clss)->every.dy_public)
#define COCO_SOBJECT_LOCKED_ON_TIME( clss ) ((clss)->every.locked_on_time)
#define COCO_SOBJECT_RASTER_INDEX( clss ) ((clss)->every.raster_index)
#define COCO_SOBJECT_NATIONALITY( clss ) ((clss)->every.nationality)
#define COCO_SOBJECT_HEALTH( clss ) ((clss)->every.health)
#define COCO_SOBJECT_ANGLE( clss ) ((clss)->every.angle)
#define COCO_SOBJECT_SCRATCH_BLIP( clss ) ((clss)->every.scratch_blip)
#define COCO_SOBJECT_CL_INDEX( clss ) ((clss)->every.cl->cl_index)
#define COCO_SOBJECT_INIT_BY_LIST_COUNT( clss ) ((clss)->every.cl->init_by_list_count)
#define COCO_SOBJECT_INIT_BY_LIST_FN( clss ) ((clss)->every.cl->init_by_list_fn)
#define COCO_SOBJECT_NAME( clss ) ((clss)->every.cl->name)
#define COCO_SOBJECT_SUPER( clss ) ((clss)->every.cl->super)
#define COCO_SOBJECT_PHASE( clss ) ((clss)->every.cl->phase)
#define COCO_SOBJECT_ROTATE_P( clss ) ((clss)->every.cl->rotate_p)
#define COCO_SOBJECT_DX_MAX( clss ) ((clss)->every.cl->dx_max)
#define COCO_SOBJECT_DY_MAX( clss ) ((clss)->every.cl->dy_max)
#define COCO_SOBJECT_X_FORCE( clss ) ((clss)->every.cl->x_force)
#define COCO_SOBJECT_Y_FORCE( clss ) ((clss)->every.cl->y_force)
#define COCO_SOBJECT_HEALTH_MAX( clss ) ((clss)->every.cl->health_max)
#define COCO_SOBJECT_DAMAGE( clss ) ((clss)->every.cl->damage)
#define COCO_SOBJECT_CLIP_TO_BOARD( clss ) ((clss)->every.cl->clip_to_board)
#define COCO_SOBJECT_DEFAULT_BLIP_COLOR_INDEX( clss ) ((clss)->every.cl->default_blip_color_index)
#define COCO_SOBJECT_CANONICAL_W( clss ) ((clss)->every.cl->canonical_w)
#define COCO_SOBJECT_CANONICAL_H( clss ) ((clss)->every.cl->canonical_h)
#define COCO_SOBJECT_Z( clss ) ((clss)->every.cl->z)
#define COCO_SOBJECT_RASTER_COUNT( clss ) ((clss)->every.cl->raster_count)
#define COCO_SOBJECT_RASTERS( clss ) ((clss)->every.cl->rasters)
#define COCO_SOBJECT_COBJECT_Y_FN( clss ) ((clss)->every.cl->cobject_y_fn)
#define COCO_SOBJECT_COBJECT_RASTER_INDEX_FN( clss ) ((clss)->every.cl->cobject_raster_index_fn)
#define COCO_SOBJECT_COBJECT_GUESS_NATIONALITY_FN( clss ) ((clss)->every.cl->cobject_guess_nationality_fn)
#define COCO_SOBJECT_BULLET_CLASS( clss ) ((clss)->every.cl->bullet_class)
#define COCO_SOBJECT_COBJECT_NTOH( clss ) ((clss)->every.cl->cobject_ntoh)
#define COCO_SOBJECT_COBJECT_DRAW( clss ) ((clss)->every.cl->cobject_draw)
#define COCO_SOBJECT_SHRAPNEL_COUNT( clss ) ((clss)->every.cl->shrapnel_count)
#define COCO_SOBJECT_REFUND( clss ) ((clss)->every.cl->refund)
#define COCO_SOBJECT_COST( clss ) ((clss)->every.cl->cost)
#define COCO_SOBJECT_COLLISION_PRIORITY( clss ) ((clss)->every.cl->collision_priority)
#define COCO_SOBJECT_COLLISION_RECTANGLE( clss ) ((clss)->every.cl->collision_rectangle)
#define COCO_SOBJECT_ROTOR_RADIUS( clss ) ((clss)->every.cl->rotor_radius)
#define COCO_SOBJECT_MAX_BULLETS( clss ) ((clss)->every.cl->max_bullets)
#define COCO_SOBJECT_MAX_BOMBS( clss ) ((clss)->every.cl->max_bombs)
#define COCO_SOBJECT_MAX_ROCKETS( clss ) ((clss)->every.cl->max_rockets)
#define COCO_SOBJECT_MAX_MISSILES( clss ) ((clss)->every.cl->max_missiles)
#define COCO_SOBJECT_MAX_INTERNAL_INFANTRY( clss ) ((clss)->every.cl->max_internal_infantry)
#define COCO_SOBJECT_MAX_FUEL( clss ) ((clss)->every.cl->max_fuel)
#define COCO_SOBJECT_ROTOR_POINTS( clss ) ((clss)->every.cl->rotor_points)
#define COCO_SOBJECT_WEAPON_HARDPOINTS( clss ) ((clss)->every.cl->weapon_hardpoints)
#define COCO_SOBJECT_CENTER_OF_MASS( clss ) ((clss)->every.cl->center_of_mass)
#define COCO_SOBJECT_GUN_BARREL_LENGTH( clss ) ((clss)->every.cl->gun_barrel_length)
#define COCO_SOBJECT_INFANTRY_TAKE_BUILDINGS_P( clss ) ((clss)->every.cl->infantry_take_buildings_p)
#define COCO_SOBJECT_SPARK_COUNT( clss ) ((clss)->every.cl->spark_count)
#define COCO_SOBJECT_THICKNESS( clss ) ((clss)->every.cl->thickness)
#define COCO_SOBJECT_MUZZLE_VELOCITY( clss ) ((clss)->every.cl->muzzle_velocity)
#define COCO_SOBJECT_LIFESPAN( clss ) ((clss)->every.cl->lifespan)
#define COCO_SOBJECT_ROOF_TAB( clss ) ((clss)->every.cl->roof_tab)
#define COCO_SOBJECT_AIRBURST_SOUND_TYPE( clss ) ((clss)->every.cl->airburst_sound_type)
#define COCO_SOBJECT_FIRE_BULLET_SOUND_TYPE( clss ) ((clss)->every.cl->fire_bullet_sound_type)
#define COCO_SOBJECT_TARGETABLE_CLASSES_COUNT( clss ) ((clss)->every.cl->targetable_classes_count)
#define COCO_SOBJECT_TARGETABLE_CLASSES( clss ) ((clss)->every.cl->targetable_classes)

