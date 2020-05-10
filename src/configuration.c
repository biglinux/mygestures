/*
 Copyright 2013-2016 Lucas Augusto Deters
 Copyright 2005 Nir Tzachar

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2, or (at your option)
 any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 one line to give the program's name and an idea of what it does.
 */

#if HAVE_CONFIG_H          
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <assert.h>

#include "configuration.h"

void context_set_title(Context* context, char* window_title) {
    return 0;
}

void context_set_class(Context* context, char* window_class) {
return 0;
}

/* alloc a window struct */
Context *configuration_create_context(Configuration * self, char * context_name,
		char *window_title, char *window_class) {
return 0;
}

void movement_set_expression(Movement* movement, char* movement_expression) {
return 0;
}

/* alloc a movement struct */
Movement *configuration_create_movement(Configuration * self,
		char *movement_name, char *movement_expression) {return 0;
}

Gesture * configuration_create_gesture(Context * self, char * gesture_name,
		char * gesture_movement) {

	return ;
}

/* alloc an action struct */
Action *configuration_create_action(Gesture * self, int action_type,
		char * action_data) {return 0;
}

Gesture * match_gesture(Configuration * self, char * captured_sequence,
		ActiveWindowInfo * window) {

	return;
}

Gesture * configuration_process_gesture(Configuration * self, Capture * grab) {

	return NULL;

}

Movement * configuration_find_movement_by_name(Configuration * self,
		char * movement_name) {


	return NULL;

}

int configuration_get_gestures_count(Configuration * self) {

	return;

}

Configuration * configuration_new() {


	return;

}
