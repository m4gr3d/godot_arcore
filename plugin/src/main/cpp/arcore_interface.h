/*************************************************************************/
/*  arcore_interface.h                                                   */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2019 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2019 Godot Engine contributors (cf. AUTHORS.md)    */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#ifndef ARCORE_INTERFACE_H
#define ARCORE_INTERFACE_H

#include "ARVRInterface.hpp"
#include "ARVRPositionalTracker.hpp"
#include "CameraFeed.hpp"

#include "include/arcore_c_api.h"

/**
	@author Bastiaan Olij <mux213@gmail.com>, Robert Hofstra <robert.hofstra@knowlogy.nl>
	ARCore interface between Android and Godot
**/

namespace gdarcore {
namespace {
using namespace godot;
}

class GodotJavaWrapper;

class ARCoreInterface : public ARVRInterface {
	GDCLASS(ARCoreInterface, ARVRInterface);

public:
	enum InitStatus {
		NOT_INITIALISED, // We're not initialised
		START_INITIALISE, // We just started our initialise process
		INITIALISED, // Yeah! we are up and running
		INITIALISE_FAILED // We failed to initialise
	};

private:
	InitStatus init_status;
	GodotJavaWrapper *godot_java;

	ArSession *ar_session;
	ArFrame *ar_frame;
	int width;
	int height;
	int display_rotation;
	uint camera_texture_id;
	uint last_anchor_id;

	Ref<CameraFeed> feed;
	bool feed_was_setup;

	Transform view;
	CameraMatrix projection;
	float z_near, z_far;
	bool have_display_transform;

protected:

public:

	virtual CameraMatrix get_projection_for_eye(ARVRInterface::Eyes p_eye, real_t p_aspect, real_t p_z_near, real_t p_z_far);
	virtual void commit_for_eye(ARVRInterface::Eyes p_eye, RID p_render_target, const Rect2 &p_screen_rect);

	virtual void process();

	ARCoreInterface();
	~ARCoreInterface();
};
}  // namespace gdarcore

#endif /* !ARCORE_INTERFACE_H */
