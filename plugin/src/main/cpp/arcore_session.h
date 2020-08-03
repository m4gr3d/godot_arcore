/**
* Created by Fredia Huya-Kouadio.
*/

#ifndef GDARCORE_ARCORE_SESSION_H
#define GDARCORE_ARCORE_SESSION_H

#include <map>

#include <core/Ref.hpp>
#include <gen/CameraFeed.hpp>
#include <util/camera_matrix.h>
#include <gen/ARVRPositionalTracker.hpp>
#include "include/arcore_c_api.h"

namespace gdarcore {

class ARCoreSession {
 public:
  enum InitStatus {
    NOT_INITIALISED, // We're not initialised
    START_INITIALISE, // We just started our initialise process
    INITIALISED, // Yeah! we are up and running
    INITIALISE_FAILED // We failed to initialise
  };

  static ARCoreSession *get_singleton_instance();
  static void delete_singleton_instance();

  bool is_initialized() const;
  bool initialize();
  void uninitialize();

  void resume();
  void pause();

  void notification(int p_what);

  int get_camera_feed_id();

  virtual Size2 get_render_targetsize();

 private:
  ARCoreSession();
  ~ARCoreSession();

  static ARCoreSession* singleton_instance;

  godot::Ref<godot::CameraFeed> feed;

  InitStatus init_status;
  ArSession *ar_session;
  ArFrame *ar_frame;
  int width;
  int height;
  int display_rotation;
  uint64_t camera_texture_id;
  uint64_t last_anchor_id;

  CameraMatrix projection;
  float z_near, z_far;
  bool have_display_transform;

  struct anchor_map {
    ARVRPositionalTracker *tracker;
    bool stale;
  };

  std::map<ArPlane *, anchor_map *> anchors;
  void make_anchors_stale();
  void remove_stale_anchors();
};

}  // namespace gdarcore

#endif // GDARCORE_ARCORE_SESSION_H
