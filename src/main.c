#include <pebble.h>
#include "background.h"

#define TRIG_360 TRIG_MAX_ANGLE
#define TRIG_180 (TRIG_MAX_ANGLE / 2)
#define TRIG_120 (TRIG_MAX_ANGLE / 3)
#define TRIG_90 (TRIG_MAX_ANGLE / 4)

#define COLORS       PBL_IF_COLOR_ELSE(true, false)
#define ANTIALIASING true

#define HAND_MARGIN  20
#define FINAL_RADIUS 70

#define ANIMATION_DURATION 500
#define ANIMATION_DELAY    600

#define MINUTE_HAND_COLOR GColorWhite
#define HOUR_HAND_COLOR GColorFolly

#define COMPLICATION_MARGIN 20
#define COMPLICATION_CENTER_RADIUS 48
#define COMPLICATION_FONT FONT_KEY_BITHAM_34_MEDIUM_NUMBERS  
#define COMPLICATION_ORIGIN_OFFSET_X 36
#define COMPLICATION_ORIGIN_OFFSET_Y 18
#define COMPLICATION_W 74
#define COMPLICATION_H 34

typedef struct {
  int hours;
  int minutes;
} Time;

static Window *s_main_window;
static Layer *s_background_layer;
static Layer *s_hands_layer;

static GPoint s_center;
static Time s_last_time, s_anim_time;
static int s_radius = 0;
static bool s_animating = false;

static float s_minute_angle = 0;
static float s_hour_angle = 0;
static float s_complication_angles[2];

static TextLayer *s_date_layer;
static char s_date_buffer[4];
static GRect s_date_frame;

static TextLayer *s_temp_layer;
static char s_temp_buffer[16];
static GRect s_temp_frame;

/*************************** AnimationImplementation **************************/

static void animation_started(Animation *anim, void *context) {
  s_animating = true;
}

static void animation_stopped(Animation *anim, bool stopped, void *context) {
  s_animating = false;
}

static void animate(int duration, int delay, AnimationImplementation *implementation, bool handlers) {
  Animation *anim = animation_create();
  animation_set_duration(anim, duration);
  animation_set_delay(anim, delay);
  animation_set_curve(anim, AnimationCurveEaseInOut);
  animation_set_implementation(anim, implementation);
  if(handlers) {
    animation_set_handlers(anim, (AnimationHandlers) {
      .started = animation_started,
      .stopped = animation_stopped
    }, NULL);
  }
  animation_schedule(anim);
}

/************************************ UI **************************************/
static void date_update_proc() {
  float date_angle = s_complication_angles[0];

  GRect date_frame = (GRect) {
    .origin = (GPoint) {
      .x = (int16_t)(sin_lookup(date_angle) * (int32_t)COMPLICATION_CENTER_RADIUS / TRIG_MAX_RATIO) + s_center.x - COMPLICATION_ORIGIN_OFFSET_X,
      .y = (int16_t)(-cos_lookup(date_angle) * (int32_t)COMPLICATION_CENTER_RADIUS / TRIG_MAX_RATIO) + s_center.y - COMPLICATION_ORIGIN_OFFSET_Y
    },
    .size = (GSize) {
      .w = COMPLICATION_W,
      .h = COMPLICATION_H
    },
  };
  layer_set_frame(text_layer_get_layer(s_date_layer), date_frame);
  text_layer_set_text(s_date_layer, s_date_buffer);

}


static void temp_update_proc() {
  float temp_angle = s_complication_angles[1];

  GRect temp_frame = (GRect) {
    .origin = (GPoint) {
      .x = (int16_t)(sin_lookup(temp_angle) * (int32_t)COMPLICATION_CENTER_RADIUS / TRIG_MAX_RATIO) + s_center.x - COMPLICATION_ORIGIN_OFFSET_X,
      .y = (int16_t)(-cos_lookup(temp_angle) * (int32_t)COMPLICATION_CENTER_RADIUS / TRIG_MAX_RATIO) + s_center.y - COMPLICATION_ORIGIN_OFFSET_Y
    },
    .size = (GSize) {
      .w = COMPLICATION_W,
      .h = COMPLICATION_H
    },
  };
  layer_set_frame(text_layer_get_layer(s_temp_layer), temp_frame);
  text_layer_set_text(s_temp_layer, s_temp_buffer);

}


static void tick_handler(struct tm *tick_time, TimeUnits changed) {
  // Store time
  s_last_time.hours = tick_time->tm_hour;
  s_last_time.hours -= (s_last_time.hours > 12) ? 12 : 0;
  s_last_time.minutes = tick_time->tm_min;
  
  // Redraw
  if(s_hands_layer) {
    layer_mark_dirty(s_hands_layer);
  }
 
  strftime(s_date_buffer, sizeof(s_date_buffer), "%d", tick_time);
  
  // Get weather update every 30 minutes
  if(tick_time->tm_min % 30 == 0) {
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
  
    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);
  
    // Send the message!
    app_message_outbox_send();
  }
  
}

static int hours_to_minutes(int hours_out_of_12) {
  return (int)(float)(((float)hours_out_of_12 / 12.0F) * 60.0F);
}

static void hands_update_proc(Layer *layer, GContext *ctx) {
  graphics_context_set_antialiased(ctx, ANTIALIASING);

  // Calculate angles
  if (s_animating) {
    s_minute_angle = TRIG_MAX_ANGLE * s_anim_time.minutes / 60;
    // Hours out of 60 for smoothness
    s_hour_angle = TRIG_MAX_ANGLE * s_anim_time.hours / 60;
  } else {
    s_minute_angle = TRIG_MAX_ANGLE * s_last_time.minutes / 60;
    s_hour_angle = TRIG_MAX_ANGLE * s_last_time.hours / 12;
  }

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
  graphics_context_set_stroke_width(ctx, 8);
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


  date_update_proc();
  temp_update_proc();
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);

  s_center = grect_center_point(&window_bounds);

  s_background_layer = background_create(window_layer);
  
  s_date_frame = GRect(s_center.x + COMPLICATION_MARGIN, s_center.y + COMPLICATION_MARGIN, s_center.x - COMPLICATION_MARGIN, s_center.y - COMPLICATION_MARGIN);
  s_date_layer = text_layer_create(s_date_frame);
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_font(s_date_layer, fonts_get_system_font(COMPLICATION_FONT));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  layer_add_child(s_background_layer, text_layer_get_layer(s_date_layer));  

  s_temp_frame = GRect(s_center.x + COMPLICATION_MARGIN, s_center.y + COMPLICATION_MARGIN, s_center.x - COMPLICATION_MARGIN, s_center.y - COMPLICATION_MARGIN);
  s_temp_layer = text_layer_create(s_temp_frame);
  text_layer_set_background_color(s_temp_layer, GColorClear);
  text_layer_set_text_color(s_temp_layer, GColorWhite);
  text_layer_set_font(s_temp_layer, fonts_get_system_font(COMPLICATION_FONT));
  text_layer_set_text_alignment(s_temp_layer, GTextAlignmentCenter);
  layer_add_child(s_background_layer, text_layer_get_layer(s_temp_layer));  

  s_hands_layer = layer_create(window_bounds);
  layer_set_update_proc(s_hands_layer, hands_update_proc);
  layer_add_child(s_background_layer, s_hands_layer);
  
}

static void window_unload(Window *window) {
  layer_destroy(s_hands_layer);
  layer_destroy(text_layer_get_layer(s_temp_layer));
  layer_destroy(text_layer_get_layer(s_date_layer));
  layer_destroy(s_background_layer);


}

/*********************************** App **************************************/

static int anim_percentage(AnimationProgress dist_normalized, int max) {
  return (int)(float)(((float)dist_normalized / (float)ANIMATION_NORMALIZED_MAX) * (float)max);
}

static void radius_update(Animation *anim, AnimationProgress dist_normalized) {
  s_radius = anim_percentage(dist_normalized, FINAL_RADIUS);

  layer_mark_dirty(s_background_layer);
}

static void hands_update(Animation *anim, AnimationProgress dist_normalized) {
  s_anim_time.hours = anim_percentage(dist_normalized, hours_to_minutes(s_last_time.hours));
  s_anim_time.minutes = anim_percentage(dist_normalized, s_last_time.minutes);

  layer_mark_dirty(s_background_layer);
}


static void accel_data_handler(AccelData *data, uint32_t num_samples) {
  // Activate backlight if tilted toward the wearer
  if (data[1].x < -400 || data[1].x > 400) return;
  if (data[1].y > 0) return;
  if (data[1].did_vibrate) return;
  if (((data[1].y * 1.2) - data[0].y) > -200) return;
  
  light_enable_interaction();

}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Read tuples for data
  Tuple *temp_tuple = dict_find(iterator, MESSAGE_KEY_TEMPERATURE);
  Tuple *conditions_tuple = dict_find(iterator, MESSAGE_KEY_CONDITIONS);

  // If temp data is available, use it
  if(temp_tuple) {
//     snprintf(s_temp_buffer, sizeof(s_temp_buffer), "%d°", (int)temp_tuple->value->int32);
    snprintf(s_temp_buffer, sizeof(s_temp_buffer), "%d", (int)temp_tuple->value->int32);

  }
  
  text_layer_set_text(s_temp_layer, s_temp_buffer);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void init() {
  srand(time(NULL));

  time_t t = time(NULL);
  struct tm *time_now = localtime(&t);
  tick_handler(time_now, MINUTE_UNIT);

  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_main_window, true);

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  // Prepare animations
  AnimationImplementation radius_impl = {
    .update = radius_update
  };
  animate(ANIMATION_DURATION, ANIMATION_DELAY, &radius_impl, false);

  AnimationImplementation hands_impl = {
    .update = hands_update
  };
  animate(2 * ANIMATION_DURATION, ANIMATION_DELAY, &hands_impl, true);

  // Subscribe to batched accelerometer data events
  accel_data_service_subscribe(2, accel_data_handler);

  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  const int inbox_size = 128;
  const int outbox_size = 128;
  app_message_open(inbox_size, outbox_size);
  
}

static void deinit() {
  window_destroy(s_main_window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}
