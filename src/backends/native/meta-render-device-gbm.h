/*
 * Copyright (C) 2021 Red Hat Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include "backends/native/meta-render-device-private.h"

#define META_TYPE_RENDER_DEVICE_GBM (meta_render_device_gbm_get_type ())
G_DECLARE_FINAL_TYPE (MetaRenderDeviceGbm, meta_render_device_gbm,
                      META, RENDER_DEVICE_GBM,
                      MetaRenderDevice)

MetaRenderDeviceGbm * meta_render_device_gbm_new (MetaBackend     *backend,
                                                  MetaDeviceFile  *device_file,
                                                  GError         **error);

struct gbm_device * meta_render_device_gbm_get_gbm_device (MetaRenderDeviceGbm *render_device_gbm);
