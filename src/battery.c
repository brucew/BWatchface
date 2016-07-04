#include <pebble.h>
#include "battery.h"

#define BATTERY_MARGIN 10
#define BATTERY_SIZE 11
#define BATTERY_RADIUS 5

extern GPoint g_center;

static GColor s_battery_color;
static Layer *s_battery_layer;
static GPoint s_battery_center;

static void battery_state_handler(BatteryChargeState charge) {
  if (charge.charge_percent >= 90) {
    s_battery_color = GColorBrightGreen;
  } else if (charge.charge_percent >= 30) {
    s_battery_color = GColorGreen;
  } else if (charge.charge_percent > 10) {
    s_battery_color = GColorYellow;
  } else {
    s_battery_color = GColorFolly;
  }
  
  if (s_battery_layer) {
    layer_mark_dirty(s_battery_layer);
  }

}

static void battery_update_proc(Layer *layer, GContext *ctx) {
  graphics_context_set_antialiased(ctx, true);
  graphics_context_set_fill_color(ctx, s_battery_color);
  graphics_fill_circle(ctx, s_battery_center, BATTERY_RADIUS);
}

void battery_create(Layer *parent_layer) {
  battery_state_service_subscribe(battery_state_handler);
  s_battery_center = GPoint(BATTERY_RADIUS, BATTERY_RADIUS);
  s_battery_layer = layer_create((GRect) {
    .origin = (GPoint) {
      .x = g_center.x - BATTERY_RADIUS,
      .y = BATTERY_RADIUS + BATTERY_MARGIN
    },
    .size = (GSize) {
      .w = BATTERY_SIZE,
      .h = BATTERY_SIZE
    },
  });
  layer_set_update_proc(s_battery_layer, battery_update_proc);
  layer_add_child(parent_layer, s_battery_layer);
  
  battery_state_handler(battery_state_service_peek());
  
}

void battery_destroy() {
  battery_state_service_unsubscribe();
  layer_destroy(s_battery_layer);
}
