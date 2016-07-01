#include <pebble.h>
#include "main.h"
#include "accel.h"
#include "background.h"
#include "complications.h"
#include "hands.h"


#define ANIMATION_DURATION 500
#define ANIMATION_DELAY    600


static Window *s_main_window;
static Layer *s_background_layer;
static Layer *s_hands_layer;
static Layer *s_date_layer;
static Layer *s_temp_layer;

GPoint g_center;
Time g_time;
Time s_anim_time;
int g_radius = 0;
bool s_animating = false;

char g_date_buffer[4];
char g_temp_buffer[16];

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

static void tick_handler(struct tm *tick_time, TimeUnits changed) {
  // Store time
  g_time.hours = tick_time->tm_hour;
  g_time.hours -= (g_time.hours > 12) ? 12 : 0;
  g_time.minutes = tick_time->tm_min;
  
  // Redraw
  if(s_hands_layer) {
    layer_mark_dirty(s_hands_layer);
  }
 
  strftime(g_date_buffer, sizeof(g_date_buffer), "%d", tick_time);
  
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


static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);

  g_center = grect_center_point(&window_bounds);

  s_background_layer = background_create(window_layer);
  s_date_layer = date_create(s_background_layer);
  s_temp_layer = temp_create(s_background_layer);
  s_hands_layer = hands_create(s_background_layer);

  
}

static void window_unload(Window *window) {
  layer_destroy(s_hands_layer);
  layer_destroy(s_temp_layer);
  layer_destroy(s_date_layer);
  layer_destroy(s_background_layer);

}

/*********************************** App **************************************/

static int anim_percentage(AnimationProgress dist_normalized, int max) {
  return (int)(float)(((float)dist_normalized / (float)ANIMATION_NORMALIZED_MAX) * (float)max);
}

static void radius_update(Animation *anim, AnimationProgress dist_normalized) {
  g_radius = anim_percentage(dist_normalized, FINAL_RADIUS);

  layer_mark_dirty(s_background_layer);
}

static void hands_update(Animation *anim, AnimationProgress dist_normalized) {
  s_anim_time.hours = anim_percentage(dist_normalized, hours_to_minutes(g_time.hours));
  s_anim_time.minutes = anim_percentage(dist_normalized, g_time.minutes);

  layer_mark_dirty(s_background_layer);
}


static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Read tuples for data
  Tuple *temp_tuple = dict_find(iterator, MESSAGE_KEY_TEMPERATURE);
//   Tuple *conditions_tuple = dict_find(iterator, MESSAGE_KEY_CONDITIONS);

  // If temp data is available, use it
  if(temp_tuple) {
//     snprintf(s_temp_buffer, sizeof(s_temp_buffer), "%d°", (int)temp_tuple->value->int32);
    snprintf(g_temp_buffer, sizeof(g_temp_buffer), "%d", (int)temp_tuple->value->int32);

  }
  
  temp_update();
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

  accel_subscribe();
  
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
  accel_unsubscribe();
  window_destroy(s_main_window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}
