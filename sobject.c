/* This file has been automatically generated.  Do not edit it directly. */

#include <stdlib.h> /* for rand() */
#include "coco.h"
#include "rasters.h"

/* Class structures themselves */

coco_sobject_cl coco_sobject_classes[] = {
  { /* coco_sobject */
    0,                                  /* cl_index */
    4,                                  /* init_by_list_count */
    NULL,                               /* init_by_list_fn */
    "sobject",                          /* name */
    0,                                  /* super */
    COCO_SOLID,                         /* phase */
    COCO_FALSE,                         /* rotate_p */
    0,                                  /* dx_max */
    0,                                  /* dy_max */
    0,                                  /* x_force */
    0,                                  /* y_force */
    COCO_NIGH_INVULNERABLE,             /* health_max */
    COCO_NIGH_INVULNERABLE,             /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY, COCO_BLIP_EMY, COCO_BLIP_EMY ), /* default_blip_color_index */
    1 /* placeholder */,                /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    0,                                  /* z */
    0,                                  /* raster_count */
    NULL,                               /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_cobject_whimpy_guess_nationality, /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_cobject_rasteroid_draw,        /* cobject_draw */
    0,                                  /* shrapnel_count */
    0,                                  /* refund */
    0,                                  /* cost */
    100,                                /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    coco_every_sobject_drop_paratrooper,
    coco_every_sobject_launch_missile,
    coco_every_sobject_fire_bullet,
    0,
    coco_every_sobject_toggle_facing,
    coco_every_sobject_raster_flip_index,
    coco_ignore_bunker,
    0,
    0,
    coco_every_sobject_is_visible,
    coco_every_sobject_blip_color_index,
    coco_trooper_always_target_me,
    sobject_ignore_neutral_buiding,
    coco_rasteroid_to_cobject,
    coco_every_sobject_ground_collide,
    coco_unarmored_effective_damage,
    coco_every_sobject_friendly_collision,
    coco_every_sobject_hostile_collision,
    0,
    coco_every_sobject_delete,
    coco_every_sobject_kaboom,
    coco_every_sobject_update_state,
    coco_every_sobject_unlink,
  },
  { /* coco_sndobject */
    1,                                  /* cl_index */
    3,                                  /* init_by_list_count */
    NULL,                               /* init_by_list_fn */
    "sndobject",                        /* name */
    (coco_sobject_classes + COCO_SOBJECT_CLASS_INDEX), /* super */
    COCO_VAPOR,                         /* phase */
    COCO_FALSE,                         /* rotate_p */
    0,                                  /* dx_max */
    0,                                  /* dy_max */
    0,                                  /* x_force */
    0,                                  /* y_force */
    COCO_NIGH_INVULNERABLE,             /* health_max */
    COCO_NIGH_INVULNERABLE,             /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY, COCO_BLIP_EMY, COCO_BLIP_EMY ), /* default_blip_color_index */
    0,                                  /* canonical_w */
    0,                                  /* canonical_h */
    0,                                  /* z */
    0,                                  /* raster_count */
    NULL,                               /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_cobject_whimpy_guess_nationality, /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_cobject_sound_draw,            /* cobject_draw */
    0,                                  /* shrapnel_count */
    0,                                  /* refund */
    0,                                  /* cost */
    100,                                /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_sndobject_is_visible,
    0,
    0,
    0,
    coco_sndobject_to_cobject,
    coco_ignore_ground_collide,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_sndobject_update_state,
    0,
  },
  { /* coco_camera */
    2,                                  /* cl_index */
    5,                                  /* init_by_list_count */
    NULL,                               /* init_by_list_fn */
    "camera",                           /* name */
    (coco_sobject_classes + COCO_SOBJECT_CLASS_INDEX), /* super */
    COCO_SOLID,                         /* phase */
    COCO_FALSE,                         /* rotate_p */
    0,                                  /* dx_max */
    0,                                  /* dy_max */
    0,                                  /* x_force */
    0,                                  /* y_force */
    COCO_NIGH_INVULNERABLE,             /* health_max */
    COCO_NIGH_INVULNERABLE,             /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY_PLAYER, COCO_BLIP_EMY_PLAYER, COCO_BLIP_EMY_PLAYER ), /* default_blip_color_index */
    1 /* placeholder */,                /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    0,                                  /* z */
    0,                                  /* raster_count */
    NULL,                               /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_cobject_whimpy_guess_nationality, /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_cobject_rasteroid_draw,        /* cobject_draw */
    0,                                  /* shrapnel_count */
    0,                                  /* refund */
    0,                                  /* cost */
    50,                                 /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_every_camera_velocity_offset,
    0,
    coco_camera_blip_color_index,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_camera_update_state,
    0,
  },
  { /* coco_inviso_camera */
    3,                                  /* cl_index */
    3,                                  /* init_by_list_count */
    NULL,                               /* init_by_list_fn */
    "inviso-camera",                    /* name */
    (coco_sobject_classes + COCO_CAMERA_CLASS_INDEX), /* super */
    COCO_VAPOR,                         /* phase */
    COCO_FALSE,                         /* rotate_p */
    0,                                  /* dx_max */
    0,                                  /* dy_max */
    0,                                  /* x_force */
    0,                                  /* y_force */
    COCO_NIGH_INVULNERABLE,             /* health_max */
    COCO_NIGH_INVULNERABLE,             /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_BLIP_TRIPLET_TRANSPARENT,      /* default_blip_color_index */
    0,                                  /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    0,                                  /* z */
    0,                                  /* raster_count */
    NULL,                               /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_cobject_whimpy_guess_nationality, /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_cobject_inviso_draw,           /* cobject_draw */
    0,                                  /* shrapnel_count */
    0,                                  /* refund */
    0,                                  /* cost */
    50,                                 /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    coco_inviso_camera_drop_paratrooper,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_trooper_never_target_me,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_inviso_camera_update_state,
    0,
  },
  { /* coco_copter */
    4,                                  /* cl_index */
    5,                                  /* init_by_list_count */
    NULL,                               /* init_by_list_fn */
    "copter",                           /* name */
    (coco_sobject_classes + COCO_CAMERA_CLASS_INDEX), /* super */
    COCO_SOLID,                         /* phase */
    COCO_TRUE,                          /* rotate_p */
    COCO_COPTER_DX_MAX,                 /* dx_max */
    COCO_COPTER_DY_MAX,                 /* dy_max */
    4,                                  /* x_force */
    0,                                  /* y_force */
    4,                                  /* health_max */
    COCO_COPTER_DAMAGE,                 /* damage */
    COCO_TRUE,                          /* clip_to_board */
    COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY_PLAYER, COCO_BLIP_EMY_PLAYER, COCO_BLIP_EMY_PLAYER ), /* default_blip_color_index */
    1 /* placeholder */,                /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    0,                                  /* z */
    COCO_COPTER_RASTER_COUNT,           /* raster_count */
    NULL,                               /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_copter_guess_nationality,      /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_copter_draw,                   /* cobject_draw */
    8,                                  /* shrapnel_count */
    0,                                  /* refund */
    0,                                  /* cost */
    50,                                 /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    10,                                 /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_BIG_AIRBURST,            /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    coco_copter_drop_paratrooper,
    coco_copter_launch_missile,
    coco_copter_fire_bullet,
    coco_copter_fire_bullet_coords,
    coco_copter_toggle_facing,
    coco_copter_raster_flip_index,
    0,
    0,
    coco_copter_velocity_offset,
    0,
    coco_copter_blip_color_index,
    0,
    0,
    coco_y_angle_to_cobject,
    coco_copter_ground_collide,
    coco_flesh_damageable_effective_damage,
    coco_copter_friendly_collision,
    coco_copter_hostile_collision,
    0,
    coco_copter_delete,
    coco_copter_kaboom,
    coco_copter_update_state,
    0,
  },
  { /* coco_cobra */
    5,                                  /* cl_index */
    5,                                  /* init_by_list_count */
    NULL,                               /* init_by_list_fn */
    "cobra",                            /* name */
    (coco_sobject_classes + COCO_COPTER_CLASS_INDEX), /* super */
    COCO_SOLID,                         /* phase */
    COCO_TRUE,                          /* rotate_p */
    COCO_COPTER_DX_MAX,                 /* dx_max */
    COCO_COPTER_DY_MAX,                 /* dy_max */
    4,                                  /* x_force */
    0,                                  /* y_force */
    4,                                  /* health_max */
    COCO_COPTER_DAMAGE,                 /* damage */
    COCO_TRUE,                          /* clip_to_board */
    COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY_PLAYER, COCO_BLIP_EMY_PLAYER, COCO_BLIP_EMY_PLAYER ), /* default_blip_color_index */
    100,                                /* canonical_w */
    30,                                 /* canonical_h */
    0,                                  /* z */
    COCO_COPTER_RASTER_COUNT,           /* raster_count */
    coco_cobra_rasters,                 /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_copter_guess_nationality,      /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_copter_draw,                   /* cobject_draw */
    8,                                  /* shrapnel_count */
    4,                                  /* refund */
    20,                                 /* cost */
    50,                                 /* collision_priority */
    COCO_MAKE_MINI_RECTANGLE( 2, 0, 62, 29 ), /* collision_rectangle */
    56,                                 /* rotor_radius */
    100,                                /* max_bullets */
    12,                                 /* max_bombs */
    10,                                 /* max_rockets */
    2,                                  /* max_missiles */
    5,                                  /* max_internal_infantry */
    200*COCO_IDEAL_FRAMES_PER_SECOND,   /* max_fuel */
    coco_cobra_rotor_points,            /* rotor_points */
    coco_cobra_weapon_hardpoints,       /* weapon_hardpoints */
    coco_cobra_center_of_mass,          /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_BIG_AIRBURST,            /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
  },
  { /* coco_bubble_copter */
    6,                                  /* cl_index */
    5,                                  /* init_by_list_count */
    NULL,                               /* init_by_list_fn */
    "bubble-copter",                    /* name */
    (coco_sobject_classes + COCO_COPTER_CLASS_INDEX), /* super */
    COCO_SOLID,                         /* phase */
    COCO_TRUE,                          /* rotate_p */
    COCO_COPTER_DX_MAX-2,               /* dx_max */
    COCO_COPTER_DY_MAX-1,               /* dy_max */
    2,                                  /* x_force */
    0,                                  /* y_force */
    3,                                  /* health_max */
    COCO_COPTER_DAMAGE,                 /* damage */
    COCO_TRUE,                          /* clip_to_board */
    COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY_PLAYER, COCO_BLIP_EMY_PLAYER, COCO_BLIP_EMY_PLAYER ), /* default_blip_color_index */
    83,                                 /* canonical_w */
    30,                                 /* canonical_h */
    0,                                  /* z */
    COCO_COPTER_RASTER_COUNT,           /* raster_count */
    coco_bubble_copter_rasters,         /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_copter_guess_nationality,      /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_copter_draw,                   /* cobject_draw */
    8,                                  /* shrapnel_count */
    1,                                  /* refund */
    5,                                  /* cost */
    50,                                 /* collision_priority */
    COCO_MAKE_MINI_RECTANGLE( 0, 0, 53, 29 ), /* collision_rectangle */
    50,                                 /* rotor_radius */
    80,                                 /* max_bullets */
    8,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    4,                                  /* max_internal_infantry */
    160*COCO_IDEAL_FRAMES_PER_SECOND,   /* max_fuel */
    coco_bubble_copter_rotor_points,    /* rotor_points */
    coco_bubble_copter_weapon_hardpoints, /* weapon_hardpoints */
    coco_bubble_copter_center_of_mass,  /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_BIG_AIRBURST,            /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
  },
  { /* coco_smoke */
    7,                                  /* cl_index */
    3,                                  /* init_by_list_count */
    NULL,                               /* init_by_list_fn */
    "smoke",                            /* name */
    (coco_sobject_classes + COCO_SOBJECT_CLASS_INDEX), /* super */
    COCO_VAPOR,                         /* phase */
    COCO_FALSE,                         /* rotate_p */
    0,                                  /* dx_max */
    0,                                  /* dy_max */
    0,                                  /* x_force */
    0,                                  /* y_force */
    COCO_NIGH_INVULNERABLE,             /* health_max */
    COCO_NIGH_INVULNERABLE,             /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_BLIP_TRIPLET_TRANSPARENT,      /* default_blip_color_index */
    COCO_SMOKE_BIG_RASTER_W,            /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    2,                                  /* z */
    COCO_SMOKE_RASTER_COUNT,            /* raster_count */
    coco_smoke_rasters,                 /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_cobject_whimpy_guess_nationality, /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_cobject_rasteroid_draw,        /* cobject_draw */
    0,                                  /* shrapnel_count */
    0,                                  /* refund */
    0,                                  /* cost */
    100,                                /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_smoke_update_state,
    0,
  },
  { /* coco_cloud */
    8,                                  /* cl_index */
    2,                                  /* init_by_list_count */
    coco_cloud_automatic_init_by_list,  /* init_by_list_fn */
    "cloud",                            /* name */
    (coco_sobject_classes + COCO_SOBJECT_CLASS_INDEX), /* super */
    COCO_VAPOR,                         /* phase */
    COCO_FALSE,                         /* rotate_p */
    0,                                  /* dx_max */
    0,                                  /* dy_max */
    0,                                  /* x_force */
    0,                                  /* y_force */
    COCO_NIGH_INVULNERABLE,             /* health_max */
    COCO_NIGH_INVULNERABLE,             /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_BLIP_TRIPLET_TRANSPARENT,      /* default_blip_color_index */
    COCO_CLOUD_RASTER_W,                /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    1,                                  /* z */
    COCO_CLOUD_RASTER_COUNT,            /* raster_count */
    coco_cloud_rasters,                 /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_cobject_whimpy_guess_nationality, /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_cobject_rasteroid_draw,        /* cobject_draw */
    0,                                  /* shrapnel_count */
    0,                                  /* refund */
    0,                                  /* cost */
    100,                                /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
  },
  { /* coco_balloon_part */
    9,                                  /* cl_index */
    4,                                  /* init_by_list_count */
    NULL,                               /* init_by_list_fn */
    "balloon-part",                     /* name */
    (coco_sobject_classes + COCO_SOBJECT_CLASS_INDEX), /* super */
    COCO_SOLID,                         /* phase */
    COCO_FALSE,                         /* rotate_p */
    0,                                  /* dx_max */
    0,                                  /* dy_max */
    0,                                  /* x_force */
    0,                                  /* y_force */
    COCO_NIGH_INVULNERABLE,             /* health_max */
    COCO_NIGH_INVULNERABLE,             /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY, COCO_BLIP_EMY, COCO_BLIP_EMY ), /* default_blip_color_index */
    1 /* placeholder */,                /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    0,                                  /* z */
    0,                                  /* raster_count */
    NULL,                               /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_cobject_whimpy_guess_nationality, /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_cobject_rasteroid_draw,        /* cobject_draw */
    0,                                  /* shrapnel_count */
    0,                                  /* refund */
    0,                                  /* cost */
    100,                                /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_trooper_never_target_me,
    0,
    0,
    0,
    0,
    0,
    coco_balloon_part_hostile_collision,
    0,
    0,
    0,
    0,
    0,
  },
  { /* coco_balloon */
    10,                                 /* cl_index */
    4,                                  /* init_by_list_count */
    coco_balloon_automatic_init_by_list, /* init_by_list_fn */
    "balloon",                          /* name */
    (coco_sobject_classes + COCO_BALLOON_PART_CLASS_INDEX), /* super */
    COCO_SOLID,                         /* phase */
    COCO_FALSE,                         /* rotate_p */
    0,                                  /* dx_max */
    0,                                  /* dy_max */
    0,                                  /* x_force */
    0,                                  /* y_force */
    4 * COCO_BULLET_DAMAGE,             /* health_max */
    7,                                  /* damage */
    COCO_TRUE,                          /* clip_to_board */
    COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY_BALLOON, COCO_BLIP_EMY_BALLOON, COCO_BLIP_EMY_BALLOON ), /* default_blip_color_index */
    COCO_BALLOON_W_RASTER_W,            /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    0,                                  /* z */
    COCO_BALLOON_RASTER_COUNT,          /* raster_count */
    coco_balloon_rasters,               /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_cobject_whimpy_guess_nationality, /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_cobject_rasteroid_draw,        /* cobject_draw */
    0,                                  /* shrapnel_count */
    0,                                  /* refund */
    0,                                  /* cost */
    150,                                /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_airbursting_kaboom,
    coco_balloon_update_state,
    coco_balloon_unlink,
  },
  { /* coco_balloon_line */
    11,                                 /* cl_index */
    6,                                  /* init_by_list_count */
    coco_balloon_line_automatic_init_by_list, /* init_by_list_fn */
    "balloon-line",                     /* name */
    (coco_sobject_classes + COCO_BALLOON_PART_CLASS_INDEX), /* super */
    COCO_EXPLOSIVE,                     /* phase */
    COCO_FALSE,                         /* rotate_p */
    0,                                  /* dx_max */
    0,                                  /* dy_max */
    0,                                  /* x_force */
    0,                                  /* y_force */
    1,                                  /* health_max */
    3,                                  /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_BLIP_TRIPLET_TRANSPARENT,      /* default_blip_color_index */
    1,                                  /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    0,                                  /* z */
    0,                                  /* raster_count */
    NULL,                               /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_cobject_whimpy_guess_nationality, /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_balloon_line_draw,             /* cobject_draw */
    0,                                  /* shrapnel_count */
    0,                                  /* refund */
    0,                                  /* cost */
    100,                                /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_balloon_line_to_cobject,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_balloon_line_update_state,
    coco_balloon_line_unlink,
  },
  { /* coco_building */
    12,                                 /* cl_index */
    2,                                  /* init_by_list_count */
    NULL,                               /* init_by_list_fn */
    "building",                         /* name */
    (coco_sobject_classes + COCO_SOBJECT_CLASS_INDEX), /* super */
    COCO_SOLID,                         /* phase */
    COCO_FALSE,                         /* rotate_p */
    0,                                  /* dx_max */
    0,                                  /* dy_max */
    0,                                  /* x_force */
    0,                                  /* y_force */
    COCO_NIGH_INVULNERABLE,             /* health_max */
    COCO_NIGH_INVULNERABLE,             /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY_BUILDING, COCO_BLIP_EMY_BUILDING, COCO_BLIP_EMY_BUILDING ), /* default_blip_color_index */
    1 /* placeholder */,                /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    -1,                                 /* z */
    0,                                  /* raster_count */
    NULL,                               /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_simple_raster_index,   /* cobject_raster_index_fn */
    coco_cobject_whimpy_guess_nationality, /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_cobject_rasteroid_draw,        /* cobject_draw */
    4,                                  /* shrapnel_count */
    0,                                  /* refund */
    0,                                  /* cost */
    200,                                /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_trooper_never_target_me,
    0,
    0,
    0,
    0,
    0,
    coco_building_hostile_collision,
    0,
    0,
    coco_airbursting_kaboom,
    0,
    0,
  },
  { /* coco_traffic_sign */
    13,                                 /* cl_index */
    2,                                  /* init_by_list_count */
    coco_traffic_sign_automatic_init_by_list, /* init_by_list_fn */
    "traffic-sign",                     /* name */
    (coco_sobject_classes + COCO_BUILDING_CLASS_INDEX), /* super */
    COCO_VAPOR,                         /* phase */
    COCO_FALSE,                         /* rotate_p */
    0,                                  /* dx_max */
    0,                                  /* dy_max */
    0,                                  /* x_force */
    0,                                  /* y_force */
    COCO_NIGH_INVULNERABLE,             /* health_max */
    COCO_NIGH_INVULNERABLE,             /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_BLIP_TRIPLET_TRANSPARENT,      /* default_blip_color_index */
    1 /* placeholder */,                /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    -1,                                 /* z */
    COCO_TRAFFIC_SIGN_RASTER_COUNT,     /* raster_count */
    coco_traffic_sign_rasters,          /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_neutral_guess_nationality,     /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_cobject_rasteroid_draw,        /* cobject_draw */
    4,                                  /* shrapnel_count */
    0,                                  /* refund */
    0,                                  /* cost */
    200,                                /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
  },
  { /* coco_hut */
    14,                                 /* cl_index */
    2,                                  /* init_by_list_count */
    coco_hut_init_by_list,              /* init_by_list_fn */
    "hut",                              /* name */
    (coco_sobject_classes + COCO_BUILDING_CLASS_INDEX), /* super */
    COCO_SOLID,                         /* phase */
    COCO_FALSE,                         /* rotate_p */
    0,                                  /* dx_max */
    0,                                  /* dy_max */
    0,                                  /* x_force */
    0,                                  /* y_force */
    COCO_MISSILE_DAMAGE<<1,             /* health_max */
    COCO_NIGH_INVULNERABLE,             /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY_BUILDING, COCO_BLIP_EMY_BUILDING, COCO_BLIP_EMY_BUILDING ), /* default_blip_color_index */
    COCO_HUT_NEUTRAL_RASTER_W,          /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    -1,                                 /* z */
    COCO_HUT_RASTER_COUNT,              /* raster_count */
    coco_hut_rasters,                   /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_hut_guess_nationality,         /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_cobject_rasteroid_draw,        /* cobject_draw */
    4,                                  /* shrapnel_count */
    4,                                  /* refund */
    0,                                  /* cost */
    200,                                /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_hut_update_state,
    coco_hut_unlink,
  },
  { /* coco_bunker */
    15,                                 /* cl_index */
    3,                                  /* init_by_list_count */
    coco_bunker_automatic_init_by_list, /* init_by_list_fn */
    "bunker",                           /* name */
    (coco_sobject_classes + COCO_BUILDING_CLASS_INDEX), /* super */
    COCO_SOLID,                         /* phase */
    COCO_FALSE,                         /* rotate_p */
    0,                                  /* dx_max */
    0,                                  /* dy_max */
    0,                                  /* x_force */
    0,                                  /* y_force */
    COCO_NIGH_INVULNERABLE,             /* health_max */
    COCO_NIGH_INVULNERABLE,             /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY_BUILDING, COCO_BLIP_EMY_BUILDING, COCO_BLIP_EMY_BUILDING ), /* default_blip_color_index */
    COCO_BUNKER_GREEN_RASTER_W,         /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    -1,                                 /* z */
    COCO_BUNKER_RASTER_COUNT,           /* raster_count */
    coco_bunker_rasters,                /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_2raster_guess_nationality,     /* cobject_guess_nationality_fn */
    COCO_RIFLE_BULLET_CLASS,            /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_cobject_rasteroid_draw,        /* cobject_draw */
    4,                                  /* shrapnel_count */
    7,                                  /* refund */
    0,                                  /* cost */
    12,                                 /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    5,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    50,                                 /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    coco_bunker_fire_bullet_coords,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_trooper_bunker_target_p,
    0,
    0,
    0,
    0,
    0,
    coco_bunker_hostile_collision,
    0,
    0,
    0,
    coco_bunker_update_state,
    0,
  },
  { /* coco_pillbox */
    16,                                 /* cl_index */
    4,                                  /* init_by_list_count */
    coco_pillbox_automatic_init_by_list, /* init_by_list_fn */
    "pillbox",                          /* name */
    (coco_sobject_classes + COCO_BUNKER_CLASS_INDEX), /* super */
    COCO_SOLID,                         /* phase */
    COCO_FALSE,                         /* rotate_p */
    0,                                  /* dx_max */
    0,                                  /* dy_max */
    0,                                  /* x_force */
    0,                                  /* y_force */
    COCO_NIGH_INVULNERABLE,             /* health_max */
    COCO_NIGH_INVULNERABLE,             /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY_BUILDING, COCO_BLIP_EMY_BUILDING, COCO_BLIP_EMY_BUILDING ), /* default_blip_color_index */
    COCO_PILLBOX_RASTER_W,              /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    -1,                                 /* z */
    COCO_PILLBOX_RASTER_COUNT,          /* raster_count */
    coco_pillbox_rasters,               /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_2raster_guess_nationality,     /* cobject_guess_nationality_fn */
    COCO_RIFLE_BULLET_CLASS,            /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_cobject_rasteroid_draw,        /* cobject_draw */
    4,                                  /* shrapnel_count */
    7,                                  /* refund */
    0,                                  /* cost */
    12,                                 /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    2,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_pillbox_unlink,
  },
  { /* coco_landing_pad */
    17,                                 /* cl_index */
    1,                                  /* init_by_list_count */
    coco_landing_pad_automatic_init_by_list, /* init_by_list_fn */
    "landing-pad",                      /* name */
    (coco_sobject_classes + COCO_BUILDING_CLASS_INDEX), /* super */
    COCO_SOLID,                         /* phase */
    COCO_FALSE,                         /* rotate_p */
    0,                                  /* dx_max */
    0,                                  /* dy_max */
    0,                                  /* x_force */
    0,                                  /* y_force */
    COCO_NIGH_INVULNERABLE,             /* health_max */
    COCO_NIGH_INVULNERABLE,             /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY_BUILDING, COCO_BLIP_EMY_BUILDING, COCO_BLIP_NEUTRAL_BUILDING ), /* default_blip_color_index */
    COCO_LANDING_PAD_RASTER_W,          /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    -1,                                 /* z */
    COCO_LANDING_PAD_RASTER_COUNT,      /* raster_count */
    coco_landing_pad_rasters,           /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_simple_raster_index,   /* cobject_raster_index_fn */
    coco_neutral_guess_nationality,     /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_cobject_rasteroid_draw,        /* cobject_draw */
    4,                                  /* shrapnel_count */
    0,                                  /* refund */
    0,                                  /* cost */
    200,                                /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
  },
  { /* coco_headquarters */
    18,                                 /* cl_index */
    3,                                  /* init_by_list_count */
    coco_headquarters_automatic_init_by_list, /* init_by_list_fn */
    "headquarters",                     /* name */
    (coco_sobject_classes + COCO_BUILDING_CLASS_INDEX), /* super */
    COCO_SOLID,                         /* phase */
    COCO_FALSE,                         /* rotate_p */
    0,                                  /* dx_max */
    0,                                  /* dy_max */
    0,                                  /* x_force */
    0,                                  /* y_force */
    COCO_NIGH_INVULNERABLE,             /* health_max */
    COCO_NIGH_INVULNERABLE,             /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY_BUILDING, COCO_BLIP_EMY_BUILDING, COCO_BLIP_EMY_BUILDING ), /* default_blip_color_index */
    COCO_HEADQUARTERS_GREEN_RASTER_W,   /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    -1,                                 /* z */
    COCO_HEADQUARTERS_RASTER_COUNT,     /* raster_count */
    coco_headquarters_rasters,          /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_2raster_guess_nationality,     /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_cobject_rasteroid_draw,        /* cobject_draw */
    25,                                 /* shrapnel_count */
    255,                                /* refund */
    0,                                  /* cost */
    200,                                /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_BIG_AIRBURST,            /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    coco_headquarters_launch_missile,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_headquarters_update_state,
    coco_headquarters_unlink,
  },
  { /* coco_gun */
    19,                                 /* cl_index */
    2,                                  /* init_by_list_count */
    NULL,                               /* init_by_list_fn */
    "gun",                              /* name */
    (coco_sobject_classes + COCO_BUILDING_CLASS_INDEX), /* super */
    COCO_SOLID,                         /* phase */
    COCO_FALSE,                         /* rotate_p */
    0,                                  /* dx_max */
    0,                                  /* dy_max */
    0,                                  /* x_force */
    0,                                  /* y_force */
    23,                                 /* health_max */
    COCO_NIGH_INVULNERABLE,             /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY_BUILDING, COCO_BLIP_EMY_BUILDING, COCO_BLIP_EMY_BUILDING ), /* default_blip_color_index */
    COCO_GUN_GREY_RASTER_W,             /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    -1,                                 /* z */
    COCO_GUN_RASTER_COUNT,              /* raster_count */
    coco_gun_rasters,                   /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_2raster_guess_nationality,     /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_cobject_barreloid_draw,        /* cobject_draw */
    4,                                  /* shrapnel_count */
    4,                                  /* refund */
    0,                                  /* cost */
    200,                                /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    coco_gun_hardpoints,                /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    20,                                 /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    coco_gun_fire_bullet_coords,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_trooper_always_target_me,
    0,
    coco_gun_tank_to_cobject,
    0,
    coco_light_armor_effective_damage,
    0,
    0,
    0,
    0,
    coco_gun_kaboom,
    coco_gun_update_state,
    0,
  },
  { /* coco_aa_gun */
    20,                                 /* cl_index */
    2,                                  /* init_by_list_count */
    coco_aa_gun_automatic_init_by_list, /* init_by_list_fn */
    "aa-gun",                           /* name */
    (coco_sobject_classes + COCO_GUN_CLASS_INDEX), /* super */
    COCO_SOLID,                         /* phase */
    COCO_FALSE,                         /* rotate_p */
    0,                                  /* dx_max */
    0,                                  /* dy_max */
    0,                                  /* x_force */
    0,                                  /* y_force */
    23,                                 /* health_max */
    COCO_NIGH_INVULNERABLE,             /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY_BUILDING, COCO_BLIP_EMY_BUILDING, COCO_BLIP_EMY_BUILDING ), /* default_blip_color_index */
    COCO_GUN_GREY_RASTER_W,             /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    -1,                                 /* z */
    COCO_GUN_RASTER_COUNT,              /* raster_count */
    coco_gun_rasters,                   /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_2raster_guess_nationality,     /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_cobject_barreloid_draw,        /* cobject_draw */
    4,                                  /* shrapnel_count */
    4,                                  /* refund */
    0,                                  /* cost */
    200,                                /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    coco_gun_hardpoints,                /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    20,                                 /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    1,                                  /* targetable_classes_count */
    COCO_COPTER_CLASS,                  /* targetable_classes */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
  },
  { /* coco_at_gun */
    21,                                 /* cl_index */
    2,                                  /* init_by_list_count */
    coco_at_gun_automatic_init_by_list, /* init_by_list_fn */
    "at-gun",                           /* name */
    (coco_sobject_classes + COCO_GUN_CLASS_INDEX), /* super */
    COCO_SOLID,                         /* phase */
    COCO_FALSE,                         /* rotate_p */
    0,                                  /* dx_max */
    0,                                  /* dy_max */
    0,                                  /* x_force */
    0,                                  /* y_force */
    23,                                 /* health_max */
    COCO_NIGH_INVULNERABLE,             /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY_BUILDING, COCO_BLIP_EMY_BUILDING, COCO_BLIP_EMY_BUILDING ), /* default_blip_color_index */
    COCO_GUN_GREY_RASTER_W,             /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    -1,                                 /* z */
    COCO_GUN_RASTER_COUNT,              /* raster_count */
    coco_gun_rasters,                   /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_2raster_guess_nationality,     /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_cobject_barreloid_draw,        /* cobject_draw */
    4,                                  /* shrapnel_count */
    4,                                  /* refund */
    0,                                  /* cost */
    200,                                /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    coco_gun_hardpoints,                /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    20,                                 /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    1,                                  /* targetable_classes_count */
    COCO_GROUND_VEHICLE_CLASS,          /* targetable_classes */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
  },
  { /* coco_gun_rubble */
    22,                                 /* cl_index */
    2,                                  /* init_by_list_count */
    coco_gun_rubble_automatic_init_by_list, /* init_by_list_fn */
    "gun-rubble",                       /* name */
    (coco_sobject_classes + COCO_BUILDING_CLASS_INDEX), /* super */
    COCO_VAPOR,                         /* phase */
    COCO_FALSE,                         /* rotate_p */
    0,                                  /* dx_max */
    0,                                  /* dy_max */
    0,                                  /* x_force */
    0,                                  /* y_force */
    COCO_NIGH_INVULNERABLE,             /* health_max */
    COCO_NIGH_INVULNERABLE,             /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_BLIP_TRIPLET_TRANSPARENT,      /* default_blip_color_index */
    COCO_GUN_RUBBLE_RASTER_W,           /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    -1,                                 /* z */
    COCO_GUN_RUBBLE_RASTER_COUNT,       /* raster_count */
    coco_gun_rubble_rasters,            /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_simple_raster_index,   /* cobject_raster_index_fn */
    coco_neutral_guess_nationality,     /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_cobject_rasteroid_draw,        /* cobject_draw */
    4,                                  /* shrapnel_count */
    0,                                  /* refund */
    0,                                  /* cost */
    200,                                /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_gun_rubble_update_state,
    0,
  },
  { /* coco_ground_unit */
    23,                                 /* cl_index */
    2,                                  /* init_by_list_count */
    NULL,                               /* init_by_list_fn */
    "ground-unit",                      /* name */
    (coco_sobject_classes + COCO_SOBJECT_CLASS_INDEX), /* super */
    COCO_SOLID,                         /* phase */
    COCO_FALSE,                         /* rotate_p */
    COCO_GROUND_UNIT_DX_MAX,            /* dx_max */
    1,                                  /* dy_max */
    0,                                  /* x_force */
    0,                                  /* y_force */
    COCO_BOMB_DAMAGE-1,                 /* health_max */
    COCO_NIGH_INVULNERABLE,             /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY, COCO_BLIP_EMY, COCO_BLIP_EMY ), /* default_blip_color_index */
    1 /* placeholder */,                /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    0,                                  /* z */
    0,                                  /* raster_count */
    NULL,                               /* rasters */
    coco_cobject_ground_unit_y,         /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_cobject_whimpy_guess_nationality, /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_cobject_rasteroid_draw,        /* cobject_draw */
    0,                                  /* shrapnel_count */
    0,                                  /* refund */
    0,                                  /* cost */
    100,                                /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_ground_rasteroid_to_cobject,
    0,
    0,
    0,
    coco_ground_unit_hostile_collision,
    0,
    0,
    0,
    coco_ground_unit_update_state,
    0,
  },
  { /* coco_ground_vehicle */
    24,                                 /* cl_index */
    2,                                  /* init_by_list_count */
    coco_ground_vehicle_automatic_init_by_list, /* init_by_list_fn */
    "ground-vehicle",                   /* name */
    (coco_sobject_classes + COCO_GROUND_UNIT_CLASS_INDEX), /* super */
    COCO_SOLID,                         /* phase */
    COCO_FALSE,                         /* rotate_p */
    COCO_GROUND_UNIT_DX_MAX,            /* dx_max */
    1,                                  /* dy_max */
    0,                                  /* x_force */
    0,                                  /* y_force */
    COCO_BOMB_DAMAGE-1,                 /* health_max */
    COCO_NIGH_INVULNERABLE,             /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY, COCO_BLIP_EMY, COCO_BLIP_EMY ), /* default_blip_color_index */
    1 /* placeholder */,                /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    0,                                  /* z */
    0,                                  /* raster_count */
    NULL,                               /* rasters */
    coco_cobject_ground_unit_y,         /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_2raster_guess_nationality,     /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_cobject_rasteroid_draw,        /* cobject_draw */
    3,                                  /* shrapnel_count */
    0,                                  /* refund */
    0,                                  /* cost */
    100,                                /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_airbursting_kaboom,
    0,
    0,
  },
  { /* coco_demo_van */
    25,                                 /* cl_index */
    2,                                  /* init_by_list_count */
    coco_demo_van_automatic_init_by_list, /* init_by_list_fn */
    "demo-van",                         /* name */
    (coco_sobject_classes + COCO_GROUND_VEHICLE_CLASS_INDEX), /* super */
    COCO_SOLID,                         /* phase */
    COCO_FALSE,                         /* rotate_p */
    COCO_GROUND_UNIT_DX_MAX,            /* dx_max */
    1,                                  /* dy_max */
    0,                                  /* x_force */
    0,                                  /* y_force */
    2,                                  /* health_max */
    COCO_NIGH_INVULNERABLE,             /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY, COCO_BLIP_EMY, COCO_BLIP_EMY ), /* default_blip_color_index */
    COCO_VAN_GREEN_RASTER_W,            /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    0,                                  /* z */
    COCO_DEMO_VAN_RASTER_COUNT,         /* raster_count */
    coco_demo_van_rasters,              /* rasters */
    coco_cobject_ground_unit_y,         /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_2raster_guess_nationality,     /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_cobject_rasteroid_draw,        /* cobject_draw */
    4,                                  /* shrapnel_count */
    0,                                  /* refund */
    2,                                  /* cost */
    100,                                /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_demo_van_hostile_collision,
    0,
    0,
    0,
    0,
    0,
  },
  { /* coco_tank */
    26,                                 /* cl_index */
    2,                                  /* init_by_list_count */
    coco_ground_vehicle_automatic_init_by_list, /* init_by_list_fn */
    "tank",                             /* name */
    (coco_sobject_classes + COCO_GROUND_VEHICLE_CLASS_INDEX), /* super */
    COCO_SOLID,                         /* phase */
    COCO_FALSE,                         /* rotate_p */
    COCO_GROUND_UNIT_DX_MAX,            /* dx_max */
    1,                                  /* dy_max */
    0,                                  /* x_force */
    0,                                  /* y_force */
    COCO_BOMB_DAMAGE*3,                 /* health_max */
    COCO_NIGH_INVULNERABLE,             /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY, COCO_BLIP_EMY, COCO_BLIP_EMY ), /* default_blip_color_index */
    COCO_TANK_GREEN_RASTER_W,           /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    0,                                  /* z */
    COCO_TANK_RASTER_COUNT,             /* raster_count */
    coco_tank_rasters,                  /* rasters */
    coco_cobject_ground_unit_y,         /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_2raster_guess_nationality,     /* cobject_guess_nationality_fn */
    COCO_TANK_BULLET_CLASS,             /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_cobject_barreloid_draw,        /* cobject_draw */
    3,                                  /* shrapnel_count */
    1,                                  /* refund */
    4,                                  /* cost */
    100,                                /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    coco_tank_hardpoints,               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    29,                                 /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_TANK_GUN,                /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    coco_tank_infantry_fire_bullet_coords,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_gun_tank_to_cobject,
    0,
    coco_heavy_armor_effective_damage,
    0,
    0,
    0,
    0,
    0,
    coco_tank_update_state,
    0,
  },
  { /* coco_firetruck */
    27,                                 /* cl_index */
    2,                                  /* init_by_list_count */
    coco_firetruck_automatic_init_by_list, /* init_by_list_fn */
    "firetruck",                        /* name */
    (coco_sobject_classes + COCO_GROUND_VEHICLE_CLASS_INDEX), /* super */
    COCO_SOLID,                         /* phase */
    COCO_FALSE,                         /* rotate_p */
    COCO_GROUND_UNIT_DX_MAX,            /* dx_max */
    1,                                  /* dy_max */
    0,                                  /* x_force */
    0,                                  /* y_force */
    COCO_BOMB_DAMAGE-1,                 /* health_max */
    COCO_NIGH_INVULNERABLE,             /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY, COCO_BLIP_EMY, COCO_BLIP_EMY ), /* default_blip_color_index */
    COCO_FIRETRUCK_GREEN_RASTER_W,      /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    0,                                  /* z */
    COCO_FIRETRUCK_RASTER_COUNT,        /* raster_count */
    coco_firetruck_rasters,             /* rasters */
    coco_cobject_ground_unit_y,         /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_firetruck_guess_nationality,   /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_cobject_rasteroid_draw,        /* cobject_draw */
    3,                                  /* shrapnel_count */
    0,                                  /* refund */
    3,                                  /* cost */
    100,                                /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    coco_firetruck_launch_missile,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_firetruck_update_state,
    0,
  },
  { /* coco_infantry */
    28,                                 /* cl_index */
    2,                                  /* init_by_list_count */
    coco_infantry_init_from_ground_by_list, /* init_by_list_fn */
    "infantry",                         /* name */
    (coco_sobject_classes + COCO_GROUND_UNIT_CLASS_INDEX), /* super */
    COCO_SOLID,                         /* phase */
    COCO_FALSE,                         /* rotate_p */
    COCO_GROUND_UNIT_DX_MAX,            /* dx_max */
    COCO_GNUM_MAX,                      /* dy_max */
    0,                                  /* x_force */
    COCO_FLYING_MAN_ACCELL,             /* y_force */
    1,                                  /* health_max */
    0,                                  /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY_INFANTRY, COCO_BLIP_TRANSPARENT, COCO_BLIP_TRANSPARENT ), /* default_blip_color_index */
    COCO_INFANTRY_E1_RASTER_W,          /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    1,                                  /* z */
    COCO_INFANTRY_RASTER_COUNT,         /* raster_count */
    coco_infantry_rasters,              /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_infantry_guess_nationality,    /* cobject_guess_nationality_fn */
    COCO_RIFLE_BULLET_CLASS,            /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_cobject_rasteroid_draw,        /* cobject_draw */
    0,                                  /* shrapnel_count */
    0,                                  /* refund */
    1,                                  /* cost */
    75,                                 /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_RIFLE,                   /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    coco_tank_infantry_fire_bullet_coords,
    0,
    0,
    coco_infantry_enter_friendly_bunker,
    coco_feisty_infantry_enter_hut,
    0,
    0,
    0,
    0,
    0,
    coco_rasteroid_to_cobject,
    0,
    coco_infantry_effective_damage,
    0,
    0,
    0,
    0,
    coco_infantry_kaboom,
    coco_infantry_update_state,
    0,
  },
  { /* coco_engineer */
    29,                                 /* cl_index */
    2,                                  /* init_by_list_count */
    coco_infantry_init_from_ground_by_list, /* init_by_list_fn */
    "engineer",                         /* name */
    (coco_sobject_classes + COCO_INFANTRY_CLASS_INDEX), /* super */
    COCO_SOLID,                         /* phase */
    COCO_FALSE,                         /* rotate_p */
    COCO_GROUND_UNIT_DX_MAX,            /* dx_max */
    COCO_GNUM_MAX,                      /* dy_max */
    0,                                  /* x_force */
    COCO_FLYING_MAN_ACCELL,             /* y_force */
    1,                                  /* health_max */
    0,                                  /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY_INFANTRY, COCO_BLIP_TRANSPARENT, COCO_BLIP_TRANSPARENT ), /* default_blip_color_index */
    COCO_INFANTRY_E1_RASTER_W,          /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    1,                                  /* z */
    COCO_ENGINEER_RASTER_COUNT,         /* raster_count */
    coco_engineer_rasters,              /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_infantry_guess_nationality,    /* cobject_guess_nationality_fn */
    COCO_RIFLE_BULLET_CLASS,            /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_cobject_rasteroid_draw,        /* cobject_draw */
    0,                                  /* shrapnel_count */
    1,                                  /* refund */
    2,                                  /* cost */
    75,                                 /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_FALSE,                         /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_RIFLE,                   /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    0,
    0,
    0,
    coco_ignore_bunker,
    coco_engineer_enter_hut,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_engineer_update_state,
    0,
  },
  { /* coco_munition */
    30,                                 /* cl_index */
    6,                                  /* init_by_list_count */
    NULL,                               /* init_by_list_fn */
    "munition",                         /* name */
    (coco_sobject_classes + COCO_SOBJECT_CLASS_INDEX), /* super */
    COCO_EXPLOSIVE,                     /* phase */
    COCO_FALSE,                         /* rotate_p */
    0,                                  /* dx_max */
    0,                                  /* dy_max */
    0,                                  /* x_force */
    0,                                  /* y_force */
    1,                                  /* health_max */
    COCO_NIGH_INVULNERABLE,             /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY_MUNITION, COCO_BLIP_EMY_MUNITION, COCO_BLIP_EMY_MUNITION ), /* default_blip_color_index */
    1 /* placeholder */,                /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    0,                                  /* z */
    0,                                  /* raster_count */
    NULL,                               /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_cobject_whimpy_guess_nationality, /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_cobject_rasteroid_draw,        /* cobject_draw */
    0,                                  /* shrapnel_count */
    0,                                  /* refund */
    0,                                  /* cost */
    25,                                 /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_trooper_never_target_me,
    0,
    0,
    0,
    0,
    0,
    coco_munition_hostile_collision,
    0,
    0,
    0,
    0,
    0,
  },
  { /* coco_bullet */
    31,                                 /* cl_index */
    7,                                  /* init_by_list_count */
    NULL,                               /* init_by_list_fn */
    "bullet",                           /* name */
    (coco_sobject_classes + COCO_MUNITION_CLASS_INDEX), /* super */
    COCO_EXPLOSIVE,                     /* phase */
    COCO_FALSE,                         /* rotate_p */
    0,                                  /* dx_max */
    0,                                  /* dy_max */
    0,                                  /* x_force */
    0,                                  /* y_force */
    1,                                  /* health_max */
    COCO_BULLET_DAMAGE,                 /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY_BULLET, COCO_BLIP_EMY_BULLET, COCO_BLIP_EMY_BULLET ), /* default_blip_color_index */
    1,                                  /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    0,                                  /* z */
    0,                                  /* raster_count */
    NULL,                               /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_cobject_whimpy_guess_nationality, /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_bullet_draw,                   /* cobject_draw */
    0,                                  /* shrapnel_count */
    0,                                  /* refund */
    0,                                  /* cost */
    25,                                 /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    1,                                  /* spark_count */
    2,                                  /* thickness */
    COCO_BULLET_SPEED,                  /* muzzle_velocity */
    COCO_BULLET_LIFESPAN,               /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_bullet_hostile_collision,
    0,
    0,
    coco_bullet_kaboom,
    coco_bullet_update_state,
    0,
  },
  { /* coco_rifle_bullet */
    32,                                 /* cl_index */
    7,                                  /* init_by_list_count */
    NULL,                               /* init_by_list_fn */
    "rifle-bullet",                     /* name */
    (coco_sobject_classes + COCO_BULLET_CLASS_INDEX), /* super */
    COCO_EXPLOSIVE,                     /* phase */
    COCO_FALSE,                         /* rotate_p */
    0,                                  /* dx_max */
    0,                                  /* dy_max */
    0,                                  /* x_force */
    0,                                  /* y_force */
    1,                                  /* health_max */
    COCO_BULLET_DAMAGE,                 /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY_BULLET, COCO_BLIP_EMY_BULLET, COCO_BLIP_EMY_BULLET ), /* default_blip_color_index */
    1,                                  /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    0,                                  /* z */
    0,                                  /* raster_count */
    NULL,                               /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_cobject_whimpy_guess_nationality, /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_bullet_draw,                   /* cobject_draw */
    0,                                  /* shrapnel_count */
    0,                                  /* refund */
    0,                                  /* cost */
    25,                                 /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    1,                                  /* thickness */
    COCO_TANK_INFANTRY_BULLET_SPEED,    /* muzzle_velocity */
    COCO_BULLET_LIFESPAN,               /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_flesh_damageable_effective_damage,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
  },
  { /* coco_tank_bullet */
    33,                                 /* cl_index */
    7,                                  /* init_by_list_count */
    NULL,                               /* init_by_list_fn */
    "tank-bullet",                      /* name */
    (coco_sobject_classes + COCO_BULLET_CLASS_INDEX), /* super */
    COCO_EXPLOSIVE,                     /* phase */
    COCO_FALSE,                         /* rotate_p */
    0,                                  /* dx_max */
    0,                                  /* dy_max */
    0,                                  /* x_force */
    0,                                  /* y_force */
    1,                                  /* health_max */
    COCO_BULLET_DAMAGE<<1,              /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY_BULLET, COCO_BLIP_EMY_BULLET, COCO_BLIP_EMY_BULLET ), /* default_blip_color_index */
    1,                                  /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    0,                                  /* z */
    0,                                  /* raster_count */
    NULL,                               /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_cobject_whimpy_guess_nationality, /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_bullet_draw,                   /* cobject_draw */
    0,                                  /* shrapnel_count */
    0,                                  /* refund */
    0,                                  /* cost */
    25,                                 /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    4,                                  /* spark_count */
    3,                                  /* thickness */
    COCO_TANK_INFANTRY_BULLET_SPEED,    /* muzzle_velocity */
    COCO_BULLET_LIFESPAN,               /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
  },
  { /* coco_bomb */
    34,                                 /* cl_index */
    6,                                  /* init_by_list_count */
    NULL,                               /* init_by_list_fn */
    "bomb",                             /* name */
    (coco_sobject_classes + COCO_MUNITION_CLASS_INDEX), /* super */
    COCO_EXPLOSIVE,                     /* phase */
    COCO_FALSE,                         /* rotate_p */
    COCO_GNUM_MAX,                      /* dx_max */
    COCO_GNUM_MAX,                      /* dy_max */
    0,                                  /* x_force */
    COCO_GRAVITY_ACCELL,                /* y_force */
    1,                                  /* health_max */
    COCO_BOMB_DAMAGE,                   /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY_MUNITION, COCO_BLIP_EMY_MUNITION, COCO_BLIP_EMY_MUNITION ), /* default_blip_color_index */
    COCO_BOMB_SOUTH_RASTER_W,           /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    0,                                  /* z */
    COCO_BOMB_RASTER_COUNT,             /* raster_count */
    coco_bomb_rasters,                  /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_cobject_whimpy_guess_nationality, /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_cobject_rasteroid_draw,        /* cobject_draw */
    0,                                  /* shrapnel_count */
    0,                                  /* refund */
    0,                                  /* cost */
    25,                                 /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_bomb_kaboom,
    coco_bomb_update_state,
    0,
  },
  { /* coco_abstract_missile */
    35,                                 /* cl_index */
    7,                                  /* init_by_list_count */
    NULL,                               /* init_by_list_fn */
    "abstract-missile",                 /* name */
    (coco_sobject_classes + COCO_MUNITION_CLASS_INDEX), /* super */
    COCO_SOLID,                         /* phase */
    COCO_TRUE,                          /* rotate_p */
    COCO_GNUM_MAX,                      /* dx_max */
    COCO_COPTER_DY_MAX + (COCO_COPTER_DY_MAX<<1), /* dy_max */
    0,                                  /* x_force */
    0,                                  /* y_force */
    1,                                  /* health_max */
    COCO_MISSILE_DAMAGE,                /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY_MUNITION, COCO_BLIP_EMY_MUNITION, COCO_BLIP_EMY_MUNITION ), /* default_blip_color_index */
    11,                                 /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    0,                                  /* z */
    0,                                  /* raster_count */
    NULL,                               /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_cobject_whimpy_guess_nationality, /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_missile_draw,                  /* cobject_draw */
    1,                                  /* shrapnel_count */
    0,                                  /* refund */
    0,                                  /* cost */
    25,                                 /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_y_angle_to_cobject,
    0,
    coco_flesh_damageable_effective_damage,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
  },
  { /* coco_rocket */
    36,                                 /* cl_index */
    7,                                  /* init_by_list_count */
    NULL,                               /* init_by_list_fn */
    "rocket",                           /* name */
    (coco_sobject_classes + COCO_ABSTRACT_MISSILE_CLASS_INDEX), /* super */
    COCO_SOLID,                         /* phase */
    COCO_TRUE,                          /* rotate_p */
    600/COCO_IDEAL_FRAMES_PER_SECOND,   /* dx_max */
    COCO_COPTER_DY_MAX + (COCO_COPTER_DY_MAX<<1), /* dy_max */
    0,                                  /* x_force */
    0,                                  /* y_force */
    1,                                  /* health_max */
    COCO_MISSILE_DAMAGE,                /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY_MUNITION, COCO_BLIP_EMY_MUNITION, COCO_BLIP_EMY_MUNITION ), /* default_blip_color_index */
    11,                                 /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    0,                                  /* z */
    0,                                  /* raster_count */
    NULL,                               /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_cobject_whimpy_guess_nationality, /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_missile_draw,                  /* cobject_draw */
    1,                                  /* shrapnel_count */
    0,                                  /* refund */
    0,                                  /* cost */
    25,                                 /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_rocket_kaboom,
    coco_rocket_update_state,
    0,
  },
  { /* coco_missile */
    37,                                 /* cl_index */
    8,                                  /* init_by_list_count */
    NULL,                               /* init_by_list_fn */
    "missile",                          /* name */
    (coco_sobject_classes + COCO_ABSTRACT_MISSILE_CLASS_INDEX), /* super */
    COCO_SOLID,                         /* phase */
    COCO_TRUE,                          /* rotate_p */
    COCO_GNUM_MAX,                      /* dx_max */
    COCO_COPTER_DY_MAX + (COCO_COPTER_DY_MAX<<1), /* dy_max */
    0,                                  /* x_force */
    0,                                  /* y_force */
    1,                                  /* health_max */
    COCO_MISSILE_DAMAGE,                /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY_MUNITION, COCO_BLIP_EMY_MUNITION, COCO_BLIP_EMY_MUNITION ), /* default_blip_color_index */
    11,                                 /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    0,                                  /* z */
    0,                                  /* raster_count */
    NULL,                               /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_cobject_whimpy_guess_nationality, /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_missile_draw,                  /* cobject_draw */
    1,                                  /* shrapnel_count */
    0,                                  /* refund */
    0,                                  /* cost */
    25,                                 /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_airbursting_kaboom,
    coco_missile_update_state,
    coco_missile_unlink,
  },
  { /* coco_spark */
    38,                                 /* cl_index */
    4,                                  /* init_by_list_count */
    NULL,                               /* init_by_list_fn */
    "spark",                            /* name */
    (coco_sobject_classes + COCO_MUNITION_CLASS_INDEX), /* super */
    COCO_VAPOR,                         /* phase */
    COCO_FALSE,                         /* rotate_p */
    0,                                  /* dx_max */
    0,                                  /* dy_max */
    0,                                  /* x_force */
    0,                                  /* y_force */
    1,                                  /* health_max */
    COCO_NIGH_INVULNERABLE,             /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_BLIP_TRIPLET_TRANSPARENT,      /* default_blip_color_index */
    1,                                  /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    2,                                  /* z */
    0,                                  /* raster_count */
    NULL,                               /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_cobject_whimpy_guess_nationality, /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_spark_draw,                    /* cobject_draw */
    0,                                  /* shrapnel_count */
    0,                                  /* refund */
    0,                                  /* cost */
    25,                                 /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_spark_to_cobject,
    coco_ignore_ground_collide,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_spark_update_state,
    0,
  },
  { /* coco_shrapnel */
    39,                                 /* cl_index */
    4,                                  /* init_by_list_count */
    NULL,                               /* init_by_list_fn */
    "shrapnel",                         /* name */
    (coco_sobject_classes + COCO_MUNITION_CLASS_INDEX), /* super */
    COCO_EXPLOSIVE,                     /* phase */
    COCO_FALSE,                         /* rotate_p */
    COCO_GNUM_MAX,                      /* dx_max */
    COCO_GNUM_MAX,                      /* dy_max */
    0,                                  /* x_force */
    COCO_GRAVITY_ACCELL,                /* y_force */
    1,                                  /* health_max */
    COCO_SHRAPNEL_DAMAGE,               /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_BLIP_TRIPLET_TRANSPARENT,      /* default_blip_color_index */
    24,                                 /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    0,                                  /* z */
    0,                                  /* raster_count */
    NULL,                               /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_neutral_guess_nationality,     /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_cobject_rasteroid_draw,        /* cobject_draw */
    0,                                  /* shrapnel_count */
    0,                                  /* refund */
    0,                                  /* cost */
    25,                                 /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_shrapnel_update_state,
    0,
  },
  { /* coco_metal_shrapnel */
    40,                                 /* cl_index */
    4,                                  /* init_by_list_count */
    NULL,                               /* init_by_list_fn */
    "metal-shrapnel",                   /* name */
    (coco_sobject_classes + COCO_SHRAPNEL_CLASS_INDEX), /* super */
    COCO_EXPLOSIVE,                     /* phase */
    COCO_FALSE,                         /* rotate_p */
    COCO_GNUM_MAX,                      /* dx_max */
    COCO_GNUM_MAX,                      /* dy_max */
    0,                                  /* x_force */
    COCO_GRAVITY_ACCELL,                /* y_force */
    1,                                  /* health_max */
    COCO_SHRAPNEL_DAMAGE,               /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_BLIP_TRIPLET_TRANSPARENT,      /* default_blip_color_index */
    24,                                 /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    0,                                  /* z */
    0,                                  /* raster_count */
    NULL,                               /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_neutral_guess_nationality,     /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_metal_shrapnel_draw,           /* cobject_draw */
    0,                                  /* shrapnel_count */
    0,                                  /* refund */
    0,                                  /* cost */
    25,                                 /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_metal_shrapnel_to_cobject,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_metal_shrapnel_update_state,
    0,
  },
  { /* coco_big_metal_shrapnel */
    41,                                 /* cl_index */
    4,                                  /* init_by_list_count */
    NULL,                               /* init_by_list_fn */
    "big-metal-shrapnel",               /* name */
    (coco_sobject_classes + COCO_METAL_SHRAPNEL_CLASS_INDEX), /* super */
    COCO_EXPLOSIVE,                     /* phase */
    COCO_FALSE,                         /* rotate_p */
    COCO_GNUM_MAX,                      /* dx_max */
    COCO_GNUM_MAX,                      /* dy_max */
    0,                                  /* x_force */
    COCO_GRAVITY_ACCELL,                /* y_force */
    1,                                  /* health_max */
    COCO_SHRAPNEL_DAMAGE,               /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_BLIP_TRIPLET_TRANSPARENT,      /* default_blip_color_index */
    COCO_SHRAPNEL_32A_RASTER_W,         /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    0,                                  /* z */
    COCO_BIG_METAL_RASTER_COUNT,        /* raster_count */
    coco_big_metal_shrapnel_rasters,    /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_neutral_guess_nationality,     /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_metal_shrapnel_draw,           /* cobject_draw */
    0,                                  /* shrapnel_count */
    0,                                  /* refund */
    0,                                  /* cost */
    25,                                 /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
  },
  { /* coco_med_metal_shrapnel */
    42,                                 /* cl_index */
    4,                                  /* init_by_list_count */
    NULL,                               /* init_by_list_fn */
    "med-metal-shrapnel",               /* name */
    (coco_sobject_classes + COCO_METAL_SHRAPNEL_CLASS_INDEX), /* super */
    COCO_EXPLOSIVE,                     /* phase */
    COCO_FALSE,                         /* rotate_p */
    COCO_GNUM_MAX,                      /* dx_max */
    COCO_GNUM_MAX,                      /* dy_max */
    0,                                  /* x_force */
    COCO_GRAVITY_ACCELL,                /* y_force */
    1,                                  /* health_max */
    COCO_SHRAPNEL_DAMAGE,               /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_BLIP_TRIPLET_TRANSPARENT,      /* default_blip_color_index */
    COCO_SHRAPNEL_24A_RASTER_W,         /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    0,                                  /* z */
    COCO_MED_METAL_RASTER_COUNT,        /* raster_count */
    coco_med_metal_shrapnel_rasters,    /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_neutral_guess_nationality,     /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_metal_shrapnel_draw,           /* cobject_draw */
    0,                                  /* shrapnel_count */
    0,                                  /* refund */
    0,                                  /* cost */
    25,                                 /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
  },
  { /* coco_small_metal_shrapnel */
    43,                                 /* cl_index */
    4,                                  /* init_by_list_count */
    NULL,                               /* init_by_list_fn */
    "small-metal-shrapnel",             /* name */
    (coco_sobject_classes + COCO_METAL_SHRAPNEL_CLASS_INDEX), /* super */
    COCO_EXPLOSIVE,                     /* phase */
    COCO_FALSE,                         /* rotate_p */
    COCO_GNUM_MAX,                      /* dx_max */
    COCO_GNUM_MAX,                      /* dy_max */
    0,                                  /* x_force */
    COCO_GRAVITY_ACCELL,                /* y_force */
    1,                                  /* health_max */
    COCO_SHRAPNEL_DAMAGE,               /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_BLIP_TRIPLET_TRANSPARENT,      /* default_blip_color_index */
    COCO_SHRAPNEL_16A_RASTER_W,         /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    0,                                  /* z */
    COCO_SMALL_METAL_RASTER_COUNT,      /* raster_count */
    coco_small_metal_shrapnel_rasters,  /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_neutral_guess_nationality,     /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_metal_shrapnel_draw,           /* cobject_draw */
    0,                                  /* shrapnel_count */
    0,                                  /* refund */
    0,                                  /* cost */
    25,                                 /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
  },
  { /* coco_explosion */
    44,                                 /* cl_index */
    6,                                  /* init_by_list_count */
    NULL,                               /* init_by_list_fn */
    "explosion",                        /* name */
    (coco_sobject_classes + COCO_MUNITION_CLASS_INDEX), /* super */
    COCO_EXPLOSIVE,                     /* phase */
    COCO_FALSE,                         /* rotate_p */
    0,                                  /* dx_max */
    0,                                  /* dy_max */
    0,                                  /* x_force */
    0,                                  /* y_force */
    1,                                  /* health_max */
    0,                                  /* damage */
    COCO_FALSE,                         /* clip_to_board */
    COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_EXPLOSION, COCO_BLIP_EXPLOSION, COCO_BLIP_EXPLOSION ), /* default_blip_color_index */
    1 /* placeholder */,                /* canonical_w */
    1 /* placeholder */,                /* canonical_h */
    1,                                  /* z */
    COCO_EXPLOSION_RASTER_COUNT,        /* raster_count */
    coco_explosion_rasters,             /* rasters */
    coco_cobject_y_y,                   /* cobject_y_fn */
    coco_cobject_standard_raster_index, /* cobject_raster_index_fn */
    coco_neutral_guess_nationality,     /* cobject_guess_nationality_fn */
    COCO_BULLET_CLASS,                  /* bullet_class */
    coco_cobject_y_ntoh,                /* cobject_ntoh */
    coco_cobject_rasteroid_draw,        /* cobject_draw */
    0,                                  /* shrapnel_count */
    0,                                  /* refund */
    0,                                  /* cost */
    25,                                 /* collision_priority */
    COCO_MINI_RECTANGLE_NULL,           /* collision_rectangle */
    0,                                  /* rotor_radius */
    0,                                  /* max_bullets */
    0,                                  /* max_bombs */
    0,                                  /* max_rockets */
    0,                                  /* max_missiles */
    0,                                  /* max_internal_infantry */
    0,                                  /* max_fuel */
    NULL,                               /* rotor_points */
    NULL,                               /* weapon_hardpoints */
    NULL,                               /* center_of_mass */
    0,                                  /* gun_barrel_length */
    COCO_TRUE,                          /* infantry_take_buildings_p */
    0,                                  /* spark_count */
    0,                                  /* thickness */
    0,                                  /* muzzle_velocity */
    0,                                  /* lifespan */
    0,                                  /* roof_tab */
    COCO_SOUND_AIRBURST,                /* airburst_sound_type */
    COCO_SOUND_COPTER_CANNON,           /* fire_bullet_sound_type */
    0,                                  /* targetable_classes_count */
    NULL,                               /* targetable_classes */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_ignore_ground_collide,
    0,
    0,
    0,
    0,
    0,
    0,
    coco_explosion_update_state,
    0,
  },
};
int coco_sobject_subtype_p( coco_sobject_cl *sub, coco_sobject_cl *super )
{
  do
    if (sub==super) return 1;
    else sub = sub->super;
  while (sub);
  return 0;
}
/* Generic Functions */

coco_sobject *
coco_drop_paratrooper( coco_sobject * inst, coco_sobject_cl * cl, coco_sstate * sstate )
{
  do
   {
     coco_sobject * (* method)( coco_sobject *, coco_sobject_cl * , coco_sstate * ) = cl->drop_paratrooper;
     if (method)
       return method( inst, cl, sstate );
     cl = cl->super;
   }
  while (cl);
  coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM, "No method on drop_paratrooper inst=%p, class_pass_idx = %d, class_slot_idx = %d\n", inst, cl ? cl->cl_index : -1, inst->every.cl->cl_index );
  return (coco_sobject *) 0; /* convince compiler we don't fall off end */
}

coco_sobject *
coco_launch_missile( coco_sobject * inst, coco_sobject_cl * cl, coco_sstate * sstate, coco_sobject * target )
{
  do
   {
     coco_sobject * (* method)( coco_sobject *, coco_sobject_cl * , coco_sstate * , coco_sobject * ) = cl->launch_missile;
     if (method)
       return method( inst, cl, sstate, target );
     cl = cl->super;
   }
  while (cl);
  coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM, "No method on launch_missile inst=%p, class_pass_idx = %d, class_slot_idx = %d\n", inst, cl ? cl->cl_index : -1, inst->every.cl->cl_index );
  return (coco_sobject *) 0; /* convince compiler we don't fall off end */
}

coco_sobject *
coco_fire_bullet( coco_sobject * inst, coco_sobject_cl * cl, coco_sstate * sstate, coco_sobject * target_or_null )
{
  do
   {
     coco_sobject * (* method)( coco_sobject *, coco_sobject_cl * , coco_sstate * , coco_sobject * ) = cl->fire_bullet;
     if (method)
       return method( inst, cl, sstate, target_or_null );
     cl = cl->super;
   }
  while (cl);
  coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM, "No method on fire_bullet inst=%p, class_pass_idx = %d, class_slot_idx = %d\n", inst, cl ? cl->cl_index : -1, inst->every.cl->cl_index );
  return (coco_sobject *) 0; /* convince compiler we don't fall off end */
}

void
coco_fire_bullet_coords( coco_sobject * inst, coco_sobject_cl * cl, coco_sobject * target, coco_gnum * dx_return, coco_gnum * dy_return, coco_gnum * x_return, coco_gnum * y_return )
{
  do
   {
     void (* method)( coco_sobject *, coco_sobject_cl * , coco_sobject * , coco_gnum * , coco_gnum * , coco_gnum * , coco_gnum * ) = cl->fire_bullet_coords;
     if (method)
       { method( inst, cl, target, dx_return, dy_return, x_return, y_return ); return; }
     cl = cl->super;
   }
  while (cl);
  coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM, "No method on fire_bullet_coords inst=%p, class_pass_idx = %d, class_slot_idx = %d\n", inst, cl ? cl->cl_index : -1, inst->every.cl->cl_index );
  return; /* convince compiler we don't fall off end */
}

void
coco_toggle_facing( coco_sobject * inst, coco_sobject_cl * cl )
{
  do
   {
     void (* method)( coco_sobject *, coco_sobject_cl * ) = cl->toggle_facing;
     if (method)
       { method( inst, cl ); return; }
     cl = cl->super;
   }
  while (cl);
  coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM, "No method on toggle_facing inst=%p, class_pass_idx = %d, class_slot_idx = %d\n", inst, cl ? cl->cl_index : -1, inst->every.cl->cl_index );
  return; /* convince compiler we don't fall off end */
}

unsigned char
coco_raster_flip_index( coco_sobject * inst, coco_sobject_cl * cl, unsigned char index )
{
  do
   {
     unsigned char (* method)( coco_sobject *, coco_sobject_cl * , unsigned char ) = cl->raster_flip_index;
     if (method)
       return method( inst, cl, index );
     cl = cl->super;
   }
  while (cl);
  coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM, "No method on raster_flip_index inst=%p, class_pass_idx = %d, class_slot_idx = %d\n", inst, cl ? cl->cl_index : -1, inst->every.cl->cl_index );
  return (unsigned char) 0; /* convince compiler we don't fall off end */
}

void
coco_enter_friendly_bunker( coco_sobject * inst, coco_sobject_cl * cl, coco_sobject * bunker, coco_sstate * sstate )
{
  do
   {
     void (* method)( coco_sobject *, coco_sobject_cl * , coco_sobject * , coco_sstate * ) = cl->enter_friendly_bunker;
     if (method)
       { method( inst, cl, bunker, sstate ); return; }
     cl = cl->super;
   }
  while (cl);
  coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM, "No method on enter_friendly_bunker inst=%p, class_pass_idx = %d, class_slot_idx = %d\n", inst, cl ? cl->cl_index : -1, inst->every.cl->cl_index );
  return; /* convince compiler we don't fall off end */
}

coco_bool
coco_infantry_enter_hut( coco_sobject * inst, coco_sobject_cl * cl, coco_sobject * hut, coco_sstate * sstate, coco_bool * stand_still_p )
{
  do
   {
     coco_bool (* method)( coco_sobject *, coco_sobject_cl * , coco_sobject * , coco_sstate * , coco_bool * ) = cl->infantry_enter_hut;
     if (method)
       return method( inst, cl, hut, sstate, stand_still_p );
     cl = cl->super;
   }
  while (cl);
  coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM, "No method on infantry_enter_hut inst=%p, class_pass_idx = %d, class_slot_idx = %d\n", inst, cl ? cl->cl_index : -1, inst->every.cl->cl_index );
  return (coco_bool) 0; /* convince compiler we don't fall off end */
}

coco_gnum
coco_camera_velocity_offset( coco_sobject * inst, coco_sobject_cl * cl )
{
  do
   {
     coco_gnum (* method)( coco_sobject *, coco_sobject_cl * ) = cl->camera_velocity_offset;
     if (method)
       return method( inst, cl );
     cl = cl->super;
   }
  while (cl);
  coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM, "No method on camera_velocity_offset inst=%p, class_pass_idx = %d, class_slot_idx = %d\n", inst, cl ? cl->cl_index : -1, inst->every.cl->cl_index );
  return (coco_gnum) 0; /* convince compiler we don't fall off end */
}

coco_bool
coco_sobject_is_visible( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum cam_left, coco_gnum cam_right, coco_sobject * cam_or_null )
{
  do
   {
     coco_bool (* method)( coco_sobject *, coco_sobject_cl * , coco_gnum , coco_gnum , coco_sobject * ) = cl->sobject_is_visible;
     if (method)
       return method( inst, cl, cam_left, cam_right, cam_or_null );
     cl = cl->super;
   }
  while (cl);
  coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM, "No method on sobject_is_visible inst=%p, class_pass_idx = %d, class_slot_idx = %d\n", inst, cl ? cl->cl_index : -1, inst->every.cl->cl_index );
  return (coco_bool) 0; /* convince compiler we don't fall off end */
}

coco_blip_index
coco_sobject_blip_color_index( coco_sobject * inst, coco_sobject_cl * cl, int p_or_c_index, coco_bool is_comp, coco_sstate * sstate )
{
  do
   {
     coco_blip_index (* method)( coco_sobject *, coco_sobject_cl * , int , coco_bool , coco_sstate * ) = cl->sobject_blip_color_index;
     if (method)
       return method( inst, cl, p_or_c_index, is_comp, sstate );
     cl = cl->super;
   }
  while (cl);
  coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM, "No method on sobject_blip_color_index inst=%p, class_pass_idx = %d, class_slot_idx = %d\n", inst, cl ? cl->cl_index : -1, inst->every.cl->cl_index );
  return (coco_blip_index) 0; /* convince compiler we don't fall off end */
}

coco_bool
coco_ground_trooper_target_p( coco_sobject * inst, coco_sobject_cl * cl, coco_sobject * trooper )
{
  do
   {
     coco_bool (* method)( coco_sobject *, coco_sobject_cl * , coco_sobject * ) = cl->ground_trooper_target_p;
     if (method)
       return method( inst, cl, trooper );
     cl = cl->super;
   }
  while (cl);
  coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM, "No method on ground_trooper_target_p inst=%p, class_pass_idx = %d, class_slot_idx = %d\n", inst, cl ? cl->cl_index : -1, inst->every.cl->cl_index );
  return (coco_bool) 0; /* convince compiler we don't fall off end */
}

void
coco_sobject_neutral_building_collide( coco_sobject * inst, coco_sobject_cl * cl, coco_sobject * building )
{
  do
   {
     void (* method)( coco_sobject *, coco_sobject_cl * , coco_sobject * ) = cl->sobject_neutral_building_collide;
     if (method)
       { method( inst, cl, building ); return; }
     cl = cl->super;
   }
  while (cl);
  coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM, "No method on sobject_neutral_building_collide inst=%p, class_pass_idx = %d, class_slot_idx = %d\n", inst, cl ? cl->cl_index : -1, inst->every.cl->cl_index );
  return; /* convince compiler we don't fall off end */
}

void
coco_sobject_to_cobject( coco_sobject * inst, coco_sobject_cl * cl, coco_cobject * cobject, coco_gnum cam_left, coco_bool network_byte_order )
{
  do
   {
     void (* method)( coco_sobject *, coco_sobject_cl * , coco_cobject * , coco_gnum , coco_bool ) = cl->sobject_to_cobject;
     if (method)
       { method( inst, cl, cobject, cam_left, network_byte_order ); return; }
     cl = cl->super;
   }
  while (cl);
  coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM, "No method on sobject_to_cobject inst=%p, class_pass_idx = %d, class_slot_idx = %d\n", inst, cl ? cl->cl_index : -1, inst->every.cl->cl_index );
  return; /* convince compiler we don't fall off end */
}

void
coco_ground_collide( coco_sobject * inst, coco_sobject_cl * cl, coco_sstate * sstate )
{
  do
   {
     void (* method)( coco_sobject *, coco_sobject_cl * , coco_sstate * ) = cl->ground_collide;
     if (method)
       { method( inst, cl, sstate ); return; }
     cl = cl->super;
   }
  while (cl);
  coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM, "No method on ground_collide inst=%p, class_pass_idx = %d, class_slot_idx = %d\n", inst, cl ? cl->cl_index : -1, inst->every.cl->cl_index );
  return; /* convince compiler we don't fall off end */
}

coco_health
coco_effective_damage( coco_sobject * inst, coco_sobject_cl * cl, coco_sobject * damager )
{
  do
   {
     coco_health (* method)( coco_sobject *, coco_sobject_cl * , coco_sobject * ) = cl->effective_damage;
     if (method)
       return method( inst, cl, damager );
     cl = cl->super;
   }
  while (cl);
  coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM, "No method on effective_damage inst=%p, class_pass_idx = %d, class_slot_idx = %d\n", inst, cl ? cl->cl_index : -1, inst->every.cl->cl_index );
  return (coco_health) 0; /* convince compiler we don't fall off end */
}

void
coco_friendly_collision( coco_sobject * inst, coco_sobject_cl * cl, coco_sobject * obj2, coco_sstate * sstate, coco_bool * stat1, coco_bool * stat2 )
{
  do
   {
     void (* method)( coco_sobject *, coco_sobject_cl * , coco_sobject * , coco_sstate * , coco_bool * , coco_bool * ) = cl->friendly_collision;
     if (method)
       { method( inst, cl, obj2, sstate, stat1, stat2 ); return; }
     cl = cl->super;
   }
  while (cl);
  coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM, "No method on friendly_collision inst=%p, class_pass_idx = %d, class_slot_idx = %d\n", inst, cl ? cl->cl_index : -1, inst->every.cl->cl_index );
  return; /* convince compiler we don't fall off end */
}

void
coco_hostile_collision( coco_sobject * inst, coco_sobject_cl * cl, coco_sobject * obj2, coco_sstate * sstate, coco_bool * stat1, coco_bool * stat2 )
{
  do
   {
     void (* method)( coco_sobject *, coco_sobject_cl * , coco_sobject * , coco_sstate * , coco_bool * , coco_bool * ) = cl->hostile_collision;
     if (method)
       { method( inst, cl, obj2, sstate, stat1, stat2 ); return; }
     cl = cl->super;
   }
  while (cl);
  coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM, "No method on hostile_collision inst=%p, class_pass_idx = %d, class_slot_idx = %d\n", inst, cl ? cl->cl_index : -1, inst->every.cl->cl_index );
  return; /* convince compiler we don't fall off end */
}

void
coco_take_damage( coco_sobject * inst, coco_sobject_cl * cl, coco_health health, coco_sstate * sstate )
{
  do
   {
     void (* method)( coco_sobject *, coco_sobject_cl * , coco_health , coco_sstate * ) = cl->take_damage;
     if (method)
       { method( inst, cl, health, sstate ); return; }
     cl = cl->super;
   }
  while (cl);
  coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM, "No method on take_damage inst=%p, class_pass_idx = %d, class_slot_idx = %d\n", inst, cl ? cl->cl_index : -1, inst->every.cl->cl_index );
  return; /* convince compiler we don't fall off end */
}

void
coco_sobject_delete( coco_sobject * inst, coco_sobject_cl * cl, coco_sstate * sstate )
{
  do
   {
     void (* method)( coco_sobject *, coco_sobject_cl * , coco_sstate * ) = cl->sobject_delete;
     if (method)
       { method( inst, cl, sstate ); return; }
     cl = cl->super;
   }
  while (cl);
  coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM, "No method on sobject_delete inst=%p, class_pass_idx = %d, class_slot_idx = %d\n", inst, cl ? cl->cl_index : -1, inst->every.cl->cl_index );
  return; /* convince compiler we don't fall off end */
}

void
coco_kaboom( coco_sobject * inst, coco_sobject_cl * cl, coco_sstate * sstate )
{
  do
   {
     void (* method)( coco_sobject *, coco_sobject_cl * , coco_sstate * ) = cl->kaboom;
     if (method)
       { method( inst, cl, sstate ); return; }
     cl = cl->super;
   }
  while (cl);
  coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM, "No method on kaboom inst=%p, class_pass_idx = %d, class_slot_idx = %d\n", inst, cl ? cl->cl_index : -1, inst->every.cl->cl_index );
  return; /* convince compiler we don't fall off end */
}

coco_bool
coco_sobject_update_state( coco_sobject * inst, coco_sobject_cl * cl, coco_sstate * sstate )
{
  do
   {
     coco_bool (* method)( coco_sobject *, coco_sobject_cl * , coco_sstate * ) = cl->sobject_update_state;
     if (method)
       return method( inst, cl, sstate );
     cl = cl->super;
   }
  while (cl);
  coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM, "No method on sobject_update_state inst=%p, class_pass_idx = %d, class_slot_idx = %d\n", inst, cl ? cl->cl_index : -1, inst->every.cl->cl_index );
  return (coco_bool) 0; /* convince compiler we don't fall off end */
}

void
coco_sobject_unlink( coco_sobject * inst, coco_sobject_cl * cl, int link_index, coco_sstate * sstate )
{
  do
   {
     void (* method)( coco_sobject *, coco_sobject_cl * , int , coco_sstate * ) = cl->sobject_unlink;
     if (method)
       { method( inst, cl, link_index, sstate ); return; }
     cl = cl->super;
   }
  while (cl);
  coco_fatal_printf( COCO_ERROR_COCO_IMPLEMENTAION_PROBLEM, "No method on sobject_unlink inst=%p, class_pass_idx = %d, class_slot_idx = %d\n", inst, cl ? cl->cl_index : -1, inst->every.cl->cl_index );
  return; /* convince compiler we don't fall off end */
}

/* Instance Initializers */

coco_sobject *
coco_init_sobject( coco_sobject * inst, coco_sobject_cl * cl, coco_facing facing, coco_gnum x, coco_gnum y, coco_nationality nationality )
{
  inst->every.cl = cl;
  inst->every.camera_index = -1;
  inst->every.comp_index = -1;
  inst->every.facing = facing;
  inst->every.new_flag = COCO_TRUE;
  inst->every.x = x;
  inst->every.y = y;
  inst->every.w = (COCO_SOBJECT_RASTER_COUNT( inst )>0) ? COCO_SOBJECT_RASTERS( inst )[0]->w : 1;
  inst->every.h = (COCO_SOBJECT_RASTER_COUNT( inst )>0) ? COCO_SOBJECT_RASTERS( inst )[0]->h : 1;
  inst->every.dx = 0;
  inst->every.dy = 0;
  inst->every.locked_on_time = COCO_U32_MAX;
  inst->every.raster_index = 0;
  inst->every.nationality = nationality;
  inst->every.health = COCO_SOBJECT_HEALTH_MAX( inst );
  inst->every.angle = 0;
  inst->every.scratch_blip = COCO_BLIP_TRANSPARENT;
  return inst;
}

coco_sobject *
coco_init_sndobject( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_gnum y, coco_sound_type sound )
{
  coco_init_sobject( inst, cl, 0, x, y, COCO_NEUTRAL );
  inst->sndobject.sound = sound;
  return inst;
}

coco_sobject *
coco_init_camera( coco_sobject * inst, coco_sobject_cl * cl, signed char camera_index, coco_facing facing, coco_gnum x, coco_gnum y, coco_nationality nationality )
{
  coco_init_sobject( inst, cl, facing, x, y, nationality );
  inst->camera.camera_index = camera_index;
  return inst;
}

coco_sobject *
coco_init_inviso_camera( coco_sobject * inst, coco_sobject_cl * cl, signed char camera_index, coco_gnum x, coco_nationality nationality )
{
  coco_init_camera( inst, cl, camera_index, 0, x, COCO_GROUND_Y + 1, nationality );
  return inst;
}

coco_sobject *
coco_init_copter( coco_sobject * inst, coco_sobject_cl * cl, signed char camera_index, coco_facing facing, coco_gnum x, coco_gnum y, coco_nationality nationality )
{
  coco_init_camera( inst, cl, camera_index, facing, x, y, nationality );
  inst->copter.bullets = COCO_SOBJECT_MAX_BULLETS( inst );
  inst->copter.bombs = COCO_SOBJECT_MAX_BOMBS( inst );
  inst->copter.missiles = COCO_SOBJECT_MAX_MISSILES( inst );
  inst->copter.rockets = COCO_SOBJECT_MAX_ROCKETS( inst );
  inst->copter.paratroopers = 0;
  inst->copter.engitroopers = 0;
  inst->copter.fuel = COCO_SOBJECT_MAX_FUEL(inst);
  inst->copter.missile_cooldown = COCO_FALSE;
  inst->copter.next_missilebeat_on_pad = 0;
  inst->copter.raster_index = ((nationality==COCO_BAD_GUYS)<<1)+(facing>0);
  inst->copter.last_bomb_drop_time = 0;
  inst->copter.last_rocket_launch_time = 0;
  inst->copter.landed_p = COCO_FALSE;
  inst->copter.landed_on_pad_p = COCO_FALSE;
  inst->copter.angle = (facing>0) ? 0 : 64;
  inst->copter.w = COCO_SOBJECT_CANONICAL_W( inst );
  inst->copter.h = COCO_SOBJECT_CANONICAL_H( inst );
  return inst;
}

coco_sobject *
coco_init_cobra( coco_sobject * inst, coco_sobject_cl * cl, signed char camera_index, coco_facing facing, coco_gnum x, coco_gnum y, coco_nationality nationality )
{
  coco_init_copter( inst, cl, camera_index, facing, x, y, nationality );
  return inst;
}

coco_sobject *
coco_init_bubble_copter( coco_sobject * inst, coco_sobject_cl * cl, signed char camera_index, coco_facing facing, coco_gnum x, coco_gnum y, coco_nationality nationality )
{
  coco_init_copter( inst, cl, camera_index, facing, x, y, nationality );
  return inst;
}

coco_sobject *
coco_init_smoke( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_gnum y, unsigned char raster_index )
{
  coco_init_sobject( inst, cl, 0, x, y, COCO_NEUTRAL );
  inst->smoke.raster_index = raster_index;
  return inst;
}

coco_sobject *
coco_init_cloud( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_gnum y )
{
  coco_init_sobject( inst, cl, 0, x, y, COCO_NEUTRAL );
  return inst;
}

coco_sobject *
coco_init_balloon_part( coco_sobject * inst, coco_sobject_cl * cl, coco_facing facing, coco_gnum x, coco_gnum y, coco_nationality nationality )
{
  coco_init_sobject( inst, cl, facing, x, y, nationality );
  return inst;
}

coco_sobject *
coco_init_balloon( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_gnum y, unsigned char raster_index, coco_nationality nationality )
{
  coco_init_balloon_part( inst, cl, 0, x, y, nationality );
  inst->balloon.line_index = -1;
  inst->balloon.balloon_time_phase = rand();
  inst->balloon.raster_index = raster_index;
  return inst;
}

coco_sobject *
coco_init_balloon_line( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_gnum y, coco_gnum h, coco_nationality nationality, int hut_index, int balloon_index )
{
  coco_init_balloon_part( inst, cl, 0, x, y, nationality );
  inst->balloon_line.hut_index = hut_index;
  inst->balloon_line.balloon_index = balloon_index;
  inst->balloon_line.w = 1;
  inst->balloon_line.h = h;
  return inst;
}

coco_sobject *
coco_init_building( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_nationality nationality )
{
  coco_init_sobject( inst, cl, 0, x, COCO_GROUND_Y, nationality );
  return inst;
}

coco_sobject *
coco_init_traffic_sign( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, unsigned char raster_index )
{
  coco_init_building( inst, cl, x, COCO_NEUTRAL );
  inst->traffic_sign.raster_index = raster_index;
  inst->traffic_sign.raster_index = raster_index;
  return inst;
}

coco_sobject *
coco_init_hut( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_nationality nationality )
{
  coco_init_building( inst, cl, x, nationality );
  inst->hut.line_index = -1;
  inst->hut.points_till_reincarnation = COCO_HUT_BALLOON_REINCARNATION_FRAMES;
  inst->hut.raster_index = nationality;
  return inst;
}

coco_sobject *
coco_init_bunker( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_nationality nationality, unsigned char defenders )
{
  coco_init_building( inst, cl, x, nationality );
  inst->bunker.defenders = defenders;
  inst->bunker.raster_index = nationality - COCO_GOOD_GUYS;
  return inst;
}

coco_sobject *
coco_init_pillbox( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_nationality nationality, unsigned char defenders, int headquarters_index )
{
  coco_init_bunker( inst, cl, x, nationality, defenders );
  inst->pillbox.headquarters_index = headquarters_index;
  inst->pillbox.raster_index = 0;
  return inst;
}

coco_sobject *
coco_init_landing_pad( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x )
{
  coco_init_building( inst, cl, x, COCO_NEUTRAL );
  return inst;
}

coco_sobject *
coco_init_headquarters( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_nationality nationality, int pillbox_index )
{
  coco_init_building( inst, cl, x, nationality );
  inst->headquarters.facing = (nationality == COCO_GOOD_GUYS) ? -1 : 1;
  inst->headquarters.smoke_countdown = 0;
  inst->headquarters.pillbox_index = pillbox_index;
  inst->headquarters.raster_index = nationality - COCO_GOOD_GUYS;
  return inst;
}

coco_sobject *
coco_init_gun( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_nationality nationality )
{
  coco_init_building( inst, cl, x, nationality );
  inst->gun.cooldown = 0;
  inst->gun.angle = 32;
  inst->gun.raster_index = nationality - COCO_GOOD_GUYS;
  return inst;
}

coco_sobject *
coco_init_aa_gun( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_nationality nationality )
{
  coco_init_gun( inst, cl, x, nationality );
  return inst;
}

coco_sobject *
coco_init_at_gun( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_nationality nationality )
{
  coco_init_gun( inst, cl, x, nationality );
  inst->at_gun.angle = (COCO_GOOD_GUYS == COCO_SOBJECT_NATIONALITY( inst )) ? 128 : 0;
  return inst;
}

coco_sobject *
coco_init_gun_rubble( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, unsigned char reincarnation_class_index )
{
  coco_init_building( inst, cl, x, COCO_NEUTRAL );
  inst->gun_rubble.reincarnation_class_index = reincarnation_class_index;
  inst->gun_rubble.points_till_reincarnation = COCO_IDEAL_FRAMES_PER_SECOND<<4;
  inst->gun_rubble.reincarnation_nationality = COCO_NEUTRAL;
  return inst;
}

coco_sobject *
coco_init_ground_unit( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_nationality nationality )
{
  coco_init_sobject( inst, cl, (COCO_GOOD_GUYS == nationality) ? -1 : 1, x, COCO_GROUND_Y, nationality );
  inst->ground_unit.dx = (COCO_GOOD_GUYS == nationality) ? -1 : 1;
  return inst;
}

coco_sobject *
coco_init_ground_vehicle( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_nationality nationality )
{
  coco_init_ground_unit( inst, cl, x, nationality );
  inst->ground_vehicle.raster_index = nationality - COCO_GOOD_GUYS;
  return inst;
}

coco_sobject *
coco_init_demo_van( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_nationality nationality )
{
  coco_init_ground_vehicle( inst, cl, x, nationality );
  inst->demo_van.raster_index = ((nationality - COCO_GOOD_GUYS)<<1) + (0==(rand()&127));
  return inst;
}

coco_sobject *
coco_init_tank( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_nationality nationality )
{
  coco_init_ground_vehicle( inst, cl, x, nationality );
  inst->tank.angle = (COCO_SOBJECT_FACING( inst )>0) ? 6 : 122;
  inst->tank.raster_index = nationality - COCO_GOOD_GUYS;
  return inst;
}

coco_sobject *
coco_init_firetruck( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_nationality nationality )
{
  coco_init_ground_vehicle( inst, cl, x, nationality );
  inst->firetruck.raster_index = (nationality - COCO_GOOD_GUYS) << 1;
  inst->firetruck.smoke_countdown = 0;
  return inst;
}

coco_sobject *
coco_init_infantry( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_gnum y, coco_gnum dx, coco_gnum dy, unsigned char raster_index, coco_nationality nationality )
{
  coco_init_ground_unit( inst, cl, x, nationality );
  inst->infantry.y = y;
  inst->infantry.dx = dx;
  inst->infantry.dy = dy;
  inst->infantry.raster_index = raster_index;
  inst->infantry.parachutes = 2;
  inst->infantry.parachute_height = COCO_SOBJECT_Y( inst ) - (rand()&127);
  return inst;
}

coco_sobject *
coco_init_engineer( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_gnum y, coco_gnum dx, coco_gnum dy, unsigned char raster_index, coco_nationality nationality )
{
  coco_init_infantry( inst, cl, x, y, dx, dy, raster_index, nationality );
  return inst;
}

coco_sobject *
coco_init_munition( coco_sobject * inst, coco_sobject_cl * cl, coco_facing facing, coco_gnum x, coco_gnum y, coco_gnum dx, coco_gnum dy, coco_nationality nationality )
{
  coco_init_sobject( inst, cl, facing, x, y, nationality );
  inst->munition.dx = dx;
  inst->munition.dy = dy;
  return inst;
}

coco_sobject *
coco_init_bullet( coco_sobject * inst, coco_sobject_cl * cl, coco_facing facing, coco_gnum x, coco_gnum y, coco_gnum dx, coco_gnum dy, coco_nationality nationality, coco_u32 expiration_date )
{
  coco_init_munition( inst, cl, facing, x, y, dx, dy, nationality );
  inst->bullet.expiration_date = expiration_date;
  return inst;
}

coco_sobject *
coco_init_rifle_bullet( coco_sobject * inst, coco_sobject_cl * cl, coco_facing facing, coco_gnum x, coco_gnum y, coco_gnum dx, coco_gnum dy, coco_nationality nationality, coco_u32 expiration_date )
{
  coco_init_bullet( inst, cl, facing, x, y, dx, dy, nationality, expiration_date );
  return inst;
}

coco_sobject *
coco_init_tank_bullet( coco_sobject * inst, coco_sobject_cl * cl, coco_facing facing, coco_gnum x, coco_gnum y, coco_gnum dx, coco_gnum dy, coco_nationality nationality, coco_u32 expiration_date )
{
  coco_init_bullet( inst, cl, facing, x, y, dx, dy, nationality, expiration_date );
  return inst;
}

coco_sobject *
coco_init_bomb( coco_sobject * inst, coco_sobject_cl * cl, coco_facing facing, coco_gnum x, coco_gnum y, coco_gnum dx, coco_gnum dy, coco_nationality nationality )
{
  coco_init_munition( inst, cl, facing, x, y, dx, dy, nationality );
  inst->bomb.horiz_ticks = 0;
  inst->bomb.raster_index = (COCO_SOBJECT_FACING( inst )>0) ? 4 : 0;
  return inst;
}

coco_sobject *
coco_init_abstract_missile( coco_sobject * inst, coco_sobject_cl * cl, coco_facing facing, coco_gnum x, coco_gnum y, coco_gnum dx, coco_gnum dy, coco_nationality nationality, coco_angle angle )
{
  coco_init_munition( inst, cl, facing, x, y, dx, dy, nationality );
  inst->abstract_missile.angle = angle;
  inst->abstract_missile.w = 11;
  inst->abstract_missile.h = 11;
  return inst;
}

coco_sobject *
coco_init_rocket( coco_sobject * inst, coco_sobject_cl * cl, coco_facing facing, coco_gnum x, coco_gnum y, coco_gnum dx, coco_gnum dy, coco_nationality nationality, coco_angle angle )
{
  coco_init_abstract_missile( inst, cl, facing, x, y, dx, dy, nationality, angle );
  return inst;
}

coco_sobject *
coco_init_missile( coco_sobject * inst, coco_sobject_cl * cl, coco_facing facing, coco_gnum x, coco_gnum y, coco_gnum dx, coco_gnum dy, coco_nationality nationality, coco_angle angle, int target_index )
{
  coco_init_abstract_missile( inst, cl, facing, x, y, dx, dy, nationality, angle );
  inst->missile.fuel = 5*COCO_IDEAL_FRAMES_PER_SECOND;
  inst->missile.target_index = target_index;
  return inst;
}

coco_sobject *
coco_init_spark( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_gnum y, signed char spark_dx, signed char spark_dy )
{
  coco_init_munition( inst, cl, 0, x, y, 0, 0, COCO_NEUTRAL );
  inst->spark.w = 1;
  inst->spark.h = 1;
  inst->spark.spark_dx = spark_dx;
  inst->spark.spark_dy = spark_dy;
  return inst;
}

coco_sobject *
coco_init_shrapnel( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_gnum y, coco_gnum dx, coco_gnum dy )
{
  coco_init_munition( inst, cl, 0, x, y, dx, dy, COCO_NEUTRAL );
  return inst;
}

coco_sobject *
coco_init_metal_shrapnel( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_gnum y, coco_gnum dx, coco_gnum dy )
{
  coco_init_shrapnel( inst, cl, x, y, dx, dy );
  inst->metal_shrapnel.color_index = COCO_INDEX_RED;
  return inst;
}

coco_sobject *
coco_init_big_metal_shrapnel( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_gnum y, coco_gnum dx, coco_gnum dy )
{
  coco_init_metal_shrapnel( inst, cl, x, y, dx, dy );
  return inst;
}

coco_sobject *
coco_init_med_metal_shrapnel( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_gnum y, coco_gnum dx, coco_gnum dy )
{
  coco_init_metal_shrapnel( inst, cl, x, y, dx, dy );
  return inst;
}

coco_sobject *
coco_init_small_metal_shrapnel( coco_sobject * inst, coco_sobject_cl * cl, coco_gnum x, coco_gnum y, coco_gnum dx, coco_gnum dy )
{
  coco_init_metal_shrapnel( inst, cl, x, y, dx, dy );
  return inst;
}

coco_sobject *
coco_init_explosion( coco_sobject * inst, coco_sobject_cl * cl, coco_facing facing, coco_gnum x, coco_gnum y, coco_gnum dx, coco_gnum dy, unsigned char raster_index )
{
  coco_init_munition( inst, cl, facing, x, y, dx, dy, COCO_NEUTRAL );
  inst->explosion.raster_index = raster_index;
  inst->explosion.health = COCO_NIGH_INVULNERABLE;
  return inst;
}

/* Init-by-list function defaults */

coco_sobject *coco_cloud_automatic_init_by_list( coco_sobject *inst, coco_sobject_cl *cla, coco_lnum *arguments )
{
  return coco_init_cloud( inst, cla, arguments[0], arguments[1] );
}
coco_sobject *coco_balloon_automatic_init_by_list( coco_sobject *inst, coco_sobject_cl *cla, coco_lnum *arguments )
{
  return coco_init_balloon( inst, cla, arguments[0], arguments[1], arguments[2], arguments[3] );
}
coco_sobject *coco_balloon_line_automatic_init_by_list( coco_sobject *inst, coco_sobject_cl *cla, coco_lnum *arguments )
{
  return coco_init_balloon_line( inst, cla, arguments[0], arguments[1], arguments[2], arguments[3], arguments[4], arguments[5] );
}
coco_sobject *coco_traffic_sign_automatic_init_by_list( coco_sobject *inst, coco_sobject_cl *cla, coco_lnum *arguments )
{
  return coco_init_traffic_sign( inst, cla, arguments[0], arguments[1] );
}
coco_sobject *coco_bunker_automatic_init_by_list( coco_sobject *inst, coco_sobject_cl *cla, coco_lnum *arguments )
{
  return coco_init_bunker( inst, cla, arguments[0], arguments[1], arguments[2] );
}
coco_sobject *coco_pillbox_automatic_init_by_list( coco_sobject *inst, coco_sobject_cl *cla, coco_lnum *arguments )
{
  return coco_init_pillbox( inst, cla, arguments[0], arguments[1], arguments[2], arguments[3] );
}
coco_sobject *coco_landing_pad_automatic_init_by_list( coco_sobject *inst, coco_sobject_cl *cla, coco_lnum *arguments )
{
  return coco_init_landing_pad( inst, cla, arguments[0] );
}
coco_sobject *coco_headquarters_automatic_init_by_list( coco_sobject *inst, coco_sobject_cl *cla, coco_lnum *arguments )
{
  return coco_init_headquarters( inst, cla, arguments[0], arguments[1], arguments[2] );
}
coco_sobject *coco_aa_gun_automatic_init_by_list( coco_sobject *inst, coco_sobject_cl *cla, coco_lnum *arguments )
{
  return coco_init_aa_gun( inst, cla, arguments[0], arguments[1] );
}
coco_sobject *coco_at_gun_automatic_init_by_list( coco_sobject *inst, coco_sobject_cl *cla, coco_lnum *arguments )
{
  return coco_init_at_gun( inst, cla, arguments[0], arguments[1] );
}
coco_sobject *coco_gun_rubble_automatic_init_by_list( coco_sobject *inst, coco_sobject_cl *cla, coco_lnum *arguments )
{
  return coco_init_gun_rubble( inst, cla, arguments[0], arguments[1] );
}
coco_sobject *coco_ground_vehicle_automatic_init_by_list( coco_sobject *inst, coco_sobject_cl *cla, coco_lnum *arguments )
{
  return coco_init_ground_vehicle( inst, cla, arguments[0], arguments[1] );
}
coco_sobject *coco_demo_van_automatic_init_by_list( coco_sobject *inst, coco_sobject_cl *cla, coco_lnum *arguments )
{
  return coco_init_demo_van( inst, cla, arguments[0], arguments[1] );
}
coco_sobject *coco_firetruck_automatic_init_by_list( coco_sobject *inst, coco_sobject_cl *cla, coco_lnum *arguments )
{
  return coco_init_firetruck( inst, cla, arguments[0], arguments[1] );
}
