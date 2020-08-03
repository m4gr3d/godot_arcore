/**
* Created by Fredia Huya-Kouadio.
*/

#ifndef ARCORE_ARVR_INTERFACE_H
#define ARCORE_ARVR_INTERFACE_H

#include <gdnative_api_struct.gen.h>
#include <arcore_session.h>

typedef struct arvr_data_struct {
  gdarcore::ARCoreSession *arcore_session;
} arvr_data_struct;

void *godot_arvr_constructor(godot_object *p_instance);

void godot_arvr_destructor(void *p_data);

godot_string godot_arvr_get_name(const void *p_data);

godot_int godot_arvr_get_capabilities(const void *p_data);

godot_int godot_arvr_get_camera_feed_id(void *p_data);

godot_bool godot_arvr_get_anchor_detection_is_enabled(const void *p_data);

void godot_arvr_set_anchor_detection_is_enabled(void *p_data, bool p_enable);

godot_bool godot_arvr_is_stereo(const void *p_data);

godot_bool godot_arvr_is_initialized(const void *p_data);

godot_bool godot_arvr_initialize(void *p_data);

void godot_arvr_uninitialize(void *p_data);

godot_vector2 godot_arvr_get_render_targetsize(const void *p_data);

godot_transform godot_arvr_get_transform_for_eye(void *p_data, godot_int p_eye, godot_transform *p_cam_transform);

void godot_arvr_fill_projection_for_eye(void *p_data, godot_real *p_projection, godot_int p_eye, godot_real p_aspect,
                                        godot_real p_z_near, godot_real p_z_far);

godot_int godot_arvr_get_external_texture_for_eye(void *p_data, godot_int p_eye);

void godot_arvr_commit_for_eye(void *p_data, godot_int p_eye, godot_rid *p_render_target, godot_rect2 *p_screen_rect);

void godot_arvr_process(void *p_data);

void godot_arvr_notification(void *p_data, godot_int p_what);

const godot_arvr_interface_gdnative interface_struct = {
    { GODOTVR_API_MAJOR,
      GODOTVR_API_MINOR },
    godot_arvr_constructor,
    godot_arvr_destructor,
    godot_arvr_get_name,
    godot_arvr_get_capabilities,
    godot_arvr_get_anchor_detection_is_enabled,
    godot_arvr_set_anchor_detection_is_enabled,
    godot_arvr_is_stereo,
    godot_arvr_is_initialized,
    godot_arvr_initialize,
    godot_arvr_uninitialize,
    godot_arvr_get_render_targetsize,
    godot_arvr_get_transform_for_eye,
    godot_arvr_fill_projection_for_eye,
    godot_arvr_commit_for_eye,
    godot_arvr_process,
    godot_arvr_get_external_texture_for_eye,
    godot_arvr_notification,
    godot_arvr_get_camera_feed_id
};

#endif // ARCORE_ARVR_INTERFACE_H
