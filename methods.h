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

/* sobject method definitions go here */

#ifdef COCO_INIT_HOOK /* an old, bad idea */
void coco_finalize_sobject( coco_sobject *sobject, coco_sobject_cl *cl );
#endif

coco_bool
coco_sndobject_update_state( coco_sobject *sobject,
                             coco_sobject_cl *cl,
                             coco_sstate *sstate );


coco_bool coco_spark_update_state( coco_sobject *sobject, 
                                   coco_sobject_cl *cl, 
                                   coco_sstate *sstate );

coco_bool coco_ground_unit_update_state( coco_sobject *sobject, 
                                         coco_sobject_cl *cl, 
                                         coco_sstate *sstate );

coco_bool
coco_explosion_update_state( coco_sobject *sobject,
                             coco_sobject_cl *cl,
                             coco_sstate *sstate );

coco_bool
coco_bunker_update_state( coco_sobject *sobject,
                          coco_sobject_cl *cl,
                          coco_sstate *sstate );

coco_bool
coco_balloon_factory_update_state( coco_sobject *sobject,
                                   coco_sobject_cl *cl,
                                   coco_sstate *sstate );


coco_bool
coco_hut_update_state( coco_sobject *sobject, coco_sobject_cl *cl, coco_sstate *sstate );

coco_bool
coco_gun_update_state( coco_sobject *sobject, coco_sobject_cl *cl, coco_sstate *sstate );



coco_bool
coco_balloon_update_state( coco_sobject *sobject,
                           coco_sobject_cl *cl,
                           coco_sstate *sstate );

coco_bool
coco_headquarters_update_state( coco_sobject *sobject,
                                coco_sobject_cl *cl,
                                coco_sstate *sstate );


coco_bool
coco_balloon_line_update_state( coco_sobject *line,
                                coco_sobject_cl *cl,
                                coco_sstate *sstate );

coco_bool
coco_engineer_update_state( coco_sobject *sobject,
                            coco_sobject_cl *cl,
                            coco_sstate *sstate );

coco_bool
coco_gun_rubble_update_state( coco_sobject *sobject,
                              coco_sobject_cl *cl,
                              coco_sstate *sstate );

coco_bool
coco_firetruck_update_state( coco_sobject *sobject,
                             coco_sobject_cl *cl,
                             coco_sstate *sstate );

coco_bool
coco_missile_update_state( coco_sobject *sobject,
                          coco_sobject_cl *cl,
                          coco_sstate *sstate );

coco_bool
coco_rocket_update_state( coco_sobject *sobject,
                          coco_sobject_cl *cl,
                          coco_sstate *sstate );


coco_bool 
coco_inviso_camera_update_state( coco_sobject *sobject, 
                                 coco_sobject_cl *cl, 
                                 coco_sstate *sstate );

coco_bool coco_every_sobject_update_state( coco_sobject *sobject, 
                                           coco_sobject_cl *cl, 
                                           coco_sstate *sstate );

coco_bool coco_infantry_update_state( coco_sobject *sobject, 
                                      coco_sobject_cl *cl, 
                                      coco_sstate *sstate );

coco_bool
coco_tank_update_state( coco_sobject *sobject,
                        coco_sobject_cl *cl,
                        coco_sstate *sstate );


coco_bool
coco_shrapnel_update_state( coco_sobject *sobject,
                            coco_sobject_cl *cl,
                            coco_sstate *sstate );

coco_bool
coco_metal_shrapnel_update_state( coco_sobject *sobject,
                                  coco_sobject_cl *cl,
                                  coco_sstate *sstate );


coco_bool coco_smoke_update_state( coco_sobject *sobject, 
                                   coco_sobject_cl *cl, 
                                   coco_sstate *sstate );

coco_bool coco_cloud_update_state( coco_sobject *sobject, 
                                   coco_sobject_cl *cl, 
                                   coco_sstate *sstate );

/* moved to server.c because its so special: */
coco_bool coco_copter_update_state( coco_sobject *sobject, 
                                    coco_sobject_cl *cl, 
                                    coco_sstate *sstate );
coco_bool coco_camera_update_state( coco_sobject *sobject, 
                                    coco_sobject_cl *cl, 
                                    coco_sstate *sstate );



coco_bool coco_bullet_update_state( coco_sobject *sobject, 
                                    coco_sobject_cl *cl, 
                                    coco_sstate *sstate );
coco_bool coco_aa_gun_update_state( coco_sobject *sobject, 
                                    coco_sobject_cl *cl, 
                                    coco_sstate *sstate );
coco_bool coco_bomb_update_state( coco_sobject *sobject, 
                                  coco_sobject_cl *cl, 
                                  coco_sstate *sstate );


void coco_every_sobject_kaboom( coco_sobject *sobject,
                                coco_sobject_cl *cl,
                                coco_sstate *sstate );


void coco_copter_kaboom( coco_sobject *sobject,
                         coco_sobject_cl *cl,
                         coco_sstate *sstate );


void coco_airbursting_kaboom( coco_sobject *sobject,
                              coco_sobject_cl *cl,
                              coco_sstate *sstate );


void coco_bullet_kaboom( coco_sobject *sobject,
                         coco_sobject_cl *cl,
                         coco_sstate *sstate );

void coco_gun_kaboom( coco_sobject *sobject,
                      coco_sobject_cl *cl,
                      coco_sstate *sstate );


void coco_bomb_kaboom( coco_sobject *sobject,
                       coco_sobject_cl *cl,
                       coco_sstate *sstate );

void coco_rocket_kaboom( coco_sobject *sobject,
                         coco_sobject_cl *cl,
                         coco_sstate *sstate );

void
coco_infantry_kaboom( coco_sobject *sobject,
                      coco_sobject_cl *cl,
                      coco_sstate *sstate );


void coco_every_sobject_delete( coco_sobject *sobject,
                                coco_sobject_cl *cl,
                                coco_sstate *sstate );

void coco_copter_delete( coco_sobject *sobject,
                         coco_sobject_cl *cl,
                         coco_sstate *sstate );


void 
coco_sndobject_to_cobject( coco_sobject *sobject, coco_sobject_cl *cl,
                           coco_cobject *cobject, coco_gnum cam_left,
                           coco_bool nbo );


void 
coco_spark_to_cobject( coco_sobject *sobject, coco_sobject_cl *cl,
                       coco_cobject *cobject, coco_gnum cam_left,
                       coco_bool nbo );

void 
coco_gun_tank_to_cobject( coco_sobject *sobject, coco_sobject_cl *cl,
                          coco_cobject *cobject, coco_gnum cam_left,
                          coco_bool nbo );


void 
coco_balloon_line_to_cobject( coco_sobject *sobject, coco_sobject_cl *cl,
                              coco_cobject *cobject, coco_gnum cam_left,
                              coco_bool nbo );


void
coco_y_angle_to_cobject( coco_sobject *sobject, coco_sobject_cl *cl,
                         coco_cobject *cobject, coco_gnum cam_left,
                         coco_bool nbo );


void coco_rasteroid_to_cobject( coco_sobject *sobject, coco_sobject_cl *cl,
                                coco_cobject *cobject,
                                coco_gnum cam_left, coco_bool nbo  );

void coco_ground_rasteroid_to_cobject( coco_sobject *sobject,
                                       coco_sobject_cl *cl,
                                       coco_cobject *cobject,
                                       coco_gnum cam_left,
                                       coco_bool nbo );

void coco_metal_shrapnel_to_cobject( coco_sobject *sobject,
                                     coco_sobject_cl *cl,
                                     coco_cobject *cobject,
                                     coco_gnum cam_left,
                                     coco_bool nbo );


unsigned char coco_every_sobject_raster_flip_index( coco_sobject *sobject,
                                                    coco_sobject_cl *cl,
                                                    unsigned char index );

unsigned char coco_copter_raster_flip_index( coco_sobject *copter,
                                             coco_sobject_cl *cl,
                                             unsigned char index );

void coco_every_sobject_toggle_facing( coco_sobject *sobject,
                                       coco_sobject_cl *cl );
void coco_copter_toggle_facing( coco_sobject *sobject, coco_sobject_cl *cl );



void
coco_gun_fire_bullet_coords( coco_sobject *gun, coco_sobject_cl *cl,
                             coco_sobject *target, 
                             coco_gnum *dx_return, coco_gnum *dy_return,
                             coco_gnum *x_return, coco_gnum *y_return );
void
coco_tank_infantry_fire_bullet_coords( coco_sobject *gun, coco_sobject_cl *cl,
                              coco_sobject *target, 
                              coco_gnum *dx_return, coco_gnum *dy_return,
                              coco_gnum *x_return, coco_gnum *y_return );

void
coco_bunker_fire_bullet_coords( coco_sobject *gun, coco_sobject_cl *cl,
                                coco_sobject *target, 
                                coco_gnum *dx_return, coco_gnum *dy_return,
                                coco_gnum *x_return, coco_gnum *y_return );


void
coco_copter_fire_bullet_coords( coco_sobject *copter, coco_sobject_cl *cl,
                                coco_sobject *target_ignore, 
                                coco_gnum *dx_return, coco_gnum *dy_return,
                                coco_gnum *x_return, coco_gnum *y_return);

coco_sobject *
coco_every_sobject_fire_bullet( coco_sobject *sobject, coco_sobject_cl *cl,
                                coco_sstate *sstate, coco_sobject *target_or_null );

coco_sobject *coco_copter_fire_bullet( coco_sobject *copter,
                                       coco_sobject_cl *cl,
                                       coco_sstate *sstate,
                                       coco_sobject *target_or_null );


coco_gnum coco_every_camera_velocity_offset( coco_sobject *camera, 
                                             coco_sobject_cl *cla );
coco_gnum coco_copter_velocity_offset( coco_sobject *camera, 
                                       coco_sobject_cl *cla );

void sobject_ignore_neutral_buiding( coco_sobject *unit, coco_sobject_cl *cla,
                                     coco_sobject *building );


coco_sobject *coco_every_sobject_drop_paratrooper( coco_sobject *sobject,
                                                   coco_sobject_cl *cla,
                                                   coco_sstate *sstate );

coco_sobject *coco_copter_drop_paratrooper( coco_sobject *sobject,
                                            coco_sobject_cl *cla,
                                            coco_sstate *sstate );

coco_sobject *coco_inviso_camera_drop_paratrooper( coco_sobject *sobject,
                                                   coco_sobject_cl *cla,
                                                   coco_sstate *sstate );

coco_health coco_flesh_damageable_effective_damage( coco_sobject *damagee,
                                                   coco_sobject_cl *damagee_cl,
                                                   coco_sobject *damager );

coco_health coco_infantry_effective_damage( coco_sobject *damagee,
                                            coco_sobject_cl *
                                            damagee_cl,
                                            coco_sobject *damager );

coco_health coco_heavy_armor_effective_damage( coco_sobject *damagee,
                                               coco_sobject_cl *damagee_cl,
                                               coco_sobject *damager );


coco_health coco_light_armor_effective_damage( coco_sobject *damagee,
                                               coco_sobject_cl *damagee_cl,
                                               coco_sobject *damager );

coco_health coco_unarmored_effective_damage( coco_sobject *damagee,
                                             coco_sobject_cl *damagee_cl,
                                             coco_sobject *damager );

coco_bool
coco_engineer_enter_hut( coco_sobject *engie, coco_sobject_cl *cla,
                         coco_sobject *hut, coco_sstate *sstate,
                         coco_bool *stand_still_p );

coco_bool
coco_feisty_infantry_enter_hut( coco_sobject *infantry, coco_sobject_cl *cla,
                                coco_sobject *hut, coco_sstate *sstate,
                                coco_bool *stand_still_p );


void coco_copter_ground_collide( coco_sobject *sobject, coco_sobject_cl *cla,
                                 coco_sstate *sstate );

void coco_ignore_ground_collide( coco_sobject *sobject, coco_sobject_cl *cla,
                                 coco_sstate *sstate );


void coco_every_sobject_ground_collide( coco_sobject *sobject,
                                        coco_sobject_cl *cla,
                                        coco_sstate *sstate );


void coco_every_sobject_hostile_collision( coco_sobject *obj1,
                                           coco_sobject_cl *obj1cl,
                                           coco_sobject *obj2, 
                                           coco_sstate *sstate,
                                           coco_bool *stat1,
                                           coco_bool *stat2 );

void
coco_building_hostile_collision( coco_sobject *building,
                                 coco_sobject_cl *building_cl,
                                 coco_sobject *obj2, 
                                 coco_sstate *sstate,
                                 coco_bool *stat1, coco_bool *stat2 );

void coco_bullet_hostile_collision( coco_sobject *obj1,
                                    coco_sobject_cl *obj1cl,
                                    coco_sobject *obj2, 
                                    coco_sstate *sstate,
                                    coco_bool *stat1, coco_bool *stat2 );

void coco_munition_hostile_collision( coco_sobject *obj1,
                                      coco_sobject_cl *obj1cl,
                                      coco_sobject *obj2, 
                                      coco_sstate *sstate,
                                      coco_bool *stat1, coco_bool *stat2 );

void coco_bunker_hostile_collision( coco_sobject *bunker,
                                    coco_sobject_cl *bunker_cl,
                                    coco_sobject *obj2, 
                                    coco_sstate *sstate,
                                    coco_bool *stat1, coco_bool *stat2 );


void coco_balloon_part_hostile_collision( coco_sobject *obj1,
                                          coco_sobject_cl *obj1cl,
                                          coco_sobject *obj2, 
                                          coco_sstate *sstate,
                                          coco_bool *stat1,
                                          coco_bool *stat2 );

void coco_demo_van_hostile_collision( coco_sobject *obj1,
                                      coco_sobject_cl *obj1cl,
                                      coco_sobject *obj2, 
                                      coco_sstate *sstate,
                                      coco_bool *stat1, coco_bool *stat2 );

void coco_ground_unit_hostile_collision( coco_sobject *obj1,
                                         coco_sobject_cl *obj1cl,
                                         coco_sobject *obj2, 
                                         coco_sstate *sstate,
                                         coco_bool *stat1, coco_bool *stat2 );

void coco_copter_hostile_collision( coco_sobject *obj1,
                                    coco_sobject_cl *obj1cl,
                                    coco_sobject *obj2, 
                                    coco_sstate *sstate,
                                    coco_bool *stat1,
                                    coco_bool *stat2 );

void coco_every_sobject_friendly_collision( coco_sobject *obj1,
                                            coco_sobject_cl *obj1cl,
                                            coco_sobject *obj2, 
                                            coco_sstate *sstate,
                                            coco_bool *stat1,
                                            coco_bool *stat2 );

void coco_copter_friendly_collision( coco_sobject *obj1,
                                     coco_sobject_cl *obj1cl,
                                     coco_sobject *obj2, 
                                     coco_sstate *sstate,
                                     coco_bool *stat1, coco_bool *stat2 );

void coco_every_sobject_unlink( coco_sobject *sobject, coco_sobject_cl *cla,
                                int unlink_idx, coco_sstate *sstate );

void coco_missile_unlink( coco_sobject *sobject, coco_sobject_cl *cla,
                         int unlink_idx, coco_sstate *sstate );

void coco_balloon_unlink( coco_sobject *sobject, coco_sobject_cl *cla,
                          int unlink_idx, coco_sstate *sstate );


void coco_balloon_line_unlink( coco_sobject *sobject, coco_sobject_cl *cla,
                               int unlink_idx, coco_sstate *sstate );


void coco_hut_unlink( coco_sobject *sobject, coco_sobject_cl *cla,
                      int unlink_idx, coco_sstate *sstate );


void coco_pillbox_unlink( coco_sobject *sobject, coco_sobject_cl *cla,
                          int unlink_idx, coco_sstate *sstate );


void coco_headquarters_unlink( coco_sobject *sobject, coco_sobject_cl *cla,
                               int unlink_idx, coco_sstate *sstate );


coco_bool coco_trooper_never_target_me( coco_sobject *sobject, 
                                         coco_sobject_cl *cla,
                                         coco_sobject *targeter );

coco_bool coco_trooper_always_target_me( coco_sobject *sobject, 
                                         coco_sobject_cl *cla,
                                         coco_sobject *targeter );

coco_bool coco_trooper_bunker_target_p( coco_sobject *sobject, 
                                        coco_sobject_cl *cla,
                                        coco_sobject *targeter );



coco_sobject *
coco_every_sobject_launch_missile( coco_sobject *sobject, coco_sobject_cl *cla,
                                  coco_sstate *sstate, coco_sobject *target );

coco_sobject *
coco_copter_launch_missile( coco_sobject *sobject, coco_sobject_cl *cla,
                           coco_sstate *sstate, coco_sobject *target );

coco_sobject *
coco_firetruck_launch_missile( coco_sobject *sobject, coco_sobject_cl *cla,
                              coco_sstate *sstate, coco_sobject *target );

coco_sobject *
coco_headquarters_launch_missile( coco_sobject *sobject, coco_sobject_cl *cla,
                                 coco_sstate *sstate, coco_sobject *target );


void
coco_infantry_enter_friendly_bunker( coco_sobject *sobject,
                                     coco_sobject_cl *cl,
                                     coco_sobject *bunker,
                                     coco_sstate *sstate );


void
coco_ignore_bunker( coco_sobject *sobject, coco_sobject_cl *cl,
                    coco_sobject *bunker, coco_sstate *sstate );


coco_blip_index
coco_every_sobject_blip_color_index( coco_sobject *sobject, 
                                     coco_sobject_cl *cl,
                                     int p_or_c_index,
                                     coco_bool is_comp,
                                     coco_sstate *sstate );

coco_blip_index
coco_camera_blip_color_index( coco_sobject *sobject, 
                              coco_sobject_cl *cl,
                              int p_or_c_index,
                              coco_bool is_comp,
                              coco_sstate *sstate );

coco_blip_index
coco_copter_blip_color_index( coco_sobject *sobject, 
                              coco_sobject_cl *cl,
                              int p_or_c_index,
                              coco_bool is_comp,
                              coco_sstate *sstate );


coco_bool
coco_every_sobject_is_visible( coco_sobject *sobject,
                               coco_sobject_cl *cl,
                               coco_gnum cam_left,
                               coco_gnum cam_right,
                               coco_sobject *cam_or_null );

coco_bool
coco_sndobject_is_visible( coco_sobject *sobject,
                           coco_sobject_cl *cl,
                           coco_gnum cam_left,
                           coco_gnum cam_right,
                           coco_sobject *cam_or_null );


/* pseudo methods related to cobjects, probably implemented elsewhere than
   methods.c, e.g. server.c */
void coco_cobject_rasteroid_draw( coco_cobject *cobject, 
                                  coco_sobject_cl *cla );

void coco_cobject_barreloid_draw( coco_cobject *cobject, 
                                  coco_sobject_cl *cla );
void coco_metal_shrapnel_draw( coco_cobject *cobject, coco_sobject_cl *cla );

void
coco_balloon_line_draw( coco_cobject *cobject, coco_sobject_cl *cla );

void
coco_balloon_factory_draw( coco_cobject *cobject, coco_sobject_cl *cla );


void coco_cobject_y_ntoh( coco_cobject *cobject );

void coco_copter_draw( coco_cobject *cobject, coco_sobject_cl *cla );

void coco_bullet_draw( coco_cobject *cobject, coco_sobject_cl *cla );

void coco_cobject_inviso_draw( coco_cobject *cobject, coco_sobject_cl *cla );

void coco_cobject_sound_draw( coco_cobject *cobject, coco_sobject_cl *cla );

void coco_missile_draw( coco_cobject *cobject, coco_sobject_cl *cla );

void coco_spark_draw( coco_cobject *cobject, coco_sobject_cl *cla );

/* things that might be promoted to generic functions eventually */

coco_sobject *coco_drop_bomb( coco_sobject *copter, coco_sobject_cl *cl,
                              coco_sstate *sstate );

coco_sobject *coco_launch_rocket( coco_sobject *sobject, coco_sobject_cl *cl,
                                  coco_sstate *sstate );


void coco_copter_pad_update( coco_sobject *copter, coco_sobject_cl *cl,
                             coco_sstate *sstate );

/* things that once were generic functions, and are needed in methods.c */

void coco_center_of_mass_obsolete( coco_sobject *sobject, 
                                   coco_sobject_cl *cl,
                                   unsigned char raster_index,
                                   coco_gnum *x_return,
                                   coco_gnum *y_return );
coco_mini_point
coco_sobject_com( coco_sobject *sobject );

coco_mini_point
coco_sobject_com_by_raster_index( coco_sobject_cl *cla, unsigned char ri );

coco_mini_point
coco_sobject_weapon_hardpoint_by_raster_index( coco_sobject_cl *cla, 
                                               unsigned char ri );


coco_mini_point
coco_cobject_com( coco_cobject *cobject );

