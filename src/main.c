#include "main.h"
#include "accel.h"
#include "animation.h"
#include "background.h"
#include "complications.h"
#include "hands.h"

float s_minute_angle = 0;
float s_hour_angle = 0;
float s_complication_angles[2];
int s_radius = 0;

char s_date_buffer[4];
char s_temp_buffer[16];

GPoint s_center;
Time g_time;
Time s_anim_time;
bool s_animating = false;

static Window *s_main_window;
Layer *s_background_layer;
static Layer *s_hands_layer;
static Layer *s_date_layer;
static Layer *s_temp_layer;

static void tick_handler(struct tm *tick_time, TimeUnits changed) {
  // Store time
  g_time.hours = tick_time->tm_hour;
  g_time.hours -= (g_time.hours > 12) ? 12 : 0;
  g_time.minutes = tick_time->tm_min;
  
  // Redraw
  if(s_hands_layer) {
    layer_mark_dirty(s_hands_layer);
  }
 
  strftime(s_date_buffer, sizeof(s_date_buffer), "%d", tick_time);
  date_update_proc();


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
  temp_update_proc();
  
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);
  s_center = grect_center_point(&window_bounds);

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

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Read tuples for data
  Tuple *temp_tuple = dict_find(iterator, MESSAGE_KEY_TEMPERATURE);
//   Tuple *conditions_tuple = dict_find(iterator, MESSAGE_KEY_CONDITIONS);

  // If temp data is available, use it
  if(temp_tuple) {
//     snprintf(s_temp_buffer, sizeof(s_temp_buffer), "%d°", (int)temp_tuple->value->int32);
    snprintf(s_temp_buffer, sizeof(s_temp_buffer), "%d", (int)temp_tuple->value->int32);

  }
  
  temp_update_proc();
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
  accel_init();

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
