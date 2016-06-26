#include <pebble.h>

#define COLORS       PBL_IF_COLOR_ELSE(true, false)
#define ANTIALIASING true

#define HAND_MARGIN  20
#define FINAL_RADIUS 70

#define ANIMATION_DURATION 500
#define ANIMATION_DELAY    600

#define BACKGROUND_COLOR GColorBlue
#define MINUTE_HAND_COLOR GColorWhite
#define HOUR_HAND_COLOR GColorFolly

#define COMPLICATION_MARGIN 20

typedef struct {
  int hours;
  int minutes;
} Time;

static Window *s_main_window;
static Layer *s_background_layer;
static Layer *s_hands_layer;
static TextLayer *s_date_layer;



static GPoint s_center;
static Time s_last_time, s_anim_time;
static int s_radius = 0;
static bool s_animating = false;
static float s_minute_angle = 0;
static float s_hour_angle = 0;

static GRect s_TL_frame;
static GRect s_TR_frame;
static GRect s_BL_frame;
static GRect s_BR_frame;

static bool s_TL_blocked = false;
static bool s_TR_blocked = false;
static bool s_BL_blocked = false;
static bool s_BR_blocked = false;


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
  s_last_time.hours = tick_time->tm_hour;
  s_last_time.hours -= (s_last_time.hours > 12) ? 12 : 0;
  s_last_time.minutes = tick_time->tm_min;

  s_TL_blocked = ((s_last_time.minutes > 45) || (s_last_time.hours > 9 && s_last_time.hours < 12));
  s_TR_blocked = ((s_last_time.minutes > 0 && s_last_time.minutes < 15) || (s_last_time.hours < 3));
  s_BL_blocked = ((s_last_time.minutes > 30 && s_last_time.minutes < 45) || (s_last_time.hours > 6 && s_last_time.hours < 9));
  s_BR_blocked = ((s_last_time.minutes > 15 && s_last_time.minutes < 30) || (s_last_time.hours > 3 && s_last_time.hours < 6));

  if(s_date_layer) {
    if(s_BR_blocked) {
      if(s_TR_blocked) {
        layer_set_frame(text_layer_get_layer(s_date_layer), s_BL_frame);
      } else {
        layer_set_frame(text_layer_get_layer(s_date_layer), s_TR_frame);
      }
    } else {
      layer_set_frame(text_layer_get_layer(s_date_layer), s_BR_frame);
    }
    
    text_layer_set_text(s_date_layer, "26");

  }
  
  // Redraw
  if(s_hands_layer) {
    layer_mark_dirty(s_hands_layer);
  }
 
}

static int hours_to_minutes(int hours_out_of_12) {
  return (int)(float)(((float)hours_out_of_12 / 12.0F) * 60.0F);
}

static void date_update_proc(Layer *layer, GContext *ctx) {
    text_layer_set_text(s_date_layer, "26");
}  

static void hands_update_proc(Layer *layer, GContext *ctx) {
  graphics_context_set_antialiased(ctx, ANTIALIASING);

  // Calculate angles
  if(s_animating) {
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
  if(s_radius > HAND_MARGIN) {
    graphics_context_set_stroke_color(ctx, MINUTE_HAND_COLOR);
    graphics_draw_line(ctx, s_center, minute_hand);
  }
  if(s_radius > HAND_MARGIN) {
    graphics_context_set_stroke_color(ctx, HOUR_HAND_COLOR);
    graphics_draw_line(ctx, s_center, hour_hand);
  }
}

static void background_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_fill_color(ctx, BACKGROUND_COLOR);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);

  s_center = grect_center_point(&window_bounds);
  s_TL_frame = GRect(COMPLICATION_MARGIN, COMPLICATION_MARGIN, s_center.x - COMPLICATION_MARGIN, s_center.y - COMPLICATION_MARGIN);
  s_TR_frame = GRect(s_center.x + COMPLICATION_MARGIN, COMPLICATION_MARGIN, s_center.x - COMPLICATION_MARGIN, s_center.y - COMPLICATION_MARGIN);
  s_BL_frame = GRect(COMPLICATION_MARGIN, s_center.y + COMPLICATION_MARGIN, s_center.x - COMPLICATION_MARGIN, s_center.y - COMPLICATION_MARGIN);
  s_BR_frame = GRect(s_center.x + COMPLICATION_MARGIN, s_center.y + COMPLICATION_MARGIN, s_center.x - COMPLICATION_MARGIN, s_center.y - COMPLICATION_MARGIN);

  s_background_layer = layer_create(window_bounds);
  layer_set_update_proc(s_background_layer, background_update_proc);
  layer_add_child(window_layer, s_background_layer);

  s_date_layer = text_layer_create(s_BR_frame);
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
//   layer_set_update_proc(s_date_layer, date_update_proc);
  layer_add_child(s_background_layer, text_layer_get_layer(s_date_layer));  

  s_hands_layer = layer_create(window_bounds);
  layer_set_update_proc(s_hands_layer, hands_update_proc);
  layer_add_child(s_background_layer, s_hands_layer);
  
}

static void window_unload(Window *window) {
  layer_destroy(s_hands_layer);
  layer_destroy(text_layer_get_layer(s_date_layer));

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
  if(!data[0].did_vibrate && data[0].y < -600) {
    light_enable_interaction();
  }
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

}

static void deinit() {
  window_destroy(s_main_window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}
