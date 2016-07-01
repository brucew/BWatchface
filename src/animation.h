#pragma once

#define ANIMATION_DURATION 500
#define ANIMATION_DELAY    600


extern bool s_animating;
extern Time g_time;
extern Time s_anim_time;
extern Layer *s_background_layer;
extern int s_radius;

void animate(int duration, int delay, AnimationImplementation *implementation, bool handlers);
void animation_started(Animation *anim, void *context);
void animation_stopped(Animation *anim, bool stopped, void *context);
void hands_update(Animation *anim, AnimationProgress dist_normalized);
static int hours_to_minutes(int hours_out_of_12);
void radius_update(Animation *anim, AnimationProgress dist_normalized);
