/*
 * Cogl
 *
 * A Low Level GPU Graphics and Utilities API
 *
 * Copyright (C) 2007,2008,2009 Intel Corporation.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *
 */

#pragma once

#include "cogl/cogl-renderer.h"
#include "cogl/cogl-scanout.h"

#ifdef HAVE_X11
#include <X11/Xutil.h>
#include "cogl/winsys/cogl-texture-pixmap-x11-private.h"
#endif

#ifdef HAVE_EGL
#include "cogl/cogl-egl-private.h"
#endif

#include "cogl/cogl-poll.h"

COGL_EXPORT uint32_t
_cogl_winsys_error_quark (void);

#define COGL_WINSYS_ERROR (_cogl_winsys_error_quark ())

typedef enum /*< prefix=COGL_WINSYS_ERROR >*/
{
  COGL_WINSYS_ERROR_INIT,
  COGL_WINSYS_ERROR_CREATE_CONTEXT,
  COGL_WINSYS_ERROR_CREATE_ONSCREEN,
  COGL_WINSYS_ERROR_MAKE_CURRENT,
} CoglWinsysError;

typedef struct _CoglWinsysVtable
{
  CoglWinsysID id;
  CoglRendererConstraint constraints;

  const char *name;

  /* Required functions */

  GCallback
  (*renderer_get_proc_address) (CoglRenderer *renderer,
                                const char   *name);

  gboolean
  (*renderer_connect) (CoglRenderer *renderer,
                       GError      **error);

  void
  (*renderer_disconnect) (CoglRenderer *renderer);

  void
  (*renderer_outputs_changed) (CoglRenderer *renderer);

  gboolean
  (*display_setup) (CoglDisplay *display,
                    GError     **error);

  void
  (*display_destroy) (CoglDisplay *display);

  CoglDmaBufHandle *
  (*renderer_create_dma_buf) (CoglRenderer     *renderer,
                              CoglPixelFormat   format,
                              uint64_t         *modifiers,
                              int               n_modifiers,
                              int               width,
                              int               height,
                              GError          **error);

  gboolean
  (*renderer_is_dma_buf_supported) (CoglRenderer *renderer);

  void
  (*renderer_bind_api) (CoglRenderer *renderer);

  gboolean
  (*context_init) (CoglContext *context,
                   GError     **error);

  void
  (*context_deinit) (CoglContext *context);

  /* Optional functions */

#ifdef HAVE_X11
  gboolean
  (*texture_pixmap_x11_create) (CoglTexturePixmapX11 *tex_pixmap);
  void
  (*texture_pixmap_x11_free) (CoglTexturePixmapX11 *tex_pixmap);

  gboolean
  (*texture_pixmap_x11_update) (CoglTexturePixmapX11       *tex_pixmap,
                                CoglTexturePixmapStereoMode stereo_mode,
                                gboolean                    needs_mipmap);

  void
  (*texture_pixmap_x11_damage_notify) (CoglTexturePixmapX11 *tex_pixmap);

  CoglTexture *
  (*texture_pixmap_x11_get_texture) (CoglTexturePixmapX11       *tex_pixmap,
                                     CoglTexturePixmapStereoMode stereo_mode);
#endif

  void *
  (*fence_add) (CoglContext *ctx);

  gboolean
  (*fence_is_complete) (CoglContext *ctx,
                        void        *fence);

  void
  (*fence_destroy) (CoglContext *ctx,
                    void        *fence);

  void
  (*update_sync) (CoglContext *ctx);

  int
  (*get_sync_fd) (CoglContext *ctx);

} CoglWinsysVtable;

typedef const CoglWinsysVtable *(*CoglWinsysVtableGetter) (void);

gboolean
_cogl_winsys_has_feature (CoglWinsysFeature feature);
