/**
* Created by Fredia Huya-Kouadio.
*/

#include <core/GodotGlobal.hpp>
#include "arvr_interface.h"
#include "godot_arcore.h"

void GDN_EXPORT godot_arcore_gdnative_init(godot_gdnative_init_options *options) {
  godot::Godot::gdnative_init(options);
}

void GDN_EXPORT godot_arcore_gdnative_singleton() {
  if (godot::arvr_api != nullptr) {
    godot::arvr_api->godot_arvr_register_interface(&interface_struct);
  }
}

void GDN_EXPORT godot_arcore_nativescript_init(void *handle) {
  godot::Godot::nativescript_init(handle);
}

void GDN_EXPORT godot_arcore_nativescript_terminate(void *handle) {
  godot::Godot::nativescript_terminate(handle);
}

void GDN_EXPORT godot_arcore_gdnative_terminate(godot_gdnative_terminate_options *options) {
  godot::Godot::gdnative_terminate(options);
}
