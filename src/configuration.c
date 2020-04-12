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

void context_set_title(Context *context, char *window_title)
{

	assert(context);
	assert(window_title);

	context->title = window_title;

	regex_t *title_compiled = NULL;
	if (context->title)
	{
		title_compiled = malloc(sizeof(regex_t));
		if (regcomp(title_compiled, window_title, REG_EXTENDED | REG_NOSUB))
		{
			fprintf(stderr, "Error compiling regexp: %s\n", window_title);
			free(title_compiled);
			title_compiled = NULL;
		}
	}
	context->title_compiled = title_compiled;
}

void context_set_class(Context *context, char *window_class)
{

	assert(context);
	assert(window_class);

	context->class = window_class;
	regex_t *class_compiled = NULL;
	if (context->class)
	{
		class_compiled = malloc(sizeof(regex_t));
		if (regcomp(class_compiled, window_class, REG_EXTENDED | REG_NOSUB))
		{
			fprintf(stderr, "Error compiling regexp: %s\n", window_class);
			class_compiled = NULL;
		}
	}
	context->class_compiled = class_compiled;
}

/* alloc a window struct */
Context *configuration_create_context(Configuration *self, char *context_name,
									  char *window_title, char *window_class)
{

	assert(self);
	assert(context_name);
	assert(window_title);
	assert(window_class);

	Context *context = malloc(sizeof(Context));
	bzero(context, sizeof(Context));

	context->name = strdup(context_name);
	context->parent_user_configuration = self;

	context_set_title(context, strdup(window_title));
	context_set_class(context, strdup(window_class));

	context->gesture_list = malloc(sizeof(Gesture *) * 255);
	context->gesture_count = 0;

	self->context_list[self->context_count++] = context;

	return context;
}

void movement_set_expression(Movement *movement, char *movement_expression)
{
	movement->expression = movement_expression;
	char *regex_str = malloc(sizeof(char) * (strlen(movement_expression) + 5));
	strcpy(regex_str, "");
	strcat(regex_str, "^(");
	strcat(regex_str, movement_expression);
	strcat(regex_str, ")$");
	regex_t *movement_compiled = NULL;
	movement_compiled = malloc(sizeof(regex_t));
	if (regcomp(movement_compiled, regex_str, REG_EXTENDED | REG_NOSUB) != 0)
	{
		fprintf(stderr, "Warning: Invalid movement sequence: %s\n", regex_str);
		free(movement_compiled);
		movement_compiled = NULL;
	}
	else
	{
		regcomp(movement_compiled, regex_str, REG_EXTENDED | REG_NOSUB);
	}
	free(regex_str);
	movement->expression_compiled = movement_compiled;
}

/* alloc a movement struct */
Movement *configuration_create_movement(Configuration *self,
										char *movement_name, char *movement_expression)
{

	assert(self);
	assert(movement_name);
	assert(movement_expression);

	Movement *movement = malloc(sizeof(Movement));
	bzero(movement, sizeof(Movement));

	movement->name = strdup(movement_name);
	movement_set_expression(movement, strdup(movement_expression));

	self->movement_list[self->movement_count] = movement;
	self->movement_count++;

	return movement;
}

Gesture *configuration_create_gesture(Context *self, char *gesture_name,
									  char *gesture_movement)
{

	assert(self);
	assert(gesture_name);
	assert(gesture_movement);

	Gesture *ans = malloc(sizeof(Gesture));
	bzero(ans, sizeof(Gesture));

	ans->name = strdup(gesture_name);
	ans->movement = configuration_find_movement_by_name(
		self->parent_user_configuration, gesture_movement);

	if (!ans->movement)
	{
		printf(
			"Movement '%s' referenced by gesture '%s' is unknown. The gesture will be inaccessible.\n",
			gesture_movement, gesture_name);
	}

	ans->context = self;
	ans->action_count = 0;
	ans->action_list = malloc(sizeof(Action) * 20);

	self->gesture_list[self->gesture_count++] = ans;

	return ans;
}

/* alloc an action struct */
Action *configuration_create_action(Gesture *self, int action_type,
									char *action_data)
{

	assert(self);
	assert(action_type);
	assert(action_data);

	Action *ans = malloc(sizeof(Action));
	bzero(ans, sizeof(Action));

	ans->type = action_type;
	ans->original_str = action_data;

	self->action_list[self->action_count++] = ans;

	return ans;
}

Gesture *match_gesture(Configuration *self, char *captured_sequence,
					   ActiveWindowInfo *window)
{

	assert(self);
	assert(captured_sequence);
	assert(window);

	Gesture *matched_gesture = NULL;

	int c = 0;

	for (c = 0; c < self->context_count; ++c)
	{

		Context *context = self->context_list[c];

		assert(context->class);
		assert(context->title);

		if (regexec(context->class_compiled, window->class, 0,
					(regmatch_t *)NULL, 0) != 0)
		{
			continue;
		}

		if (regexec(context->title_compiled, window->title, 0,
					(regmatch_t *)NULL, 0) != 0)
		{
			continue;
		}

		assert(context->gesture_count);

		int g = 0;

		for (g = 0; g < context->gesture_count; ++g)
		{

			Gesture *gest = context->gesture_list[g];

			assert(gest);
			assert(gest->movement);
			assert(gest->movement->expression_compiled);

			if (regexec(gest->movement->expression_compiled, captured_sequence,
						0, (regmatch_t *)NULL, 0) == 0)
			{

				matched_gesture = gest;
				break;
			}
		}
	}

	return matched_gesture;
}

Gesture *configuration_process_gesture(Configuration *self, Capture *grab)
{

	assert(self);
	assert(grab);

	Gesture *gest = NULL;

	int i = 0;

	for (i = 0; i < grab->expression_count; ++i)
	{

		char *sequence = grab->expression_list[i];
		gest = match_gesture(self, sequence, grab->active_window_info);

		if (gest)
		{
			return gest;
		}
	}

	return NULL;
}

Movement *configuration_find_movement_by_name(Configuration *self,
											  char *movement_name)
{

	assert(self);
	assert(movement_name);

	if (!movement_name)
	{
		return NULL;
	}

	int i = 0;

	for (i = 0; i < self->movement_count; ++i)
	{
		Movement *m = self->movement_list[i];

		if ((m->name) && (movement_name) && (strcasecmp(movement_name, m->name) == 0))
		{
			return m;
		}
	}

	return NULL;
}

int configuration_get_gestures_count(Configuration *self)
{

	assert(self);

	int count = 0;

	for (int c = 0; c < self->context_count; ++c)
	{
		count += self->context_list[c]->gesture_count;
	}

	return count;
}

Configuration *configuration_new()
{

	Configuration *self = malloc(sizeof(Configuration));
	bzero(self, sizeof(Configuration));

	self->movement_count = 0;
	self->movement_list = malloc(sizeof(Movement *) * 254);

	self->context_count = 0;
	self->context_list = malloc(sizeof(Context *) * 254);

	return self;
}
