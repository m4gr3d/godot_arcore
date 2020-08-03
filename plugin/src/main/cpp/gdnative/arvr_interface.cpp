/**
* Created by Fredia Huya-Kouadio.
*/

#include <core/GodotGlobal.hpp>
#include <gen/ARVRInterface.hpp>
#include <gen/ARVRServer.hpp>
#include <gen/MainLoop.hpp>
#include "arvr_interface.h"

namespace {
const char *kName = "ARCore";
}  // namespace

godot_string godot_arvr_get_name(const void *p_data) {
  godot_string ret;

  godot::api->godot_string_new(&ret);
  godot::api->godot_string_parse_utf8(&ret, kName);

  return ret;
}

godot_int godot_arvr_get_capabilities(const void *p_data) {
  return godot::ARVRInterface::ARVR_MONO + godot::ARVRInterface::ARVR_AR;
}

godot_int godot_arvr_get_camera_feed_id(void *p_data) {
  auto* arvr_data = (arvr_data_struct *) p_data;
  return arvr_data->arcore_session->get_camera_feed_id();
}

godot_bool godot_arvr_get_anchor_detection_is_enabled(const void *p_data) {
  // does not apply here
  return false;
}

godot_vector2 godot_arvr_get_render_targetsize(const void *p_data) {
  auto* arvr_data = (arvr_data_struct *) p_data;

  Size2 render_target_size = arvr_data->arcore_session->get_render_targetsize();
  godot_vector2 godot_size;
  godot::api->godot_vector2_new(&godot_size, render_target_size.width, render_target_size.height);
  return godot_size;
}

godot_transform godot_arvr_get_transform_for_eye(void *p_data, godot_int p_eye, godot_transform *p_cam_transform) {
  auto* arvr_data = (arvr_data_struct *) p_data;
}

void godot_arvr_set_anchor_detection_is_enabled(void *p_data, bool p_enable) {
  // we ignore this, not supported in this interface!
}

void godot_arvr_notification(void *p_data, godot_int p_what) {
  auto* arvr_data = (arvr_data_struct*) p_data;
  arvr_data->arcore_session->notification(p_what);
}

godot_bool godot_arvr_is_initialized(const void *p_data) {
  auto* arvr_data = (arvr_data_struct *) p_data;
  return arvr_data->arcore_session->is_initialized();
}

godot_bool godot_arvr_initialize(void *p_data) {
  auto *arvr_data = (arvr_data_struct *)p_data;
  return arvr_data->arcore_session->initialize();
}

void godot_arvr_uninitialize(void *p_data) {
  auto* arvr_data = (arvr_data_struct*) p_data;
  arvr_data->arcore_session->uninitialize();
}

godot_int godot_arvr_get_external_texture_for_eye(void *p_data, godot_int p_eye) {
  return 0;
}

godot_bool godot_arvr_is_stereo(const void *p_data) {
  return false;
}

void *godot_arvr_constructor(godot_object *p_instance) {
  auto *arvr_data =
      (arvr_data_struct *)godot::api->godot_alloc(sizeof(arvr_data_struct));

  arvr_data->arcore_session = gdarcore::ARCoreSession::get_singleton_instance();

  return arvr_data;
};

void godot_arvr_destructor(void *p_data) {
  if (p_data != nullptr) {
    gdarcore::ARCoreSession::delete_singleton_instance();
    godot::api->godot_free(p_data);
  }
}
