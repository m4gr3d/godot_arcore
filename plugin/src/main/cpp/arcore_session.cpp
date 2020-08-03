/**
* Created by Fredia Huya-Kouadio.
*/

#include "arcore_session.h"

#include <gen/ARVRServer.hpp>
#include <gen/CameraServer.hpp>
#include <gen/OS.hpp>
#include <gen/MainLoop.hpp>

namespace gdarcore {

ARCoreSession* ARCoreSession::singleton_instance = nullptr;

ARCoreSession* ARCoreSession::get_singleton_instance() {
  if (singleton_instance == nullptr) {
    singleton_instance = new ARCoreSession();
  }
  return singleton_instance;
}

void ARCoreSession::delete_singleton_instance() {
  delete singleton_instance;
  singleton_instance = nullptr;
}

ARCoreSession::ARCoreSession() {
  ar_session = nullptr;
  ar_frame = nullptr;
  init_status = NOT_INITIALISED;
  width = 1;
  height = 1;
  display_rotation = 0;
  camera_texture_id = 0;
  last_anchor_id = 0;
  z_near = 0.01;
  z_far = 1000.0;
  have_display_transform = false;
  projection.set_perspective(60.0, 1.0, z_near, z_far, false); // this is just a default, will be changed by ARCore
}

ARCoreSession::~ARCoreSession() {
  if (is_initialized()) {
    uninitialize();
  }
}

bool ARCoreSession::is_initialized() const {
  // if we're in the process of initialising we treat this as initialised...
  return (init_status != NOT_INITIALISED) && (init_status != INITIALISE_FAILED);
}

bool ARCoreSession::initialize() {
  // TODO we may want to check for status PAUZED and just call resume (if we decide to implement that)

  if (init_status == INITIALISE_FAILED) {
    // if we fully failed last time, don't try again..
    return false;
  } else if (init_status == NOT_INITIALISED) {
    godot::Godot::print("Godot ARCore: Initialising...");
    init_status = START_INITIALISE;

    // create our camera feed
    if (feed.is_null()) {
      godot::Godot::print("Godot ARCore: Creating camera feed...");

      feed.instance();
      feed->_set_name("ARCore");
      feed->set_active(true);

      godot::CameraServer *cs = godot::CameraServer::get_singleton();
      if (cs != nullptr) {
        cs->add_feed(feed);

        godot::Godot::print("Godot ARCore: Feed " + godot::String::num_int64(feed->get_id()) + " added");
      }
    }

    if (ar_session == nullptr) {
      godot::Godot::print("Godot ARCore: Getting environment");

      // get some android things
      JNIEnv *env = godot::android_api->godot_android_get_env();

      jobject context = godot::android_api->godot_android_get_activity();
      if (context == nullptr) {
        godot::Godot::print("Godot ARCore: Couldn't get context");
        init_status = INITIALISE_FAILED; // don't try again.
        return false;
      }

      godot::Godot::print("Godot ARCore: Create ArSession");

      if (ArSession_create(env, context, &ar_session) != AR_SUCCESS || ar_session == nullptr) {
        godot::Godot::print("Godot ARCore: ARCore couldn't be created.");
        init_status = INITIALISE_FAILED; // don't try again.
        return false;
      }

      godot::Godot::print("Godot ARCore: Create ArFrame.");

      ArFrame_create(ar_session, &ar_frame);
      if (ar_frame == nullptr) {
        godot::Godot::print("Godot ARCore: Frame couldn't be created.");

        ArSession_destroy(ar_session);
        ar_session = nullptr;

        init_status = INITIALISE_FAILED; // don't try again.
        return false;
      }

      // Get our size, make sure we have these in portrait
      godot::Size2 size = godot::OS::get_singleton()->get_window_size();
      if (size.x > size.y) {
        width = size.y;
        height = size.x;
      } else {
        width = size.x;
        height = size.y;
      }

      // Trigger display rotation
      display_rotation = -1;

      godot::Godot::print("Godot ARCore: Initialised.");
      init_status = INITIALISED;
    }

    // and call resume for the first time to complete this
    resume();

    if (init_status != INITIALISE_FAILED) {
      // make sure our feed is marked as active if we already have one...
      if (feed != nullptr) {
        feed->set_active(true);
      }
    }
  }

  return is_initialized();
}

void ARCoreSession::uninitialize() {
  if (is_initialized()) {
    // TODO we may want to call ArSession_pauze here and introduce a new status PAUZED
    // then move cleanup to our destruct.

    make_anchors_stale();
    remove_stale_anchors();

    if (ar_session != nullptr) {
      ArSession_destroy(ar_session);
      ArFrame_destroy(ar_frame);

      ar_session = nullptr;
      ar_frame = nullptr;
    }

    if (feed.is_valid()) {
      feed->set_active(false);

      godot::CameraServer *cs = godot::CameraServer::get_singleton();
      if (cs != nullptr) {
        cs->remove_feed(feed);
      }
      feed.unref();
      camera_texture_id = 0;
    }

    init_status = NOT_INITIALISED;
  }
}

void ARCoreSession::resume() {
  if (init_status == INITIALISED && ar_session != nullptr) {
    ArStatus status = ArSession_resume(ar_session);
    if (status != AR_SUCCESS) {
      godot::Godot::print("Godot ARCore: Failed to resume.");

      // TODO quit? how?
    }
  }
}

void ARCoreSession::notification(int p_what) {
  // Needs testing, this should now be called

  ARVRServer *arvr_server = ARVRServer::get_singleton();
  ERR_FAIL_NULL(arvr_server);

  switch (p_what) {
    case MainLoop::NOTIFICATION_APP_RESUMED: {
      if (is_initialized()) {
        resume();
      }
    }; break;
    case MainLoop::NOTIFICATION_APP_PAUSED:
      if (is_initialized()) {
        pause();
      }
      break;
    default:
      break;
  }
}

int ARCoreSession::get_camera_feed_id() {
  if (feed.is_valid()) {
    return feed->get_id();
  } else {
    return 0;
  }
}

Size2 ARCoreSession::get_render_targetsize() {
  Size2 target_size = OS::get_singleton()->get_window_size();
  return target_size;
}

void ARCoreSession::pause() {
  if (ar_session != nullptr) {
    ArSession_pause(ar_session);
  }
}

void ARCoreSession::make_anchors_stale() {
  for (const auto & anchor : anchors) {
    anchor.second->stale = true;
  }
}

void ARCoreSession::remove_stale_anchors() {
  // back to forth so when we remove entries we don't screw up...
  for (int i = anchors.size() - 1; i >= 0; i--) {
    ArPlane *ar_plane = anchors.getk(i);
    anchor_map *am = anchors.getv(i);
    if (am->stale) {
      anchors.erase(ar_plane); // no erase on i?

      ARVRServer::get_singleton()->remove_tracker(am->tracker);
      memdelete(am->tracker);
      memdelete(am);
      ArTrackable_release(ArAsTrackable(ar_plane));
    }
  }
}

}  // namespace gdarcore
