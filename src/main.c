#include <pebble.h>
#include "main.h"
#include "accel.h"
#include "anim.h"
#include "background.h"
#include "complications.h"
#include "hands.h"
#include "messages.h"

static Window *s_main_window;
Layer *s_background_layer;
static Layer *s_hands_layer;
static Layer *s_date_layer;
static Layer *s_temp_layer;

GPoint g_center;
Time g_time;
int g_radius = 10;

char g_date_buffer[4];
char g_temp_buffer[8];


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
    weather_update();
  }
  
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

static void init() {
  srand(time(NULL));

  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_main_window, true);

  animation_run();

  time_t t = time(NULL);
  struct tm *time_now = localtime(&t);
  tick_handler(time_now, MINUTE_UNIT);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  accel_subscribe();
  messages_register();
  
  // Open AppMessage
  const int inbox_size = 128;
  const int outbox_size = 128;
  app_message_open(inbox_size, outbox_size);
  
}

static void deinit() {
  accel_unsubscribe();
  tick_timer_service_unsubscribe();
  window_destroy(s_main_window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}
