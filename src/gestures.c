/*

 Copyright 2005 Nir Tzachar
 Copyright 2013 Lucas Augusto Deters


 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2, or (at your option)
 any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 */

#if HAVE_CONFIG_H          
#include <config.h>
#endif

#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include "gestures.h"
#include "helpers.h"
#include "wm.h"
#include <regex.h>
#include <X11/Xutil.h>

/* Actions */
enum {
	ACTION_EXIT_GEST = 1,
	ACTION_EXECUTE,
	ACTION_ICONIFY,
	ACTION_KILL,
	ACTION_DISPLAY_GESTS,
	ACTION_RECONF,
	ACTION_RAISE,
	ACTION_LOWER,
	ACTION_MAXIMIZE,
	ACTION_ROOT_SEND,
	ACTION_LAST
};

/* The name from the actions, to read in .gestures file. */
char *gestures_names[] = { "NONE", "exit", "exec", "minimize", "kill",
		"display_gests", "reconf", "raise", "lower", "maximize", "root_send" };

/* this holds all known gestures */
struct gesture **specific_gestures = NULL;
int specific_gestures_count = 0;
/* a temporary stack - it remains necessary?) */
EMPTY_STACK(temp_stack);

/* a temporary stack - it remains necessary?) */
EMPTY_STACK(movement_stack);

/* this holds all known gestures */
struct gesture **global_gestures = NULL;
int global_gestures_count = 0;
/* a temporary stack - it remains necessary?) */
EMPTY_STACK(temp_general_stack);

/* conf file: */
extern char conf_file[];

/* shutdown? */
extern int shut_down;

/* alloc a window struct */
struct window *alloc_window(char *window_title, char *window_class,
		regex_t window_title_compiled, regex_t window_class_compiled) {
	struct window *ans = malloc(sizeof(struct window));
	bzero(ans, sizeof(struct window));

	ans->title = window_title;
	ans->class = window_class;
	ans->title_compiled = window_title_compiled;
	ans->class_compiled = window_class_compiled;
	return ans;
}

/* release a window struct */
void free_window(struct window *free_me) {
	free(free_me->title);
	free(free_me->class);
	free(free_me);
	return;
}

/* alloc a movement struct */
struct movement *alloc_movement(char *movement_name, char *movement_expression,
		regex_t movement_compiled) {
	struct movement *ans = malloc(sizeof(struct movement));
	bzero(ans, sizeof(struct movement));

	ans->name = movement_name;
	ans->expression = movement_expression;
	ans->compiled = movement_compiled;
	return ans;
}

/* release a movement struct */
void free_movement(struct movement *free_me) {
	free(free_me->name);
	free(free_me->expression);
	//free(free_me->movement_compiled);
	free(free_me);
	return;
}

/* alloc a gesture struct */
struct gesture *alloc_gesture(struct window *gesture_window,
		struct action *gesture_action, struct movement *gesture_movement) {
	struct gesture *ans = malloc(sizeof(struct gesture));
	bzero(ans, sizeof(struct gesture));

	ans->window = gesture_window;
	ans->action = gesture_action;
	ans->movement = gesture_movement;
	return ans;
}

/* release a gesture struct */
void free_gesture(struct gesture *free_me) {
	free(free_me->action);
	free(free_me);

	return;
}

/* alloc an action struct */
struct action *alloc_action(int action_type, void *action_data) {
	struct action *ans = malloc(sizeof(struct action));
	bzero(ans, sizeof(struct action));
	ans->type = action_type;
	ans->data = action_data;

	return ans;
}

/* release an action struct */
void free_action(struct action *free_me) {
	free(free_me);
	return;
}

/* alloc a key_press struct ???? */
struct key_press *alloc_key_press(void) {
	struct key_press *ans = malloc(sizeof(struct key_press));
	bzero(ans, sizeof(struct key_press));
	return ans;
}

/* release a key_press struct */
void free_key_press(struct key_press *free_me) {
	free(free_me);
	return;
}

/*
 * Match the window, the class and the grabbed gesture with a known gesture
 */
int match_gesture(char *expression, const struct window_info *activeWindow,
		struct gesture *a_known_gesture) {
	// the gesture reg exp
	char *expressao = a_known_gesture->movement->expression;

	// the received gesture
	char *str = expression;

	// compile the regular expression for the movement
	regex_t reg = a_known_gesture->movement->compiled;

	// try to match the command
	int status = regexec(&reg, str, 0, (regmatch_t *) NULL, 0);

	if (status == 0) {

		// try to match also the title
		if (a_known_gesture->window->title != NULL) {
			reg = a_known_gesture->window->title_compiled;
			status = regexec(&reg, activeWindow->title, 0, (regmatch_t *) NULL,
					0);
		}

		// try to match also the class
		if (a_known_gesture->window->class != NULL) {
			reg = a_known_gesture->window->class_compiled;
			status = regexec(&reg, activeWindow->class, 0, (regmatch_t *) NULL,
					0);
		}

	}

	return status;

}

/**
 * Show the configured gestures.
 TODO: REVIEW THIS METHOD...
 */
void display_all_gestures() {
	/*
	 int i;
	 int id = fork();
	 if (id == 0){
	 char *msg;
	 int index = 0;
	 char *command;
	 int msg_len = (GEST_SEQUENCE_MAX+2)*sizeof(char)*known_gestures_num;
	 msg = malloc(msg_len);
	 bzero(msg, msg_len);
	 command = malloc(msg_len + sizeof(char)*256);

	 for (i=0; i< known_gestures_num; i++){
	 struct gesture *gesture = known_gestures[i];
	 index = strlen(msg);
	 if (gesture->action->type == ACTION_EXECUTE)
	 sprintf(&msg[index], "%s %s %s\n",
	 gesture->movement->expression, gestures_names[gesture->action->type],
	 gesture->action->data);
	 else if (gesture->action->type == ACTION_ROOT_SEND){
	 struct key_press *key = gesture->action->data;
	 if (key == NULL)
	 continue;
	 sprintf(&msg[index], "%s %s %s\n",
	 gesture->movement->expression, gestures_names[gesture->action->type],
	 key->original_str);
	 } else sprintf(&msg[index], "%s %s\n",
	 gesture->movement->expression, gestures_names[gesture->action->type]);

	 }
	 sprintf(command, "echo -e \"%s\" | xmessage -center -file -", msg);
	 system(command);
	 exit(0);
	 }
	 */
	printf("Uninplemented...");
	exit(0);

}

/**
 * Fake key event
 */
void press_key(Display *dpy, KeySym key, Bool is_press) {

	XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, key), is_press, CurrentTime);
	return;
}

/**
 * Fake sequence key events
 */
void root_send(XButtonEvent *ev, struct action *action) {
	struct key_press *first_key;
	struct key_press *tmp;

	if (action == NULL) {
		fprintf(stderr, " internal error in %s\n", __func__);
		return;

	}
	first_key = (struct key_press *) action->data;

	if (first_key == NULL) {
		fprintf(stderr, " internal error in %s, key is null\n", __func__);
		return;
	}

	for (tmp = first_key; tmp != NULL; tmp = tmp->next)
		press_key(ev->display, tmp->key, True);

	for (tmp = first_key; tmp != NULL; tmp = tmp->next)
		press_key(ev->display, tmp->key, False);

	return;
}

/**
 * Execute an action
 */
void execute_action(XButtonEvent *ev, struct action *action) {
	int id;

	// if there are a action
	if (action == NULL) {
		// do nothing

	} // if are
	else {

		switch (action->type) {
		case ACTION_EXIT_GEST:
			shut_down = True;
			break;
		case ACTION_EXECUTE:
			id = fork();
			if (id == 0) {
				int i = system(action->data);
				exit(i);
			}
			break;
		case ACTION_ICONIFY:
			wm_helper->iconify(ev);
			break;
		case ACTION_KILL:
			wm_helper->kill(ev);
			break;
		case ACTION_DISPLAY_GESTS:
			display_all_gestures();
			break;
		case ACTION_RECONF:
			init_gestures(conf_file);
			break;
		case ACTION_RAISE:
			wm_helper->raise(ev);
			break;
		case ACTION_LOWER:
			wm_helper->lower(ev);
			break;
		case ACTION_MAXIMIZE:
			wm_helper->maximize(ev);
			break;
		case ACTION_ROOT_SEND:
			root_send(ev, action);
			break;
		default:
			fprintf(stderr, "found an unknown gesture \n");
		}
	}
	return;
}

/**
 * Receives captured movement from two diferent algoritms: a simple and a complex.
 * 
 *   The simple algoritm can understand movements in four directions: U, D, R, L
 *   The complex algoritm can understand movements in eight directions, including the diagonals
 * 
 * The program will process the complex movement sequence prior to the simplest sequence.
 * 
 * The system will also prioritize the gestures defined for the current application, 
 * and only then will consider the global gestures.
 */
void process_movement_sequences(XButtonEvent *e,
		const struct window_info *activeWindow, char *complex_sequence,
		char *simple_sequence) {

	struct gesture *gest = NULL;
	int i;


	printf("\n\n");
	printf("Window class: %s\n",activeWindow->class);
	printf("Window title: %s\n",activeWindow->title);
	printf("Searching gesture for sequence: '%s'\n",complex_sequence);

	// searching the complex sequence on the application specific gestures
	for (i = 0; i < specific_gestures_count; i++) {
		if (!match_gesture(complex_sequence, activeWindow,
				specific_gestures[i])) {

			gest = specific_gestures[i];

			printf("Gesture found");
			if (strcmp("",gest->window->class)!=0){
				printf(" @class=%s\n",gest->window->class);
			}

			if (strcmp("",gest->window->title)!=0){
				printf(" @title=%s\n",gest->window->title);
			}

			printf(":\n");

			break;
		}
	}



	// searching the complex sequence on the global gestures
	if (gest == NULL) {


		for (i = 0; i < global_gestures_count; i++) {
			if (!match_gesture(complex_sequence, activeWindow,
					global_gestures[i])) {
				gest = global_gestures[i];

				printf("Global gesture found:");
				printf("\n");

				break;
			}
		}
	}

	// searching the simple sequence on the application specific gestures
	if (gest == NULL) {

		printf("Gesture not found.\n");
		printf("Searching gesture for alternative sequence: '%s'\n",simple_sequence);
		for (i = 0; i < specific_gestures_count; i++) {
			if (!match_gesture(simple_sequence, activeWindow,
					specific_gestures[i])) {

				printf("Gesture found");
				if (strcmp("",gest->window->class)!=0){
					printf(" @class=%s\n",gest->window->class);
				}

				if (strcmp("",gest->window->title)!=0){
					printf(" @title=%s\n",gest->window->title);
				}

				printf(":\n");

				gest = specific_gestures[i];
				break;
			}
		}
	}

	// searching the simple sequence on the global gestures
	if (gest == NULL) {
		for (i = 0; i < global_gestures_count; i++) {
			if (!match_gesture(simple_sequence, activeWindow,
					global_gestures[i])) {
				gest = global_gestures[i];

				printf("Global gesture found:\n");

				break;
			}
		}
	}


	// execute the action
	if (gest == NULL) {

		printf("Gesture not found.");
		printf("\n");


		execute_action(e, NULL);

	} else {

		printf("    %s\n",gest->movement->name);

		execute_action(e, gest->action);

}


	free(complex_sequence);
	free(simple_sequence);
}

/**
 * Removes the line break from a string
 */
char *remove_new_line(char *str) {
	int len = 0;
	int i;
	if (str == NULL)
		return NULL;
	len = strlen(str);

	for (i = 0; i < len; i++)
		if (str[i] == '\n')
			str[i] = '\0';
	return str;

}

/**
 * Reads the conf file
 */
int read_config(char *conf_file) {

	printf("Reading gestures from %s\n", conf_file);

	FILE *conf = fopen(conf_file, "r");

	struct action *action;
	struct gesture *gest;
	struct window *gesture_window;
	char buff[4096];

	struct movements **known_movements[254];
	int known_movements_num = 0;

	char *buff_ptr = buff;
	char **buff_ptr_ptr;
	char *gesture_action;

	char *gesture_params;
	void *data;
	char *sequence;
	int i;
	char *token;
	char *window_title = NULL;
	char *window_class = NULL;

	if (conf == NULL) {
		fprintf(stderr, "Error reading file from %s\n.", conf_file);
		return -1;
	}

	int currentline = -1;

	// get 1 line
	while (fgets(buff, 4096, conf) != NULL) {

		currentline = currentline + 1;

		// ignore comments
		if (buff[0] == '#')
			continue;

		// remove line breaks
		remove_new_line(buff);

		// ???
		buff_ptr = buff;

		// ???
		buff_ptr_ptr = &buff_ptr;

		// ignoring white spaces
		token = strsep(buff_ptr_ptr, " \t");
		while ((token != NULL) && (strcmp(token, "") == 0)) {
			token = strsep(buff_ptr_ptr, " \t");
		}
		if ((token == NULL) || (strlen(token) == 0))
			continue; // go to next line

		// found a TITLE token

		if (strcasecmp(token, "TITLE") == 0) {
			// get the window title
			token = strsep(buff_ptr_ptr, " \t");
			if (token == NULL)
				continue;
			window_class = NULL; // the class remains empty
			window_title = strdup(token); // copy the window title

			regex_t reg;

			if (regcomp(&reg, window_title, REG_EXTENDED | REG_NOSUB) != 0) {
				printf("Error on compiling a regular expression: \t");
				printf(window_title);
				exit(1); // exit
			}

			gesture_window = alloc_window(window_title, window_class, reg,
					reg/*TODO NULL*/);

			continue; // go to next line
		}

		// found a CLASS token
		if (strcasecmp(token, "CLASS") == 0) {
			// get the window class
			token = strsep(buff_ptr_ptr, " \t");
			if (token == NULL)
				continue;
			window_class = strdup(token);
			window_title = NULL; // the title remain empty

			regex_t reg;

			if (regcomp(&reg, window_class, REG_EXTENDED | REG_NOSUB) != 0) {
				printf("Error on compiling a regular expression: \t");
				printf(window_class);
				exit(1); // exit
			}

			gesture_window = alloc_window(window_title, window_class,
					reg/*TODO NULL*/, reg);

			continue;
		}

		// found a ALL token
		if (strcasecmp(token, "ALL") == 0) {
			// next lines configuration will be valid to any window
			window_class = NULL;
			window_title = NULL;

			regex_t reg; // TODO: Remove this. will not be used

			gesture_window = alloc_window(window_title, window_class,
					reg/*TODO NULL*/, reg/*TODO NULL*/);

			continue;
		}

		// found a MOVEMENT token
		if (strcasecmp(token, "MOVEMENT") == 0) {
			// get the name of the movement
			token = strsep(buff_ptr_ptr, " \t");
			if (token == NULL)
				continue;
			char *movement_name = malloc(sizeof(char) * strlen(token));
			strcpy(movement_name, token);

			// get the regular expression of the movement
			token = strsep(buff_ptr_ptr, " \t");
			if (token == NULL) {
				free(movement_name);
				continue;
			}

			// forces the start and end of the RE
			char *movement_value = malloc(sizeof(char) * (strlen(token) + 5));
			strcpy(movement_value, "");
			strcat(movement_value, "^(");
			strcat(movement_value, token);
			strcat(movement_value, ")$");

			// compile the regular expression for the movement
			regex_t movement_compiled;
			if (regcomp(&movement_compiled, movement_value,
			REG_EXTENDED | REG_NOSUB) != 0) {
				printf("Warning: Invalid movement sequence: %s\tat line %i",
						movement_value, currentline-1);
				exit(1); // exit
			}

			struct movement *movement = alloc_movement(movement_name,
					movement_value, movement_compiled);

			known_movements[known_movements_num] = movement;
			known_movements_num = known_movements_num + 1;
			continue;
		}

		// If not found a token, then this line is a GESTURE definition
		// A GESTURE contains a MOVEMENT name, a ACTION name and the PARAMS to the action

		struct movement *gesture_movement;

		char *movementused = token;

		// TODO remove this variables:
		char *mov_name = "";
		char *mov_value = "";

		// Try to get the Movement from the known movements
		// TODO: create a separated method to do this
		for (i = 0; i < known_movements_num; i++) {

			char *movement_name =
					((struct movement *) (known_movements[i]))->name;
			char *movement_value =
					((struct movement *) (known_movements[i]))->expression;

			if (strcmp(movementused, movement_name) == 0) {
				gesture_movement = ((struct movement *) (known_movements[i]));
				// TODO remove this variables	
				mov_value = movement_value;
				mov_name = movement_name;
				continue;
			}

		}
		if (gesture_movement == NULL) {
			continue; //ingores the movement.
		}

		// get the ACTION name
		token = strsep(buff_ptr_ptr, " \t");
		if (token == NULL)
			continue;
		gesture_action = token;

		// get the PARAMS of the action
		gesture_params = *buff_ptr_ptr; // the remainder chars on the line

		for (i = 1; i < ACTION_LAST; i++) {
			if (strncasecmp(gestures_names[i], token, strlen(gestures_names[i]))
					!= 0)
				continue;

			// ACTION_EXECUTE
			if (i == ACTION_EXECUTE) {
				// parameters mandatory
				if (strlen(gesture_params) <= 0) {
					fprintf(stderr, "error in exec action\n");
					continue;
				}
				data = strdup(gesture_params);
				// ACTION_ROOT_SEND
			} else if (i == ACTION_ROOT_SEND) {
				// parameters mandatory
				if (strlen(gesture_params) <= 0) {
					fprintf(stderr, "error in exec action\n");
					continue;
				}
				// try to compile the key
				data = compile_key_action(gesture_params);
				if (data == NULL) {
					fprintf(stderr, "error reading config file: root_send\n");
					exit(-1);
				}
			} else
				// OTHER ACTIONS DON'T NEED PARAMETERS
				data = NULL;

			// creates an ACTION
			action = alloc_action(i, data);

			// creates the gesture
			sequence = strdup(movementused);

			if (mov_name != "") {
				gest = alloc_gesture(gesture_window, action, gesture_movement);
			} else {
				//gest = alloc_gesture(sequence, action, window_title, window_class, 
				//"Unknown movement");
				printf("Warning: Invalid gesture name at line %i: %s \n",
						currentline, sequence);
				continue;
			}

			// push the gesture to a temporary stack 
			// TODO: the temp_stack remain's necessary??? the is no more sort...
			if ((window_title == NULL) && (window_class == NULL)) {
				push(gest, &temp_general_stack);
			} else {
				push(gest, &temp_stack);
			}

			break;

		}

	}
	// closes the file
	fclose(conf);
	return 0;

}

/**
 * Sort the gesture sequences... // TODO: make a refactoring
 */

int init_gestures(char *config_file) {
	int err = 0;
	int i;

	err = read_config(config_file);
	if (err) {
		fprintf(stderr,
				"Cannot open config file: %s. Falling back to /etc/mygestures.conf  \n",
				config_file);
		err = read_config("/etc/mygestures.conf");
		if (err) {
			fprintf(stderr, "Cannot open config file: /etc/gestures.");
			return err;
		}
	}

	/* now, fill the gesture array */
	if (specific_gestures_count != 0) /* reconfiguration.. */
		free(specific_gestures);

	specific_gestures_count = stack_size(&temp_stack);
	specific_gestures = malloc(
			sizeof(struct gesture *) * specific_gestures_count);

	/* printf("got %d gests\n", known_gestures_num); */

	for (i = 0; i < specific_gestures_count; i++) {
		specific_gestures[i] = (struct gesture *) pop(&temp_stack);
	}

	/* now, fill the gesture array */
	if (global_gestures_count != 0) /* reconfiguration.. */
		free(global_gestures);
	global_gestures_count = stack_size(&temp_general_stack);
	global_gestures = malloc(sizeof(struct gesture *) * global_gestures_count);


	for (i = 0; i < global_gestures_count; i++) {
		global_gestures[i] = (struct gesture *) pop(&temp_general_stack);
	}
	// the gesture sequences are now regular expressions.
	// since there are no method to know if a regular expression is greatest to other, there 
	// aren't way to sort the gestures array.
	// It is necessary to try to match the recognized pattern with all the patterns on the array.

	return err;
}
