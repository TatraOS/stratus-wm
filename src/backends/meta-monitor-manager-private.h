/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */

/*
 * Copyright (C) 2001 Havoc Pennington
 * Copyright (C) 2003 Rob Adams
 * Copyright (C) 2004-2006 Elijah Newren
 * Copyright (C) 2013 Red Hat Inc.
 * Copyright (C) 2020 NVIDIA CORPORATION
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
 */

#pragma once

#include "cogl/cogl.h"
#include <graphene.h>

#ifdef HAVE_GNOME_DESKTOP
#include <libgnome-desktop/gnome-pnp-ids.h>
#endif

#include "backends/meta-backend-private.h"
#include "backends/meta-crtc.h"
#include "backends/meta-cursor.h"
#include "backends/meta-display-config-shared.h"
#include "backends/meta-monitor-transform.h"
#include "backends/meta-viewport-info.h"
#include "core/util-private.h"
#include "meta/display.h"
#include "meta/meta-monitor-manager.h"

#define META_MONITOR_MANAGER_MIN_SCREEN_WIDTH 640
#define META_MONITOR_MANAGER_MIN_SCREEN_HEIGHT 480

typedef enum _MetaMonitorManagerCapability
{
  META_MONITOR_MANAGER_CAPABILITY_NONE = 0,
  META_MONITOR_MANAGER_CAPABILITY_LAYOUT_MODE = (1 << 0),
  META_MONITOR_MANAGER_CAPABILITY_GLOBAL_SCALE_REQUIRED = (1 << 1)
} MetaMonitorManagerCapability;

/* Equivalent to the 'method' enum in org.gnome.Mutter.DisplayConfig */
typedef enum _MetaMonitorsConfigMethod
{
  META_MONITORS_CONFIG_METHOD_VERIFY = 0,
  META_MONITORS_CONFIG_METHOD_TEMPORARY = 1,
  META_MONITORS_CONFIG_METHOD_PERSISTENT = 2
} MetaMonitorsConfigMethod;

/* Equivalent to the 'layout-mode' enum in org.gnome.Mutter.DisplayConfig */
typedef enum _MetaLogicalMonitorLayoutMode
{
  META_LOGICAL_MONITOR_LAYOUT_MODE_LOGICAL = 1,
  META_LOGICAL_MONITOR_LAYOUT_MODE_PHYSICAL = 2
} MetaLogicalMonitorLayoutMode;

/* The source the privacy screen change has been triggered */
typedef enum
{
  META_PRIVACY_SCREEN_CHANGE_STATE_NONE,
  META_PRIVACY_SCREEN_CHANGE_STATE_INIT,
  META_PRIVACY_SCREEN_CHANGE_STATE_PENDING_HOTKEY,
  META_PRIVACY_SCREEN_CHANGE_STATE_PENDING_SETTING,
} MetaPrivacyScreenChangeState;

/*
 * MetaCrtcAssignment:
 *
 * A representation of a CRTC configuration, generated by
 * MetaMonitorConfigManager.
 */
struct _MetaCrtcAssignment
{
  MetaCrtc *crtc;
  MetaCrtcMode *mode;
  graphene_rect_t layout;
  MetaMonitorTransform transform;
  GPtrArray *outputs;

  gpointer backend_private;
  GDestroyNotify backend_private_destroy;
};

/*
 * MetaOutputAssignment:
 *
 * A representation of a connector configuration, generated by
 * MetaMonitorConfigManager.
 */
struct _MetaOutputAssignment
{
  MetaOutput *output;
  gboolean is_primary;
  gboolean is_presentation;
  gboolean is_underscanning;
  gboolean has_max_bpc;
  unsigned int max_bpc;
  unsigned int rgb_range;
};

/*
 * MetaOutputCtm:
 *
 * A 3x3 color transform matrix in the fixed-point S31.32 sign-magnitude format
 * used by DRM.
 */
typedef struct _MetaOutputCtm
{
  uint64_t matrix[9];
} MetaOutputCtm;

#define META_TYPE_MONITOR_MANAGER            (meta_monitor_manager_get_type ())
#define META_MONITOR_MANAGER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), META_TYPE_MONITOR_MANAGER, MetaMonitorManager))
#define META_MONITOR_MANAGER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  META_TYPE_MONITOR_MANAGER, MetaMonitorManagerClass))
#define META_IS_MONITOR_MANAGER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), META_TYPE_MONITOR_MANAGER))
#define META_IS_MONITOR_MANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  META_TYPE_MONITOR_MANAGER))
#define META_MONITOR_MANAGER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  META_TYPE_MONITOR_MANAGER, MetaMonitorManagerClass))

typedef struct _MetaDBusDisplayConfig MetaDBusDisplayConfig;

G_DEFINE_AUTOPTR_CLEANUP_FUNC (MetaMonitorManager, g_object_unref)

struct _MetaMonitorManager
{
  GObject parent_instance;

  MetaDBusDisplayConfig *display_config;

  MetaBackend *backend;

  /* XXX: this structure is very badly
     packed, but I like the logical organization
     of fields */

  gboolean in_init;
  unsigned int serial;

  MetaLogicalMonitorLayoutMode layout_mode;

  int screen_width;
  int screen_height;

  GList *monitors;

  GList *logical_monitors;
  MetaLogicalMonitor *primary_logical_monitor;

  guint dbus_name_id;
  guint restore_config_id;
  guint persistent_timeout_id;

  guint panel_orientation_managed : 1;

  MetaMonitorConfigManager *config_manager;

  MetaMonitorSwitchConfigType current_switch_config;

  MetaPrivacyScreenChangeState privacy_screen_change_state;
};

/**
 * MetaMonitorManagerClass:
 *
 * @read_edid: Returns the raw Extended Display Identification Data (EDID)
 *   for the given #MetaOutput object.
 *
 * @ensure_initial_config: Called on setup. Makes sure an initial config
 *   is loaded.
 *
 * @apply_monitors_config: Tries to apply the given config using the given
 *   method. Throws an error if something went wrong.
 *
 * @set_power_save_mode: Sets the #MetaPowerSave mode (for all displays).
 *
 * @change_backlight: Changes the backlight intensity to the given value (in
 *   percent).
 *
 * @tiled_monitor_added: Should be called by a #MetaMonitor when it is created.
 *
 * @tiled_monitor_removed: Should be called by a #MetaMonitor when it is
 *   destroyed.
 *
 * @is_transform_handled: vfunc for
 *   meta_monitor_manager_is_transform_handled().
 * @calculate_monitor_mode_scale: vfunc for
 *   meta_monitor_manager_calculate_monitor_mode_scale().
 * @calculate_supported_scales: vfunc for
 *   meta_monitor_manager_calculate_supported_scales().
 * @get_capabilities: vfunc for meta_monitor_manager_get_capabilities().
 * @get_max_screen_size: vfunc for meta_monitor_manager_get_max_screen_size().
 * @get_default_layout_mode: vfunc for meta_monitor_manager_get_default_layout_mode().
 * @set_output_ctm: vfunc for meta_monitor_manager_output_set_ctm()
 *
 * The base class for a #MetaMonitorManager.
 */
struct _MetaMonitorManagerClass
{
  GObjectClass parent_class;

  GBytes * (* read_edid) (MetaMonitorManager *manager,
                          MetaOutput         *output);

  void (* read_current_state) (MetaMonitorManager *manager);

  void (* ensure_initial_config) (MetaMonitorManager *manager);

  gboolean (* apply_monitors_config) (MetaMonitorManager        *manager,
                                      MetaMonitorsConfig        *config,
                                      MetaMonitorsConfigMethod   method,
                                      GError                   **error);

  void (* set_power_save_mode) (MetaMonitorManager *manager,
                                MetaPowerSave       power_save);

  void (* change_backlight) (MetaMonitorManager *manager,
                             MetaOutput         *output,
                             int                 backlight);

  void (* tiled_monitor_added) (MetaMonitorManager *manager,
                                MetaMonitor        *monitor);

  void (* tiled_monitor_removed) (MetaMonitorManager *manager,
                                  MetaMonitor        *monitor);

  gboolean (* is_transform_handled) (MetaMonitorManager   *manager,
                                     MetaCrtc             *crtc,
                                     MetaMonitorTransform  transform);

  float (* calculate_monitor_mode_scale) (MetaMonitorManager           *manager,
                                          MetaLogicalMonitorLayoutMode  layout_mode,
                                          MetaMonitor                  *monitor,
                                          MetaMonitorMode              *monitor_mode);

  float * (* calculate_supported_scales) (MetaMonitorManager           *manager,
                                          MetaLogicalMonitorLayoutMode  layout_mode,
                                          MetaMonitor                  *monitor,
                                          MetaMonitorMode              *monitor_mode,
                                          int                          *n_supported_scales);

  MetaMonitorManagerCapability (* get_capabilities) (MetaMonitorManager *manager);

  gboolean (* get_max_screen_size) (MetaMonitorManager *manager,
                                    int                *width,
                                    int                *height);

  MetaLogicalMonitorLayoutMode (* get_default_layout_mode) (MetaMonitorManager *manager);

  void (* set_output_ctm) (MetaOutput          *output,
                           const MetaOutputCtm *ctm);

  MetaVirtualMonitor * (* create_virtual_monitor) (MetaMonitorManager            *manager,
                                                   const MetaVirtualMonitorInfo  *info,
                                                   GError                       **error);
};

META_EXPORT_TEST
MetaBackend *       meta_monitor_manager_get_backend (MetaMonitorManager *manager);

void                meta_monitor_manager_setup (MetaMonitorManager *manager);

META_EXPORT_TEST
void                meta_monitor_manager_rebuild (MetaMonitorManager *manager,
                                                  MetaMonitorsConfig *config);

META_EXPORT_TEST
void                meta_monitor_manager_rebuild_derived (MetaMonitorManager *manager,
                                                          MetaMonitorsConfig *config);

META_EXPORT_TEST
int                 meta_monitor_manager_get_num_logical_monitors (MetaMonitorManager *manager);

META_EXPORT_TEST
GList *             meta_monitor_manager_get_logical_monitors (MetaMonitorManager *manager);

MetaLogicalMonitor *meta_monitor_manager_get_logical_monitor_from_number (MetaMonitorManager *manager,
                                                                          int                 number);

META_EXPORT_TEST
MetaLogicalMonitor *meta_monitor_manager_get_primary_logical_monitor (MetaMonitorManager *manager);

MetaLogicalMonitor *meta_monitor_manager_get_logical_monitor_at (MetaMonitorManager *manager,
                                                                 float               x,
                                                                 float               y);

MetaLogicalMonitor *meta_monitor_manager_get_logical_monitor_from_rect (MetaMonitorManager *manager,
                                                                        MtkRectangle       *rect);

MetaLogicalMonitor *meta_monitor_manager_get_highest_scale_monitor_from_rect (MetaMonitorManager *manager,
                                                                              MtkRectangle       *rect);

MetaLogicalMonitor *meta_monitor_manager_get_logical_monitor_neighbor (MetaMonitorManager  *manager,
                                                                       MetaLogicalMonitor  *logical_monitor,
                                                                       MetaDisplayDirection direction);

MetaMonitor *       meta_monitor_manager_get_primary_monitor (MetaMonitorManager *manager);

META_EXPORT_TEST
MetaMonitor *       meta_monitor_manager_get_laptop_panel (MetaMonitorManager *manager);

MetaMonitor *       meta_monitor_manager_get_monitor_from_spec (MetaMonitorManager *manager,
                                                                MetaMonitorSpec    *monitor_spec);

MetaMonitor *       meta_monitor_manager_get_monitor_from_connector (MetaMonitorManager *manager,
                                                                     const char         *connector);

META_EXPORT_TEST
GList *             meta_monitor_manager_get_monitors      (MetaMonitorManager *manager);

void                meta_monitor_manager_get_screen_size   (MetaMonitorManager *manager,
                                                            int                *width,
                                                            int                *height);

MetaPowerSave       meta_monitor_manager_get_power_save_mode (MetaMonitorManager *manager);

void                meta_monitor_manager_power_save_mode_changed (MetaMonitorManager        *manager,
                                                                  MetaPowerSave              mode,
                                                                  MetaPowerSaveChangeReason  reason);

void                meta_monitor_manager_confirm_configuration (MetaMonitorManager *manager,
                                                                gboolean            ok);

META_EXPORT_TEST
void               meta_monitor_manager_read_current_state (MetaMonitorManager *manager);

META_EXPORT_TEST
void               meta_monitor_manager_reconfigure (MetaMonitorManager *manager);

META_EXPORT_TEST
void               meta_monitor_manager_reload (MetaMonitorManager *manager);

gboolean           meta_monitor_manager_get_monitor_matrix (MetaMonitorManager *manager,
                                                            MetaMonitor        *monitor,
                                                            MetaLogicalMonitor *logical_monitor,
                                                            gfloat              matrix[6]);

void               meta_monitor_manager_tiled_monitor_added (MetaMonitorManager *manager,
                                                             MetaMonitor        *monitor);
void               meta_monitor_manager_tiled_monitor_removed (MetaMonitorManager *manager,
                                                               MetaMonitor        *monitor);

META_EXPORT_TEST
MetaMonitorsConfig * meta_monitor_manager_ensure_configured (MetaMonitorManager *manager);

META_EXPORT_TEST
void               meta_monitor_manager_update_logical_state (MetaMonitorManager *manager,
                                                              MetaMonitorsConfig *config);

META_EXPORT_TEST
void               meta_monitor_manager_update_logical_state_derived (MetaMonitorManager *manager,
                                                                      MetaMonitorsConfig *config);

META_EXPORT_TEST
void               meta_monitor_manager_lid_is_closed_changed (MetaMonitorManager *manager);

gboolean           meta_monitor_manager_is_headless (MetaMonitorManager *manager);

float              meta_monitor_manager_calculate_monitor_mode_scale (MetaMonitorManager           *manager,
                                                                      MetaLogicalMonitorLayoutMode  layout_mode,
                                                                      MetaMonitor                  *monitor,
                                                                      MetaMonitorMode              *monitor_mode);

float *            meta_monitor_manager_calculate_supported_scales (MetaMonitorManager          *,
                                                                    MetaLogicalMonitorLayoutMode ,
                                                                    MetaMonitor                 *,
                                                                    MetaMonitorMode             *,
                                                                    int                         *);

gboolean           meta_monitor_manager_is_scale_supported (MetaMonitorManager          *manager,
                                                            MetaLogicalMonitorLayoutMode layout_mode,
                                                            MetaMonitor                 *monitor,
                                                            MetaMonitorMode             *monitor_mode,
                                                            float                        scale);

MetaMonitorManagerCapability
                   meta_monitor_manager_get_capabilities (MetaMonitorManager *manager);

gboolean           meta_monitor_manager_get_max_screen_size (MetaMonitorManager *manager,
                                                             int                *max_width,
                                                             int                *max_height);

MetaLogicalMonitorLayoutMode
                   meta_monitor_manager_get_default_layout_mode (MetaMonitorManager *manager);

META_EXPORT_TEST
MetaVirtualMonitor * meta_monitor_manager_create_virtual_monitor (MetaMonitorManager            *manager,
                                                                  const MetaVirtualMonitorInfo  *info,
                                                                  GError                       **error);

META_EXPORT_TEST
MetaMonitorConfigManager *
                   meta_monitor_manager_get_config_manager (MetaMonitorManager *manager);

void meta_monitor_manager_rotate_monitor (MetaMonitorManager *manager);

gboolean meta_monitor_has_aspect_as_size (MetaMonitor *monitor);

static inline MetaOutputAssignment *
meta_find_output_assignment (MetaOutputAssignment **outputs,
                             unsigned int           n_outputs,
                             MetaOutput            *output)
{
  unsigned int i;

  for (i = 0; i < n_outputs; i++)
    {
      MetaOutputAssignment *output_assignment = outputs[i];

      if (output == output_assignment->output)
        return output_assignment;
    }

  return NULL;
}

void meta_monitor_manager_post_init (MetaMonitorManager *manager);

MetaViewportInfo * meta_monitor_manager_get_viewports (MetaMonitorManager *manager);

GList * meta_monitor_manager_get_virtual_monitors (MetaMonitorManager *manager);

void meta_monitor_manager_maybe_emit_privacy_screen_change (MetaMonitorManager *manager);

META_EXPORT_TEST
gboolean meta_monitor_manager_apply_monitors_config (MetaMonitorManager        *manager,
                                                     MetaMonitorsConfig        *config,
                                                     MetaMonitorsConfigMethod   method,
                                                     GError                   **error);
