/*
 * Wayland Support
 *
 * Copyright (C) 2015 Red Hat
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Carlos Garnacho <carlosg@gnome.org>
 */

#pragma once

#include <glib.h>
#include <wayland-server.h>

#include "clutter/clutter.h"
#include "wayland/meta-wayland-types.h"

gboolean meta_wayland_pointer_gesture_pinch_handle_event (MetaWaylandPointer *pointer,
                                                          const ClutterEvent *event);

void meta_wayland_pointer_gesture_pinch_create_new_resource (MetaWaylandPointer *pointer,
                                                             struct wl_client   *client,
                                                             struct wl_resource *gestures_resource,
                                                             uint32_t            id);

void meta_wayland_pointer_gesture_pinch_cancel (MetaWaylandPointer *pointer,
                                                uint32_t            serial);
