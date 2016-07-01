#include "main.h"
#include "hands.h"

Layer* hands_create(Layer *parent_layer) {
  Layer *hands_layer = layer_create(layer_get_bounds(parent_layer));
  layer_set_update_proc(hands_layer, hands_update_proc);
  layer_add_child(parent_layer, hands_layer);
  
  return hands_layer;
}

void hands_update_proc(Layer *layer, GContext *ctx) {
  graphics_context_set_antialiased(ctx, ANTIALIASING);

  // Calculate angles
//   if (is_animating()) {
//     s_minute_angle = TRIG_MAX_ANGLE * s_anim_time.minutes / 60;
//     // Hours out of 60 for smoothness
//     s_hour_angle = TRIG_MAX_ANGLE * s_anim_time.hours / 60;
//   } else {
    s_minute_angle = TRIG_MAX_ANGLE * g_time.minutes / 60;
    s_hour_angle = TRIG_MAX_ANGLE * g_time.hours / 12;
//   }
  s_hour_angle += s_minute_angle >= TRIG_180 ? TRIG_180 / 12 : 0;

  // Plot hands
  GPoint minute_hand = (GPoint) {
    .x = (int16_t)(sin_lookup(s_minute_angle) * (int32_t)s_radius / TRIG_MAX_RATIO) + s_center.x,
    .y = (int16_t)(-cos_lookup(s_minute_angle) * (int32_t)s_radius / TRIG_MAX_RATIO) + s_center.y,
  };
  GPoint hour_hand = (GPoint) {
    .x = (int16_t)(sin_lookup(s_hour_angle) * (int32_t)(s_radius - HAND_MARGIN) / TRIG_MAX_RATIO) + s_center.x,
    .y = (int16_t)(-cos_lookup(s_hour_angle) * (int32_t)(s_radius - HAND_MARGIN) / TRIG_MAX_RATIO) + s_center.y,
  };

  // Draw hands with positive length only
  graphics_context_set_stroke_width(ctx, 9);
  if (s_radius > HAND_MARGIN) {
    graphics_context_set_stroke_color(ctx, MINUTE_HAND_COLOR);
    graphics_draw_line(ctx, s_center, minute_hand);
  }
  if (s_radius > HAND_MARGIN) {
    graphics_context_set_stroke_color(ctx, HOUR_HAND_COLOR);
    graphics_draw_line(ctx, s_center, hour_hand);
  }
  
  // Calculate complication angles
  float delta_angle = abs(s_minute_angle - s_hour_angle);
  float bisect_angle = (s_minute_angle + s_hour_angle) / 2;
  if (delta_angle > TRIG_120 && delta_angle < (TRIG_120 * 2)) {    
    s_complication_angles[0] = bisect_angle;
    s_complication_angles[1] = bisect_angle + TRIG_180;
    s_complication_angles[1] -= s_complication_angles[1] > TRIG_360 ? TRIG_360 : 0;
  } else {
    if (delta_angle < TRIG_180) {
      delta_angle = TRIG_360 - delta_angle;
      bisect_angle += TRIG_180;
      bisect_angle -= bisect_angle > TRIG_360 ? TRIG_360 : 0;
    }
    delta_angle = delta_angle / 6;
    s_complication_angles[0] = bisect_angle - delta_angle;
    s_complication_angles[1] = bisect_angle + delta_angle;
  }
  
//   // Plot complication pointers
//   graphics_context_set_stroke_width(ctx, 2);
//   graphics_context_set_stroke_color(ctx, GColorGreen);
//   GPoint complication_center = (GPoint) {
//     .x = (int16_t)(sin_lookup(s_complication_angles[0]) * (int32_t)COMPLICATION_CENTER_RADIUS / TRIG_MAX_RATIO) + s_center.x,
//     .y = (int16_t)(-cos_lookup(s_complication_angles[0]) * (int32_t)COMPLICATION_CENTER_RADIUS / TRIG_MAX_RATIO) + s_center.y,
//   };
//   graphics_draw_line(ctx, s_center, complication_center);
//   complication_center = (GPoint) {
//     .x = (int16_t)(sin_lookup(s_complication_angles[1]) * (int32_t)COMPLICATION_CENTER_RADIUS / TRIG_MAX_RATIO) + s_center.x,
//     .y = (int16_t)(-cos_lookup(s_complication_angles[1]) * (int32_t)COMPLICATION_CENTER_RADIUS / TRIG_MAX_RATIO) + s_center.y,
//   };
//   graphics_draw_line(ctx, s_center, complication_center);


}

