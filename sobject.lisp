;;;;; Copter Commander - A 2d network helicopter game
;;;;; Copyright (C) 2001  David Morse
;;;;; 
;;;;; This program is free software; you can redistribute it and/or modify
;;;;; it under the terms of the GNU General Public License as published by
;;;;; the Free Software Foundation; either version 2 of the License, or
;;;;; (at your option) any later version.
;;;;; 
;;;;; This program is distributed in the hope that it will be useful,
;;;;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;;;;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;;;;; GNU General Public License for more details.
;;;;; 
;;;;; You should have received a copy of the GNU General Public License
;;;;; along with this program; if not, write to the Free Software
;;;;; Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

;;;;; sobject.lisp: define server objects, which are implemented as 
;;;;; sntructs with pointers to class structs.

(c-define-class "sobject" ()
  (             ;; four chars star the day off right
   ("in_use"       nil         "coco_bool") ; written only by the allocator
   ("camera_index" "-1"        "signed char")
   ("comp_index"   "-1"        "signed char")
   ("facing"       :arg        "coco_facing")
   ("new_flag"     "COCO_TRUE" "coco_bool") ; true iff we're just made
   ;; now all the important ints
   ("index"        nil         "unsigned int") ; written only by allocator
   ("x"            :arg        "coco_gnum")
   ("y"            :arg        "coco_gnum")
   ("w"            
    "(COCO_SOBJECT_RASTER_COUNT( inst )>0) ? COCO_SOBJECT_RASTERS( inst )[0]->w : 1"
    "coco_gnum")
   ("h"            
    "(COCO_SOBJECT_RASTER_COUNT( inst )>0) ? COCO_SOBJECT_RASTERS( inst )[0]->h : 1" 
    "coco_gnum")
   ("dx"           "0"         "coco_gnum")
   ("dy"           "0"         "coco_gnum")
   ("dx_public"    nil         "coco_gnum") ; initilized by finalizer
   ("dy_public"    nil         "coco_gnum") ; initilized by finalizer
   ;; time we were last locked onto by ground-based missile launchers.
   ("locked_on_time" "COCO_U32_MAX" "coco_u32")
   ;; and some more chars
   ("raster_index" "0"         "unsigned char") ; set w/ coco_sobject_set_raster_index
   ("nationality"  :arg        "coco_nationality")
   ("health"       "COCO_SOBJECT_HEALTH_MAX( inst )" "coco_health")
   ;; used for different things by different objects.
   ;; for tanks and guns, its the angle of their barrel.
   ;; for missiles and copters, its the angle of the entire object.
   ("angle"        "0"         "coco_angle")

   ;; Used for temporarily caching radar blip colors during
   ;; unit serialization
   ("scratch_blip" "COCO_BLIP_TRANSPARENT" "coco_blip_index")

   ;; now the per-class
   ("phase"        "COCO_SOLID" "coco_phase" :allocation :every-class)
   ("emits_smoke_when_damaged" "COCO_FALSE" "coco_bool"
                               :allocation :every-class)
   ;; controls whether the whole object is transformed by its angle about
   ;; its center of mass 
   ("rotate_p"    "COCO_FALSE" "coco_bool" :allocation :every-class)

   ("dx_max"       "0"         "coco_gnum" :allocation :every-class)
   ("dy_max"       "0"         "coco_gnum" :allocation :every-class)
   ("x_force"      "0"         "coco_gnum" :allocation :every-class)
   ("y_force"      "0"         "coco_gnum" :allocation :every-class)
   ("health_max"   "COCO_NIGH_INVULNERABLE" "coco_health" :allocation :every-class)
   ("damage"       "COCO_NIGH_INVULNERABLE" "coco_health" :allocation :every-class)
   ("clip_to_board" "COCO_FALSE" "coco_bool" :allocation :every-class)
   ("default_blip_color_index" 
    "COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY, COCO_BLIP_EMY, COCO_BLIP_EMY )"
    "coco_blip_triplet"
    :allocation :every-class)
   ;; width+height available before the object is created.
   ;; canonical_w is used by all units to place them at the right x 
   ;; (usually centered).  canonical_h is not always defined.
   ;; it tells what is the max index (w,h) into their
   ;; raster.
   ("canonical_w"  "1 /* placeholder */" "coco_gnum" :allocation :every-class)
   ("canonical_h"  "1 /* placeholder */" "coco_gnum" :allocation :every-class)
   ("z"            "0"         "coco_znum" :allocation :every-class)
   ("raster_count" "0"         "unsigned int" :allocation :every-class)
   ("rasters"      "NULL"      "struct coco_raster **" :allocation :every-class)
   ("cobject_y_fn" "coco_cobject_y_y" ("coco_cnum (*" ")(coco_cobject *)")
                   :allocation :every-class)
   ("cobject_raster_index_fn" "coco_cobject_standard_raster_index"
                              ("unsigned char (*" ")(coco_cobject *)")
                              :allocation :every-class)
   ("cobject_guess_nationality_fn" "coco_cobject_whimpy_guess_nationality"
                                   ("int (*" ")(coco_cobject *)")
                                   :allocation :every-class)
   ("bullet_class" "COCO_BULLET_CLASS" "coco_sobject_cl *" 
                   :allocation :every-class)
   ;; this should be true if the last 2 bytes of the cobject are interpreted as
   ;; a coco_cnum (short)
   ("cobject_ntoh" "coco_cobject_y_ntoh" 
                   ("void (*" ")(coco_cobject *)")
                   :allocation :every-class)
   ("cobject_draw" "coco_cobject_rasteroid_draw"
                   ("void (*" ")(coco_cobject *, coco_sobject_cl *)")
                   :allocation :every-class)
   ("shrapnel_count" "0" "unsigned char" :allocation :every-class)
   ;; number of funds recieved for killing/capturing enemy instance.
   ("refund"         "0" "unsigned char" :allocation :every-class)
   ("cost"           "0" "unsigned char" :allocation :every-class)
   ("collision_priority" "100" "unsigned char" :allocation :every-class)
   ;; the collision rectangle is the inner region of the copter that is
   ;; susceptible to collisions.  If it is COCO_MINI_RECTANGLE_NULL_P
   ;; then the unit's 
   ;; w and h are used.  By convention the rectangle assumes the unit is
   ;; facing west (-1), and is raster index independant.  This value is
   ;; accessed by get_collision_recti.
   ("collision_rectangle" "COCO_MINI_RECTANGLE_NULL" 
                          "coco_mini_rectangle" :allocation :every-class)

   ;; only used by copters
   ("rotor_radius" "0" "coco_gnum" :allocation :every-class)
   ("max_bullets" "0" "unsigned short"  :allocation :every-class)
   ("max_bombs"   "0" "unsigned char"  :allocation :every-class)
   ("max_rockets" "0" "unsigned char"  :allocation :every-class)
   ("max_missiles" "0" "unsigned char"  :allocation :every-class)
   ("max_plasma"  "0" "unsigned short" :allocation :every-class)
   ("max_energy"   "0" "unsigned short" :allocation :every-class)
   ("max_internal_infantry" "0" "unsigned char" :allocation :every-class)
   ("max_fuel" "0" "unsigned short" :allocation :every-class)
   ("rotor_points" "NULL" "coco_mini_point *" :allocation :every-class)
   ;; only used by copters, guns, and tanks.
   ("weapon_hardpoints" "NULL" "coco_mini_point *" :allocation :every-class)
   ("center_of_mass"    "NULL" "coco_mini_point *" :allocation :every-class)
   ;; only used by guns and tanks (barreloids)
   ("gun_barrel_length" "0" "unsigned char" :allocation :every-class)
   ;; only used by infantry
   ("infantry_take_buildings_p" "COCO_TRUE" "coco_bool" 
                                :allocation :every-class)
   ;; only used by bullets, but usuable by others I suppose
   ("spark_count"     "0" "unsigned char" :allocation :every-class)
   ("thickness"       "0" "unsigned char" :allocation :every-class)
   ("muzzle_velocity" "0" "coco_gnum" :allocation :every-class)
   ("lifespan"        "0" "coco_u32" :allocation :every-class)
   ;; only used by bunkers
   ("roof_tab"        "0" "unsigned char" :allocation :every-class)
   ;; used by things that airburst
   ("airburst_sound_type" "COCO_SOUND_AIRBURST" "coco_sound_type" 
                          :allocation :every-class)
   ;; used by things that fire guns.
   ("fire_bullet_sound_type" "COCO_SOUND_COPTER_CANNON" "coco_sound_type"
                             :allocation :every-class)
   ;; used only by guns
   ;; you might think it would be more usefull than that,
   ;; but in at least one case (troopers versus bunkers) the computation
   ;; is more complex than that.  
   ("targetable_classes_count" "0" "unsigned char" :allocation :every-class)
   ("targetable_classes"       "NULL" "coco_sobject_cl **" 
                               :allocation :every-class)
   ;; only used by fancy vehicle classes (mech, cobra, cargo-copter, etc)
   ("dashboard_icon"           "NULL" "struct coco_raster *"
                               :allocation :every-class)
   ("dashboard_list"           "0"   "unsigned int"  :allocation :every-class);; opengl display list
   ("dashboard_items"          "NULL" "struct coco_dashboard_item **"
                               :allocation :every-class)
   ("dashboard_nitems"         "-1"   "int" :allocation :every-class)
   ("gen_dashboard_items"      "NULL"
                               ("int (*" ")(coco_sobject_cl *, struct coco_dashboard_item **)")
                               :allocation :every-class))
  :class-layout :index
  :named-classes-p t
  :init-by-list-fn :virtual)


(c-define-generic-function "sobject_unlink" "sobject" "void"
                           (("link_index" "int")
                            ("sstate"     "coco_sstate *")))

(c-define-method "sobject_unlink" "sobject" 
                 "coco_every_sobject_unlink")

(c-define-generic-function "emit_smoke" "sobject" "coco_sobject *" 
                           (("sstate" "coco_sstate *")))

(c-define-method "emit_smoke" "sobject"
                 "coco_every_sobject_emit_smoke")

(c-define-generic-function "sobject_update_state" "sobject" "coco_bool"
                           (("sstate" "coco_sstate *")))
;; blow yourself up, A-team style
(c-define-generic-function "kaboom" "sobject" "void"
                           (("sstate" "coco_sstate *")))
;; clean up any private memory you may have
(c-define-generic-function "sobject_delete" "sobject" "void"
                           (("sstate" "coco_sstate *")))
(c-define-generic-function "take_damage" "sobject" "void"
                           (("health" "coco_health")
                            ("sstate" "coco_sstate *")))

(c-define-generic-function "hostile_collision" 
                           "sobject" "void"
                           (("obj2" "coco_sobject *")
                            ("sstate" "coco_sstate *")
                            ("stat1" "coco_bool *")
                            ("stat2" "coco_bool *")))

(c-define-method "hostile_collision" "sobject"
                 "coco_every_sobject_hostile_collision")

(c-define-generic-function "friendly_collision" 
                           "sobject" "void"
                           (("obj2" "coco_sobject *")
                            ("sstate" "coco_sstate *")
                            ("stat1" "coco_bool *")
                            ("stat2" "coco_bool *")))

(c-define-method "friendly_collision" "sobject"
                 "coco_every_sobject_friendly_collision")


(c-define-generic-function "effective_damage" "sobject" "coco_health"
                           (("damager" "coco_sobject *")))

(c-define-method "effective_damage" "sobject" 
                 "coco_unarmored_effective_damage")

(c-define-generic-function "ground_collide" "sobject" "void"
                           (("sstate" "coco_sstate *")))

(c-define-method "ground_collide" "sobject" "coco_every_sobject_ground_collide")

(c-define-generic-function "sobject_to_cobject" "sobject" "void"
                           (("cobject" "coco_cobject *")
                            ("cam_left" "coco_gnum")
                            ("network_byte_order" "coco_bool")))

(c-define-method "sobject_to_cobject" "sobject"
                 "coco_rasteroid_to_cobject")

(c-define-generic-function "sobject_neutral_building_collide" "sobject" "void"
                           (("building" "coco_sobject *")))

(c-define-method "sobject_neutral_building_collide" "sobject"
                 "sobject_ignore_neutral_buiding")

#+init-hook
(c-define-method "sobject_init_hook" "sobject" "coco_finalize_sobject")

(c-define-method "sobject_update_state" "sobject" 
                 "coco_every_sobject_update_state")

(c-define-method "kaboom" "sobject" "coco_every_sobject_kaboom")
(c-define-method "sobject_delete" "sobject" "coco_every_sobject_delete")

(c-define-generic-function "ground_trooper_target_p" "sobject" "coco_bool"
                           (("trooper" "coco_sobject *")))



(c-define-method "ground_trooper_target_p" "sobject"
                 "coco_trooper_always_target_me")

(c-define-generic-function "sobject_blip_color_index" "sobject" 
                           "coco_blip_index"
                           (("p_or_c_index" "int")
                            ("is_comp" "coco_bool")
                            ("sstate" "coco_sstate *")))

(c-define-method "sobject_blip_color_index" "sobject"
                 "coco_every_sobject_blip_color_index")

(c-define-generic-function "sobject_is_visible" "sobject" "coco_bool"
                           (("cam_left" "coco_gnum")
                            ("cam_right" "coco_gnum")
                            ("cam_or_null" "coco_sobject *")))

(c-define-method "sobject_is_visible" "sobject" 
                 "coco_every_sobject_is_visible")

(c-define-class "sndobject" ("sobject")
  (("canonical_w"  "0")
   ("canonical_h"  "0")
   ("w" 0)
   ("h" 0)
   ("cobject_draw" "coco_cobject_sound_draw")
   ("sound" :arg "coco_sound_type")
   ("facing" "0")
   ("nationality" "COCO_NEUTRAL")
   ("phase"       "COCO_VAPOR")))

(c-define-method "sobject_update_state" "sndobject" 
                 "coco_sndobject_update_state")

(c-define-method "sobject_is_visible" "sndobject" 
                 "coco_sndobject_is_visible")

(c-define-method "sobject_to_cobject" "sndobject"
                 "coco_sndobject_to_cobject")

;; necessary?  Its phase is COCO_VAPOR anyway. -dm 01-29-2001
(c-define-method "ground_collide" "sndobject" "coco_ignore_ground_collide")

;; anything the player looks through (abstract)
(c-define-class "camera" ("sobject")
  (("camera_index" :arg)
   ("collision_priority" "50")
   ("default_blip_color_index" 
       "COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY_PLAYER, COCO_BLIP_EMY_PLAYER, COCO_BLIP_EMY_PLAYER )")))

(c-define-generic-function "camera_velocity_offset" "sobject"
                           "coco_gnum" ())

(c-define-method "camera_velocity_offset" "camera"
                 "coco_every_camera_velocity_offset")

(c-define-method "sobject_update_state" "camera" 
                 "coco_camera_update_state")

(c-define-method "sobject_blip_color_index" "camera"
                 "coco_camera_blip_color_index")

;; a invisble viewer thingie for the player, so he can see his shrapnell
;; when he dies.
(c-define-class "inviso_camera" ("camera")
  (("facing"       "0")
   ("canonical_w"  "0")
   ("phase"        "COCO_VAPOR")
   ("y"            "COCO_GROUND_Y + 1")
   ("cobject_draw" "coco_cobject_inviso_draw")
   ("default_blip_color_index" 
       "COCO_BLIP_TRIPLET_TRANSPARENT")))

(c-define-method "ground_trooper_target_p" "inviso_camera"
                 "coco_trooper_never_target_me")

(c-define-method "sobject_update_state" "inviso_camera" 
                 "coco_inviso_camera_update_state")

(c-define-class "copter" ("camera")
  (("bullets"      "COCO_SOBJECT_MAX_BULLETS( inst )" "unsigned short")
   ("bombs"        "COCO_SOBJECT_MAX_BOMBS( inst )"   "unsigned char")
   ("missiles"      "COCO_SOBJECT_MAX_MISSILES( inst )" "unsigned char")
   ("rockets"      "COCO_SOBJECT_MAX_ROCKETS( inst )" "unsigned char")
   ("paratroopers" "0"                                "unsigned char")
   ("engitroopers" "0"                                "unsigned short")
   ("fuel"         "COCO_SOBJECT_MAX_FUEL(inst)"      "unsigned short")
   ("missile_cooldown" "COCO_FALSE" "coco_bool")
   ;; when to set missile_cooldown to false again
   ("missile_cooldown_time" nil     "coco_u32") 
   ("next_missilebeat_on_pad" "0"   "coco_u32")
   ("dx_max"       "COCO_COPTER_DX_MAX")
   ("dy_max"       "COCO_COPTER_DY_MAX")
   ("x_force"      "4") ; for decel.  Accell is half that.
   ("health_max"   "4")
   ("clip_to_board" "COCO_TRUE")
   ("raster_index" "((nationality==COCO_BAD_GUYS)<<1)+(facing>0)")
   ("shrapnel_count" "8")
   ;; copter is now abstract class, with individual model classes below it
   ;;("canonical_w"  "COCO_COPTER_EVEN_RASTER_W")
   ;;("rasters"      "coco_copter_rasters")
   ("raster_count" "COCO_COPTER_RASTER_COUNT") ;; all have same # rasters
   ("cobject_guess_nationality_fn" "coco_copter_guess_nationality")
   ;; also massy-rectangle rotor_points, and rotor_radius in sobject.
   ("cobject_draw" "coco_copter_draw")
   ("rotor_radius" "10")
   ("last_bomb_drop_time" "0" "coco_u32")
   ("last_rocket_launch_time" "0" "coco_u32")
   ("landed_p"      "COCO_FALSE" "coco_bool")
   ;; only valid to look at when landed_p:
   ("landed_on_pad_p" "COCO_FALSE" "coco_bool")
   ("damage"        "COCO_COPTER_DAMAGE")
   ("angle"        "(facing>0) ? 0 : 128")
   ("rotate_p"    "COCO_TRUE")
   ("w"           "COCO_SOBJECT_CANONICAL_W( inst )")
   ("h"           "COCO_SOBJECT_CANONICAL_H( inst )")
   ("airburst_sound_type" "COCO_SOUND_BIG_AIRBURST")
   ("gen_dashboard_items" "coco_copter_dashboard_items")
   ("emits_smoke_when_damaged" "COCO_TRUE")))

(c-define-method "emit_smoke" "copter" "coco_copter_emit_smoke")

(c-define-method "effective_damage" "copter" 
                 "coco_flesh_damageable_effective_damage")

(c-define-method "kaboom" "copter" "coco_copter_kaboom")

(c-define-method "friendly_collision" "copter"
                 "coco_copter_friendly_collision")

(c-define-method "hostile_collision" "copter"
                 "coco_copter_hostile_collision")

(c-define-method "ground_collide" "copter" "coco_copter_ground_collide")

(c-define-method "camera_velocity_offset" "copter"
                 "coco_copter_velocity_offset")

(c-define-method "sobject_delete" "copter" "coco_copter_mech_delete")

(c-define-method "sobject_update_state" "copter" 
                 "coco_copter_update_state")

(c-define-method "sobject_blip_color_index" "copter"
                 "coco_copter_mech_blip_color_index")

(c-define-class "cobra" ("copter")
  (("max_bullets"      "100")
   ("max_bombs"        "12")
   ("max_missiles"      "2")
   ("max_rockets"      "10")
   ("max_internal_infantry" "5")
   ("max_fuel"     "200*COCO_IDEAL_FRAMES_PER_SECOND")
   ("canonical_w"  "100")
   ("canonical_h"  "30")
   ("collision_rectangle" "COCO_MAKE_MINI_RECTANGLE( 2, 0, 62, 29 )")
   ("rotor_points"      "coco_cobra_rotor_points")
   ("weapon_hardpoints" "coco_cobra_weapon_hardpoints")
   ("center_of_mass"    "coco_cobra_center_of_mass")
   ("rasters"      "coco_cobra_rasters")
   ("rotor_radius" "56")
   ("refund"       "4")
   ("cost"         "20")
   ("dashboard_icon" "&coco_cobra_icon_raster")
))

(c-define-class "bubble_copter" ("copter")
  (("max_bullets"      "80")
   ("max_bombs"        "8")
   ("max_missiles"      "0")
   ("max_rockets"      "0")
   ("max_internal_infantry" "4")
   ("max_fuel"     "160*COCO_IDEAL_FRAMES_PER_SECOND")
   ("dx_max"       "COCO_COPTER_DX_MAX-2")
   ("dy_max"       "COCO_COPTER_DY_MAX-1")
   ("canonical_w"  "84")
   ("canonical_h"  "30")
   ("collision_rectangle" "COCO_MAKE_MINI_RECTANGLE( 0, 0, 53, 29 )")
   ("rotor_points"      "coco_bubble_copter_rotor_points")
   ("weapon_hardpoints" "coco_bubble_copter_weapon_hardpoints")
   ("center_of_mass"    "coco_bubble_copter_center_of_mass")
   ("rasters"      "coco_bubble_copter_rasters")
   ("rotor_radius" "50")
   ("refund"       "1")
   ("cost"         "5")
   ("health_max"   "3")
   ("x_force"      "2")
))

(c-define-class "cargo_copter" ("copter")
  (("max_bullets"      "80")
   ("max_bombs"        "0")
   ("max_missiles"     "0")
   ("max_rockets"      "36")
   ("max_internal_infantry" "12")
   ("max_fuel"     "300*COCO_IDEAL_FRAMES_PER_SECOND")
   ("dx_max"       "15")
   ("dy_max"       "9")
   ("canonical_w"  "124")
   ("canonical_h"  "48")
   ("collision_rectangle" 
    "COCO_MAKE_MINI_RECTANGLE( 1, 7, 83, 35 )")
   ("rotor_points"      "coco_cargo_copter_rotor_points")
   ("weapon_hardpoints" "coco_cargo_copter_weapon_hardpoints")
   ("center_of_mass"    "coco_cargo_copter_center_of_mass")
   ("rasters"      "coco_cargo_copter_rasters")
   ("rotor_radius" "65")
   ("refund"       "2")
   ("cost"         "10")
   ("health_max"   "5")
   ("x_force"      "2")
   ("dashboard_icon" "&coco_cargo_copter_icon_raster")))

(c-define-class "mech" ("camera")
  (("energy"       "COCO_SOBJECT_MAX_ENERGY( inst )" "coco_u16")
   ("landed_on_pad_p" "COCO_FALSE" "coco_bool")
   ("max_energy"   "COCO_MECH_CAPACITANCE")
   ;; should missile_cooldown be ported here, or be factored up to camera?
   ;;("missile_cooldown" "COCO_FALSE" "coco_bool")
   ;; when to set missile_cooldown to false again
   ;;("missile_cooldown_time" nil     "coco_u32") 
   ;;("next_missilebeat_on_pad" "0"   "coco_u32")
   ("last_plasma_fire_time" "0" "coco_u32")
   ("plasma" "COCO_SOBJECT_MAX_PLASMA( inst )" "unsigned short")
   ("max_plasma"    "100")
   ("squatting_down" "COCO_FALSE" "coco_bool")
   ("shield_powered" "COCO_FALSE" "coco_bool")
   ("dx_max"       "COCO_MECH_JUMPING_DX_MAX")
   ("dy_max"       "COCO_MECH_DY_MAX")
   ("x_force"      "4") ; only used when flying, otherwise accell is instant??
   ("health_max"   "7")
   ("torso_raster_index" "0" "unsigned char")
   ("leg_raster_index" "COCO_MIN_IDLE_PARTS" "unsigned char")
   ("clip_to_board" "COCO_TRUE")
   ;;("raster_index" "0") ;; doesn't use normal raster facility
   ("shrapnel_count" "8")
   ;; copter is now abstract class, with individual model classes below it
   ("canonical_w"  "34") ;; never reset
   ("canonical_h"  "60") ;; never reset
   ;;("rasters"      "coco_copter_rasters")
   ("raster_count" "0")
   ("cobject_guess_nationality_fn" "coco_mech_guess_nationality")
   ;; also massy-rectangle rotor_points, and rotor_radius in sobject.
   ("cobject_draw" "coco_mech_draw")
   ("damage"        "COCO_MECH_DAMAGE")
   ;;("angle"        "(facing>0) ? 0 : 128")
   ("w"           "COCO_SOBJECT_CANONICAL_W( inst )")
   ("h"           "COCO_SOBJECT_CANONICAL_H( inst )")
   ("airburst_sound_type" "COCO_SOUND_BIG_AIRBURST")
   ("cost"        "20")
   ("dashboard_icon" "&coco_mech_icon_raster") ;; XXX wrong
   ("input_buffers" nil
    ;; intentially left uninitialized.  Don't have aggregate initializers
    ;; in oo-compiler, I think.
    ;; Will be sorted out after ~3 frames of confusion anyway.
    ("COCO_BIT_VEC_VAR( "
     ", COCO_MECH_BUFFERED_BUTTONS_HACK*COCO_MECH_BUFFERED_TURNS_HACK )"))
   ("gen_dashboard_items" "coco_mech_dashboard_items")
   ("emits_smoke_when_damaged" "COCO_TRUE")))

(c-define-method "hostile_collision" "mech" 
                 "coco_mech_hostile_collision")

(c-define-method "sobject_to_cobject" "mech"
                 "coco_mech_to_cobject")

(c-define-method "sobject_update_state" "mech" 
                 "coco_mech_update_state")

(c-define-method "sobject_blip_color_index" "mech"
                 "coco_copter_mech_blip_color_index")

(c-define-method "sobject_delete" "mech" "coco_copter_mech_delete")

(c-define-method "camera_velocity_offset" "mech"
                 "coco_mech_velocity_offset")

(c-define-method "ground_collide" "mech" "coco_fixup_ground_collide")

(c-define-method "kaboom" "mech" "coco_airbursting_kaboom")

(c-define-class "smoke" ("sobject")
  (("raster_index" :arg)
   ("z"            "2")
   ("phase"        "COCO_VAPOR")
   ("facing"       "0")
   ("nationality"  "COCO_NEUTRAL")
   ("rasters"      "coco_smoke_rasters")
   ("raster_count" "COCO_SMOKE_RASTER_COUNT")
   ("canonical_w"  "COCO_SMOKE_BIG_RASTER_W")
   ("default_blip_color_index" 
       "COCO_BLIP_TRIPLET_TRANSPARENT")))


(c-define-method "sobject_update_state" "smoke" 
                 "coco_smoke_update_state")

(c-define-class "cloud" ("sobject")
  (("cloud_time_phase" "rand()&255" "coco_u32")
   ("z"            "1")
   ("phase"        "COCO_VAPOR")
   ("facing"       "0")
   ("nationality"  "COCO_NEUTRAL")
   ("raster_index" "rand()%COCO_CLOUD_RASTER_COUNT")
   ("rasters"      "coco_cloud_rasters")
   ("raster_count" "COCO_CLOUD_RASTER_COUNT")
   ("canonical_w"  "COCO_CLOUD1_RASTER_W")
   ("clip_to_board" "COCO_TRUE")
   ("default_blip_color_index" 
       "COCO_BLIP_TRIPLET_TRANSPARENT")
   ("cobject_draw"  "coco_cloud_draw"))
  :init-by-list-fn :automatic)

(c-define-method "sobject_update_state" "cloud" 
                 "coco_cloud_update_state")



(c-define-class "balloon_part" ("sobject")
  ())

(c-define-method "ground_trooper_target_p" "balloon_part"
                 "coco_trooper_never_target_me")

(c-define-method "hostile_collision" "balloon_part"
                 "coco_balloon_part_hostile_collision")

(c-define-class "balloon" ("balloon_part")
  (("line_index"   "-1"  "int")
   ("balloon_time_phase" "rand()&255" "coco_u32")
   ("facing"       "0")
   ("raster_index" :arg)
   ("rasters"      "coco_balloon_rasters")
   ("raster_count" "COCO_BALLOON_RASTER_COUNT")
   ("health_max"   "4 * COCO_BULLET_DAMAGE")
   ("z"             "-1") ;; behind balloon-factory's 0.
   ;;("cobject_raster_index_fn" "coco_cobject_simple_raster_index") ; not used
   ("collision_priority" "150")
   ("canonical_w"   "COCO_BALLOON_W_RASTER_W")
   ("canonical_h"   "COCO_BALLOON_W_RASTER_H")
   ("damage"       "7")
   ("factory_fresh" "COCO_FALSE" "coco_bool")
   ("clip_to_board" "COCO_TRUE")
   ("default_blip_color_index" 
       "COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY_BALLOON, COCO_BLIP_EMY_BALLOON, COCO_BLIP_EMY_BALLOON )"))
  :init-by-list-fn :automatic)


(c-define-method "kaboom" "balloon" "coco_airbursting_kaboom")

(c-define-method "sobject_unlink" "balloon" "coco_balloon_unlink")
(c-define-method "sobject_update_state" "balloon" "coco_balloon_update_state")

;; this is a dopelganger for balloons that come out of factories, because
;; everyone complians that they shouldn't collide with newly beamed in
;; balloons.
(c-define-class "fake_balloon" ("balloon")
  (("phase" "COCO_VAPOR")
   ("z"     "-2")))

(c-define-method "sobject_update_state" "fake_balloon" 
                 "coco_fake_balloon_update_state")

(c-define-class "balloon_line" ("balloon_part")
  (("hut_index"     :arg "int")
   ("balloon_index" :arg "int")
   ("facing"        "0")
   ("phase"         "COCO_EXPLOSIVE")
   ("cobject_draw"  "coco_balloon_line_draw")
   ("w"             "1")
   ("h"             :arg)
   ("canonical_w"   "1")
   ("health_max"    "1")
   ("damage"        "3")
   ("default_blip_color_index" 
       "COCO_BLIP_TRIPLET_TRANSPARENT"))
  :init-by-list-fn :automatic)

(c-define-method "sobject_update_state" "balloon_line" 
                 "coco_balloon_line_update_state")
(c-define-method "sobject_unlink" "balloon_line" "coco_balloon_line_unlink")

(c-define-class "building" ("sobject")
  (("y"            "COCO_GROUND_Y")
   ("facing"       "0")
   ;; by default buildings have only 1 raster 
   ;; (except for headquarters, bunkers, guns, huts ... 
   ;; guess that was bad assumption!)
   ("cobject_raster_index_fn" "coco_cobject_simple_raster_index")
   ("shrapnel_count" "4")
   ("collision_priority" "200")
   ("z" "-1")
   ("default_blip_color_index" 
       "COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY_BUILDING, COCO_BLIP_EMY_BUILDING, COCO_BLIP_EMY_BUILDING )")))

(c-define-method "kaboom" "building" "coco_airbursting_kaboom")

(c-define-method "ground_trooper_target_p" "building"
                 "coco_trooper_never_target_me")

(c-define-method "hostile_collision" "building"
                 "coco_building_hostile_collision")

;; also known as tree
(c-define-class "traffic_sign" ("building")
  (("raster_index" :arg)
   ("nationality" "COCO_NEUTRAL")
   ("phase"       "COCO_VAPOR")
   ("raster_index"  :arg)
   ("rasters"       "coco_traffic_sign_rasters")
   ("raster_count"  "COCO_TRAFFIC_SIGN_RASTER_COUNT")
   ("cobject_guess_nationality_fn" "coco_neutral_guess_nationality")
   ("cobject_raster_index_fn" "coco_cobject_standard_raster_index")
   ("default_blip_color_index" 
       "COCO_BLIP_TRIPLET_TRANSPARENT"))
  :init-by-list-fn :automatic)

(c-define-class "hut" ("building")
  (("line_index"    "-1"       "int")
   ;; reincarnation of balloon:
   ("points_till_reincarnation" "COCO_HUT_BALLOON_REINCARNATION_FRAMES" "int")
   ("raster_index"  "nationality")
   ("rasters"       "coco_hut_rasters")
   ("raster_count"  "COCO_HUT_RASTER_COUNT")
   ("cobject_guess_nationality_fn" "coco_hut_guess_nationality")
   ("health_max"    "COCO_MISSILE_DAMAGE<<1")
   ("canonical_w"   "COCO_HUT_NEUTRAL_RASTER_W")
   ("cobject_raster_index_fn" "coco_cobject_standard_raster_index")
   #+coco-appearance-doodle
   ("h"             "28")
   ("refund"        "4")
   )
  :init-by-list-fn "coco_hut_init_by_list"
  :init-by-list-count 2)

(c-define-method "sobject_update_state" "hut"
                 "coco_hut_update_state")

(c-define-method "sobject_unlink" "hut" "coco_hut_unlink")

(c-define-class "bunker" ("building")
  (("defenders"      :arg     "unsigned char")
   ("raster_index"   "nationality - COCO_GOOD_GUYS")
   ("collision_priority"  "12")
   ("rasters"        "coco_bunker_rasters")
   ("raster_count"   "COCO_BUNKER_RASTER_COUNT")
   ("canonical_w"    "COCO_BUNKER_GREEN_RASTER_W")
   ("roof_tab"       "50")
   ("bullet_class"   "COCO_RIFLE_BULLET_CLASS")
   ("cobject_raster_index_fn" "coco_cobject_standard_raster_index")
   ("cobject_guess_nationality_fn" "coco_2raster_guess_nationality")
   ("max_internal_infantry" "5")
   ("refund"         "7"))
  :init-by-list-fn :automatic)

(c-define-method "hostile_collision" "bunker" 
                 "coco_bunker_hostile_collision")

(c-define-method "ground_trooper_target_p" "bunker"
                 "coco_trooper_bunker_target_p")

(c-define-method "sobject_update_state" "bunker"
                 "coco_bunker_update_state")

(c-define-class "pillbox" ("bunker")
  (("headquarters_index"      :arg     "int")
   ("raster_index" "0")
   ("rasters"      "coco_pillbox_rasters")
   ("raster_count" "COCO_PILLBOX_RASTER_COUNT")
   ("canonical_w"  "COCO_PILLBOX_RASTER_W")
   ("roof_tab"     "0")
   ("max_internal_infantry" "2"))
  :init-by-list-fn :automatic)

(c-define-method "sobject_unlink" "pillbox" 
                 "coco_pillbox_unlink")

(c-define-class "balloon_factory" ("building")
  (("nationality"    "COCO_NEUTRAL")
   ("phase"          "COCO_VAPOR")
   ("raster_index"   "0")
   ("rasters"        "coco_balloon_factory_rasters")
   ("raster_count"   "COCO_BALLOON_FACTORY_RASTER_COUNT")
   ("canonical_w"    "COCO_BALLOON_FACTORY_BASE_RASTER_W")
   ("balloon_rate"   :arg "int");; in seconds, must be >= 2
   ("cobject_guess_nationality_fn" "coco_neutral_guess_nationality")
   ("cobject_draw"  "coco_balloon_factory_draw"))
  :init-by-list-fn :automatic)

(c-define-method "sobject_update_state" "balloon_factory"
                 "coco_balloon_factory_update_state")

(c-define-class "landing_pad" ("building")
  (("nationality"  "COCO_NEUTRAL")
   ("rasters"      "coco_landing_pad_rasters")
   ("raster_count" "COCO_LANDING_PAD_RASTER_COUNT")
   ("cobject_guess_nationality_fn" "coco_neutral_guess_nationality")
   ("canonical_w"  "COCO_LANDING_PAD_RASTER_W")
   ("default_blip_color_index" 
       "COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_LANDING_PAD, COCO_BLIP_LANDING_PAD, COCO_BLIP_LANDING_PAD )"))
  :init-by-list-fn :automatic)

(c-define-class "headquarters" ("building")
  ;; should use COCO_SOBJECT_NATIONALITY( inst ) not nationality, but
  ;; out-of-order execution problem arises.
  (("x"               :arg)
   ("facing"          "(nationality == COCO_GOOD_GUYS) ? -1 : 1")
   ("smoke_countdown" "0"    "unsigned char")
   ("pillbox_index"   :arg   "int")
   ("raster_index"    "nationality - COCO_GOOD_GUYS")
   ("rasters"         "coco_headquarters_rasters")
   ("raster_count"    "COCO_HEADQUARTERS_RASTER_COUNT")
   ("canonical_w"     "COCO_HEADQUARTERS_GREEN_RASTER_W")
   ("cobject_guess_nationality_fn" "coco_2raster_guess_nationality")
   ("cobject_raster_index_fn" "coco_cobject_standard_raster_index")
   ("shrapnel_count"  "25")
   ("refund"          "255")
   ("airburst_sound_type" "COCO_SOUND_BIG_AIRBURST"))
  :init-by-list-fn :automatic)

(c-define-method "sobject_unlink" "headquarters" 
                 "coco_headquarters_unlink")

(c-define-method "sobject_update_state" "headquarters"
                 "coco_headquarters_update_state")

(c-define-class "gun" ("building")
  (("cooldown"     "0"         "unsigned short")
   ("angle"        "32")
   ("cobject_raster_index_fn" "coco_cobject_standard_raster_index")
   ;; slightly less than 2 missiles, plenty more than 1 copter:
   ("health_max"   "23")
   ("rasters"       "coco_gun_rasters")
   ("raster_count"  "COCO_GUN_RASTER_COUNT")
   ("raster_index"  "nationality - COCO_GOOD_GUYS")
   ("cobject_guess_nationality_fn" "coco_2raster_guess_nationality")
   ("shrapnel_count" "4")
   ("cobject_draw" "coco_cobject_barreloid_draw")
   ("weapon_hardpoints" "coco_gun_hardpoints")
   ("gun_barrel_length" "20")
   ("canonical_w"   "COCO_GUN_GREY_RASTER_W")
   ("refund"        "4")))

(c-define-method "sobject_update_state" "gun"
                 "coco_gun_update_state")

(c-define-method "ground_trooper_target_p" "gun"
                 "coco_trooper_always_target_me")

(c-define-method "kaboom" "gun" "coco_gun_kaboom")

(c-define-method "effective_damage" "gun" "coco_light_armor_effective_damage")

(c-define-class "aa_gun" ("gun")
  (("targetable_classes_count" "2")
   ("targetable_classes"       "coco_aa_targetable_classes"))
  :init-by-list-fn :automatic)

(c-define-class "at_gun" ("gun")
  (("targetable_classes_count" "1")
   ("targetable_classes"       "coco_at_targetable_classes")
   ("angle"        "(COCO_GOOD_GUYS == COCO_SOBJECT_NATIONALITY( inst )) ? 128 : 0"))
  :init-by-list-fn :automatic)

(c-define-class "gun_rubble" ("building")
  (("reincarnation_class_index" :arg "unsigned char")
   ("points_till_reincarnation" "COCO_IDEAL_FRAMES_PER_SECOND<<4" "int")
   ("reincarnation_nationality" "COCO_NEUTRAL" "coco_nationality")
   ("phase" "COCO_VAPOR")
   ("rasters" "coco_gun_rubble_rasters")
   ("raster_count" "COCO_GUN_RUBBLE_RASTER_COUNT")
   ("cobject_guess_nationality_fn" "coco_neutral_guess_nationality")
   ("nationality" "COCO_NEUTRAL")
   ("canonical_w"   "COCO_GUN_RUBBLE_RASTER_W")
   ("default_blip_color_index" 
       "COCO_BLIP_TRIPLET_TRANSPARENT"))
  :init-by-list-fn :automatic)

(c-define-method "sobject_update_state" "gun_rubble"
                 "coco_gun_rubble_update_state")

(c-define-class "ground_unit" ("sobject")
  (("y"            "COCO_GROUND_Y")
   ("dx_max"       "COCO_GROUND_UNIT_DX_MAX")
   ("dy_max"        "1")
   ;; can't use COCO_SOBJECT_NATIONALITY( inst ) because of order problems:
   ("facing"       "(COCO_GOOD_GUYS == nationality) ? -1 : 1")
   ("dx"           "(COCO_GOOD_GUYS == nationality) ? -1 : 1")
   ("cobject_y_fn" "coco_cobject_ground_unit_y")
   ("health_max"   "COCO_BOMB_DAMAGE-1")))

(c-define-method "hostile_collision" "ground_unit" 
                 "coco_ground_unit_hostile_collision")

(c-define-method "sobject_update_state" "ground_unit"
                 "coco_ground_unit_update_state")

(c-define-class "ground_vehicle" ("ground_unit")
  (("shrapnel_count" "3")
   ("raster_index" "nationality - COCO_GOOD_GUYS")
   ("cobject_guess_nationality_fn" "coco_2raster_guess_nationality"))
  :init-by-list-fn :automatic)

(c-define-method "kaboom" "ground_vehicle" "coco_airbursting_kaboom")

(c-define-class "demo_van" ("ground_vehicle")
  (("raster_index" "((nationality - COCO_GOOD_GUYS)<<1) + (0==(rand()&127))")
   ("rasters"      "coco_demo_van_rasters")
   ("raster_count" "COCO_DEMO_VAN_RASTER_COUNT")
   ("cobject_guess_nationality_fn" "coco_demo_van_guess_nationality")
   ("shrapnel_count" "4")
   ("health_max"   "2")
   ("canonical_w"   "COCO_VAN_GREEN_RASTER_W")
   ("cost" "2"))
  :init-by-list-fn :automatic)

(c-define-method "hostile_collision" "demo_van" 
                 "coco_demo_van_hostile_collision")

(c-define-class "tank" ("ground_vehicle")
  (("angle"        "(COCO_SOBJECT_FACING( inst )>0) ? 6 : 122")
   ("raster_index" "nationality - COCO_GOOD_GUYS")
   ("rasters"      "coco_tank_rasters")
   ("raster_count" "COCO_TANK_RASTER_COUNT")
   ("health_max"   "COCO_BOMB_DAMAGE*3")
   ("gun_barrel_length" "29")
   ("cobject_draw" "coco_cobject_barreloid_draw")
   ("weapon_hardpoints" "coco_tank_hardpoints")
   ("canonical_w"  "COCO_TANK_GREEN_RASTER_W")
   ("bullet_class" "COCO_TANK_BULLET_CLASS")
   ("refund"       "1")
   ("cost"         "4")
   ("fire_bullet_sound_type" "COCO_SOUND_TANK_GUN"))
  :init-by-list-fn :virtual)

(c-define-method "effective_damage" "tank" "coco_heavy_armor_effective_damage")

(c-define-method "sobject_update_state" "tank" 
                 "coco_tank_update_state")

(c-define-class "firetruck" ("ground_vehicle")
  (("raster_index" "(nationality - COCO_GOOD_GUYS) << 1")
   ("rasters"      "coco_firetruck_rasters")
   ("raster_count" "COCO_FIRETRUCK_RASTER_COUNT")
   ("cobject_guess_nationality_fn" "coco_firetruck_guess_nationality")
   ("smoke_countdown" "0" "unsigned char")
   ("canonical_w"   "COCO_FIRETRUCK_GREEN_RASTER_W")
   ("cost"         "3"))
  :init-by-list-fn :automatic)

(c-define-method "sobject_update_state" "firetruck"
                 "coco_firetruck_update_state")

(c-define-class "infantry" ("ground_unit")
  (("y"            :arg)
   ("dx"           :arg)
   ("dy"           :arg)
   ("raster_index" :arg)
   ("dy_max"       "COCO_GNUM_MAX")
   ("parachutes"   "2"               "unsigned char")
   ;; height at which to try to deploy chute.
   ("parachute_height" "COCO_SOBJECT_Y( inst ) - (rand()&127)" "coco_gnum")
   ("cobject_y_fn" "coco_cobject_y_y")
   ("y_force"      "COCO_FLYING_MAN_ACCELL")
   ("rasters"       "coco_infantry_rasters")
   ("raster_count"  "COCO_INFANTRY_RASTER_COUNT")
   ("cobject_guess_nationality_fn" "coco_infantry_guess_nationality")
   ("health_max"   "1")
   ("bullet_class" "COCO_RIFLE_BULLET_CLASS")
   ("shrapnel_count" "0")
   ("damage"         "0")
   ("collision_priority" "75")
   ("canonical_w"    "COCO_INFANTRY_E1_RASTER_W")
   ("infantry_take_buildings_p" "COCO_TRUE")
   ("z" "1")
   ("default_blip_color_index" 
       "COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY_INFANTRY, COCO_BLIP_TRANSPARENT, COCO_BLIP_TRANSPARENT )")
   ("cost" "1")
   ("fire_bullet_sound_type" "COCO_SOUND_RIFLE"))
  :init-by-list-fn "coco_infantry_init_from_ground_by_list"
  :init-by-list-count 2)

(c-define-method "kaboom" "infantry" "coco_infantry_kaboom")

(c-define-generic-function "infantry_enter_hut" "sobject"
                           "coco_bool"
                           (("hut" "coco_sobject *")
                            ("sstate" "coco_sstate *")
                            ("stand_still_p" "coco_bool *")))
(c-define-method "infantry_enter_hut" "infantry"
                 "coco_feisty_infantry_enter_hut")

(c-define-method "sobject_update_state" "infantry" 
                 "coco_infantry_update_state")

(c-define-class "engineer" ("infantry")
  (("rasters"       "coco_engineer_rasters")
   ("raster_count"  "COCO_ENGINEER_RASTER_COUNT")
   ("infantry_take_buildings_p" "COCO_FALSE")
   ("refund"        "1")
   ("cost"          "2"))
  :init-by-list-fn :virtual
  :init-by-list-count 2)

;; purely for side effects
(c-define-generic-function "enter_friendly_bunker" "sobject" "void"
                           (("bunker" "coco_sobject *")
                            ("sstate" "coco_sstate *")))

(c-define-method "enter_friendly_bunker" "sobject"  "coco_ignore_bunker")
(c-define-method "enter_friendly_bunker" "infantry" 
                 "coco_infantry_enter_friendly_bunker")
(c-define-method "enter_friendly_bunker" "engineer"  "coco_ignore_bunker")


(c-define-method "infantry_enter_hut" "engineer"
                 "coco_engineer_enter_hut")

(c-define-method "sobject_update_state" "engineer"
                 "coco_engineer_update_state")

(c-define-method "effective_damage" "infantry" 
                 "coco_infantry_effective_damage")

(c-define-class "munition" ("sobject")
  (("dx"           :arg)
   ("dy"           :arg)
   ("phase"        "COCO_EXPLOSIVE")
   ("health_max"   "1")
   ("shrapnel_count" "0")
   ("collision_priority" "COCO_MUNITION_COLLISION_PRIORITY")
   ("default_blip_color_index" "COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY_MUNITION, COCO_BLIP_EMY_MUNITION, COCO_BLIP_EMY_MUNITION )")))

(c-define-method "ground_trooper_target_p" "munition"
                 "coco_trooper_never_target_me")

(c-define-method "hostile_collision" "munition"
                 "coco_munition_hostile_collision")

(c-define-class "bullet" ("munition")
  (("damage"       "COCO_BULLET_DAMAGE")
   ("expiration_date" :arg "coco_u32")
   ("cobject_draw" "coco_bullet_draw")
   ("canonical_w"  "1")
   ("thickness"       "2")
   ("muzzle_velocity" "COCO_BULLET_SPEED")
   ("lifespan"        "COCO_BULLET_LIFESPAN")
   ("spark_count"  "1")
   ("default_blip_color_index" 
       "COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY_BULLET, COCO_BLIP_EMY_BULLET, COCO_BLIP_EMY_BULLET )")))

(c-define-method "hostile_collision" "bullet"
                 "coco_bullet_hostile_collision")

(c-define-method "sobject_update_state" "bullet" 
                 "coco_bullet_update_state")

(c-define-method "kaboom" "bullet" 
                 "coco_bullet_kaboom")

(c-define-class "rifle_bullet" ("bullet")
  (("thickness"       "1")
   ("muzzle_velocity" "COCO_TANK_INFANTRY_BULLET_SPEED")
   ("lifespan"        "COCO_BULLET_LIFESPAN")
   ("spark_count"     "0")))

(c-define-method "effective_damage" "rifle_bullet" 
                 "coco_flesh_damageable_effective_damage")

(c-define-class "tank_bullet" ("bullet")
  (("damage"          "COCO_BULLET_DAMAGE<<1")
   ("thickness"       "3")
   ("muzzle_velocity" "COCO_TANK_INFANTRY_BULLET_SPEED")
   ("lifespan"        "COCO_BULLET_LIFESPAN")
   ("spark_count"     "4")))

(c-define-class "bomb"   ("munition")
  (("horiz_ticks"  "0"         "unsigned char")
   ("raster_index" "(COCO_SOBJECT_FACING( inst )>0) ? 4 : 0")
   ("dx_max"       "COCO_GNUM_MAX")
   ("dy_max"       "COCO_GNUM_MAX")
   ("y_force"      "COCO_GRAVITY_ACCELL")
   ("rasters"      "coco_bomb_rasters")
   ("raster_count" "COCO_BOMB_RASTER_COUNT")
   ("damage"       "COCO_BOMB_DAMAGE")
   ("canonical_w"   "COCO_BOMB_SOUTH_RASTER_W")))

(c-define-method "sobject_update_state" "bomb" 
                 "coco_bomb_update_state")

(c-define-method "kaboom" "bomb"
                 "coco_bomb_kaboom")

;; supertype of missiles and rockets.
(c-define-class "abstract_missile" ("munition")
  (("angle"        :arg)
   ("w"            "11")
   ("h"            "11")
   ("canonical_w"  "11")
   ("phase"        "COCO_SOLID")
   ("dx_max"       "COCO_GNUM_MAX")
   ;; huh? Pretty sure dy_max isn't used...  So why's it here?:
   ("dy_max"       "COCO_COPTER_DY_MAX + (COCO_COPTER_DY_MAX<<1)")
   ;; ("y_force"      "COCO_GRAVITY_ACCELL") ; not used
   #+missiles-as-rasters ("rasters"      "coco_missile_rasters")
   #+missiles-as-rasters ("raster_count" "COCO_MISSILE_RASTER_COUNT")
   ("damage"       "COCO_MISSILE_DAMAGE")
   ("shrapnel_count" "1")
   ("cobject_draw" "coco_missile_draw")
   ("rotate_p"    "COCO_TRUE")))

(c-define-method "effective_damage" "abstract_missile" 
                 "coco_flesh_damageable_effective_damage")

(c-define-class "rocket" ("abstract_missile")
  (("dx_max"          "600/COCO_IDEAL_FRAMES_PER_SECOND")))

(c-define-method "kaboom" "rocket"
                 "coco_rocket_kaboom")

(c-define-method "sobject_update_state" "rocket"
                 "coco_rocket_update_state")

(c-define-class "missile" ("abstract_missile")
  (("fuel"         "5*COCO_IDEAL_FRAMES_PER_SECOND" "unsigned int")
   ("target_index" :arg        "int")))

(c-define-method "kaboom" "missile"         "coco_airbursting_kaboom")

(c-define-method "sobject_unlink" "missile" "coco_missile_unlink")

(c-define-method "sobject_update_state" "missile" 
                 "coco_missile_update_state")

(c-define-class "plasma" ("munition")
  (("damage"       "COCO_PLASMA_DAMAGE")
   ("radius"       "COCO_SOBJECT_CANONICAL_W( inst )>>1" "unsigned char")
   ("coherent"    "COCO_TRUE"  "coco_bool")
   ("cobject_draw" "coco_plasma_draw")
   ("canonical_w"  "12")
   ("canonical_h"  "12")
   ("w"            "COCO_SOBJECT_CANONICAL_W( inst )")
   ("h"            "COCO_SOBJECT_CANONICAL_H( inst )")
   ("default_blip_color_index" 
       "COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_FRY_MUNITION, COCO_BLIP_EMY_MUNITION, COCO_BLIP_EMY_MUNITION )")
   ("facing"       "0")
   ("rasters"      "coco_plasma_rasters")
   ("raster_count" "COCO_PLASMA_RASTER_COUNT")
   ("raster_index" "nationality")
   ("collision_priority" "COCO_MUNITION_COLLISION_PRIORITY - 1")
   ("health_max"   "COCO_NIGH_INVULNERABLE")
   ("z"            "2")
   ("phase"       "COCO_SOLID") ;; *snort* how ironic
   ("cobject_guess_nationality_fn" "coco_raster_guess_nationality")))


(c-define-method "sobject_to_cobject" "plasma"
                 "coco_plasma_to_cobject")

(c-define-method "sobject_update_state" "plasma" 
                 "coco_plasma_update_state")

(c-define-method "hostile_collision" "plasma"
                 "coco_plasma_hostile_collision")

(c-define-method "friendly_collision" "plasma"
                 "coco_plasma_friendly_collision")

(c-define-method "ground_collide" "plasma" 
                 "coco_plasma_ground_collide")

(c-define-class "spark" ("munition")
  (("phase"        "COCO_VAPOR")
   ("canonical_w"  "1")
   ("w"            "1")
   ("h"            "1")
   ("dx"           "0")
   ("dy"           "0")
   ("facing"       "0")
   ("nationality"  "COCO_NEUTRAL")
   ("spark_dx"     :arg  "signed char")
   ("spark_dy"     :arg  "signed char")
   ("cobject_draw" "coco_spark_draw")
   ("z"            "2") ; bullets are the weap of choice against clouds
   ("default_blip_color_index" 
       "COCO_BLIP_TRIPLET_TRANSPARENT")))

(c-define-method "ground_collide" "spark" "coco_ignore_ground_collide")

(c-define-method "sobject_update_state" "spark"
                 "coco_spark_update_state")

(c-define-class "shrapnel" ("munition")
  (("facing"       "0")
   ("phase"        "COCO_EXPLOSIVE")
   ("nationality"  "COCO_NEUTRAL")
   ("dx_max"       "COCO_GNUM_MAX")
   ("dy_max"       "COCO_GNUM_MAX")
   ("y_force"      "COCO_GRAVITY_ACCELL")
   ("damage"       "COCO_SHRAPNEL_DAMAGE")
   ("shrapnel_count" "0")
   ("canonical_w"  "24")
   ("default_blip_color_index" 
       "COCO_BLIP_TRIPLET_TRANSPARENT")
   ("cobject_guess_nationality_fn" "coco_neutral_guess_nationality")))

(c-define-method "sobject_update_state" "shrapnel"
                 "coco_shrapnel_update_state")

; (c-define-class "fireball_shrapnel" ("shrapnel")
;   (("rasters"      "coco_fireball_shrapnel_rasters")
;    ("raster_count" "COCO_FIREBALL_SHRAPNEL_RASTER_COUNT")
;    ("canonical_w"  "COCO_FIREBALL_24A_RASTER_W")
;    ("default_blip_color_index" 
;        "COCO_BLIP_TRIPLET_TRANSPARENT")))

(c-define-class "metal_shrapnel" ("shrapnel")
  (("color_index"   "COCO_INDEX_RED" "coco_color_index")
   ("cobject_draw" "coco_metal_shrapnel_draw")))

(c-define-method "sobject_update_state" "metal_shrapnel"
                 "coco_metal_shrapnel_update_state")

(c-define-class "big_metal_shrapnel" ("metal_shrapnel")
  (("rasters"      "coco_big_metal_shrapnel_rasters")
   ("raster_count" "COCO_BIG_METAL_RASTER_COUNT")
   ("canonical_w"  "COCO_SHRAPNEL_32A_RASTER_W")))

(c-define-class "med_metal_shrapnel" ("metal_shrapnel")
  (("rasters"      "coco_med_metal_shrapnel_rasters")
   ("raster_count" "COCO_MED_METAL_RASTER_COUNT")
   ("canonical_w"  "COCO_SHRAPNEL_24A_RASTER_W")))

(c-define-class "small_metal_shrapnel" ("metal_shrapnel")
  (("rasters"      "coco_small_metal_shrapnel_rasters")
   ("raster_count" "COCO_SMALL_METAL_RASTER_COUNT")
   ("canonical_w"  "COCO_SHRAPNEL_16A_RASTER_W")))

(c-define-class "explosion" ("munition")
  (("rasters"      "coco_explosion_rasters")
   ("raster_count" "COCO_EXPLOSION_RASTER_COUNT")
   ("raster_index" :arg)
   ("phase"        "COCO_EXPLOSIVE")
   ("damage"       "0")
   ("z"            "1")
   ("nationality"  "COCO_NEUTRAL")
   ("cobject_guess_nationality_fn" "coco_neutral_guess_nationality")
   ("health"       "COCO_NIGH_INVULNERABLE")
   ("default_blip_color_index" 
       "COCO_MAKE_BLIP_TRIPLET( COCO_BLIP_EXPLOSION, COCO_BLIP_EXPLOSION, COCO_BLIP_EXPLOSION )")))

(c-define-method "ground_collide" "explosion" "coco_ignore_ground_collide")

(c-define-method "sobject_update_state" "explosion"
                 "coco_explosion_update_state")

(c-define-method "sobject_to_cobject" "metal_shrapnel"
                 "coco_metal_shrapnel_to_cobject")

(c-define-method "sobject_to_cobject" "balloon_line"
                 "coco_balloon_line_to_cobject")

(c-define-method "sobject_to_cobject" "ground_unit" 
                 "coco_ground_rasteroid_to_cobject")

(c-define-method "sobject_to_cobject" "infantry" 
                 "coco_rasteroid_to_cobject")

(c-define-method "sobject_to_cobject" "gun"
                 "coco_gun_tank_to_cobject")

(c-define-method "sobject_to_cobject" "tank"
                 "coco_gun_tank_to_cobject")

(c-define-method "sobject_to_cobject" "abstract_missile"
                 "coco_y_angle_to_cobject")

(c-define-method "sobject_to_cobject" "copter"
                 "coco_y_angle_to_cobject")

(c-define-method "sobject_to_cobject" "spark"
                 "coco_spark_to_cobject")

(c-define-generic-function "raster_flip_index" "sobject" "unsigned char"
                           (("index" "unsigned char")))

(c-define-method "raster_flip_index" "sobject" 
                 "coco_every_sobject_raster_flip_index")

(c-define-method "raster_flip_index" "copter" 
                 "coco_copter_raster_flip_index")

(c-define-generic-function "toggle_facing" "sobject" "void" ())
(c-define-method "toggle_facing" "sobject" "coco_every_sobject_toggle_facing")
(c-define-method "toggle_facing" "copter" "coco_copter_toggle_facing")


(c-define-generic-function "fire_bullet_coords" "sobject" "void"
                           (("target" "coco_sobject *")
                            ("dx_return" "coco_gnum *")
                            ("dy_return" "coco_gnum *")
                            ("x_return" "coco_gnum *")
                            ("y_return" "coco_gnum *")))
(c-define-method "fire_bullet_coords" "copter" "coco_copter_fire_bullet_coords")
(c-define-method "fire_bullet_coords" "gun" "coco_gun_fire_bullet_coords")

(c-define-method "fire_bullet_coords" "tank" 
                 "coco_tank_infantry_fire_bullet_coords")

(c-define-method "fire_bullet_coords" "infantry" 
                 "coco_tank_infantry_fire_bullet_coords")

(c-define-method "fire_bullet_coords" "bunker" 
                 "coco_bunker_fire_bullet_coords")


(c-define-generic-function "fire_bullet" "sobject" "coco_sobject *"
                           (("sstate" "coco_sstate *")
                            ("target_or_null" "coco_sobject *")))

(c-define-method "fire_bullet" "sobject" "coco_every_sobject_fire_bullet")
(c-define-method "fire_bullet" "copter" "coco_copter_fire_bullet")

(c-define-generic-function "launch_missile" "sobject" "coco_sobject *"
                           (("sstate" "coco_sstate *")
                            ("target" "coco_sobject *")))

(c-define-method "launch_missile" "sobject"
                 "coco_every_sobject_launch_missile")

(c-define-method "launch_missile" "firetruck"
                 "coco_firetruck_launch_missile")

(c-define-method "launch_missile" "headquarters"
                 "coco_headquarters_launch_missile")

(c-define-method "launch_missile" "copter"
                 "coco_copter_launch_missile")

(c-define-generic-function "drop_paratrooper" "sobject" "coco_sobject *"
                           (("sstate" "coco_sstate *")))

(c-define-method "drop_paratrooper" "sobject" 
                 "coco_every_sobject_drop_paratrooper")

(c-define-method "drop_paratrooper" "copter" 
                 "coco_copter_drop_paratrooper")

(c-define-method "drop_paratrooper" "inviso_camera" 
                 "coco_inviso_camera_drop_paratrooper")
