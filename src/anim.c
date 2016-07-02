#include <pebble.h>
#include "main.h"
#include "anim.h"

#define ANIMATION_DURATION 500
#define ANIMATION_DELAY    600

extern int g_radius;
extern Time g_time;
extern Layer *s_background_layer;
  
static Time s_anim_time;

void animate(int duration, int delay, AnimationImplementation *implementation, bool handlers) {
  Animation *anim = animation_create();
  animation_set_duration(anim, duration);
  animation_set_delay(anim, delay);
  animation_set_curve(anim, AnimationCurveEaseInOut);
  animation_set_implementation(anim, implementation);
//   if(handlers) {
//     animation_set_handlers(anim, (AnimationHandlers) {
//       .started = animation_started,
//       .stopped = animation_stopped
//     }, NULL);
//   }
  animation_schedule(anim);
}

int anim_percentage(AnimationProgress dist_normalized, int max) {
  return (int)(float)(((float)dist_normalized / (float)ANIMATION_NORMALIZED_MAX) * (float)max);
}

void radius_update(Animation *anim, AnimationProgress dist_normalized) {
  g_radius = anim_percentage(dist_normalized, FINAL_RADIUS);

  if(s_background_layer) {
    layer_mark_dirty(s_background_layer);
  }
}

void hands_update(Animation *anim, AnimationProgress dist_normalized) {
  g_time.hours = anim_percentage(dist_normalized, s_anim_time.hours);
  g_time.minutes = anim_percentage(dist_normalized, s_anim_time.minutes);

  if(s_background_layer) {
    layer_mark_dirty(s_background_layer);
  }
}

void animation_run() {
  // Prepare animations
  time_t t = time(NULL);
  struct tm *time_now = localtime(&t);
  s_anim_time.hours = time_now->tm_hour;
  s_anim_time.hours -= (g_time.hours > 12) ? 12 : 0;
  s_anim_time.minutes = time_now->tm_min;

  AnimationImplementation radius_impl = {
    .update = radius_update
  };
  animate(ANIMATION_DURATION, ANIMATION_DELAY, &radius_impl, false);

  AnimationImplementation hands_impl = {
    .update = hands_update
  };
  animate(2 * ANIMATION_DURATION, ANIMATION_DELAY, &hands_impl, true);
}
