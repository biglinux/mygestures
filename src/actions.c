/*
 Copyright 2008-2016 Lucas Augusto Deters
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

#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "actions.h"
#include "configuration.h"

/* Actions */
const char *action_name[ACTION_COUNT] = {
	"ERROR", "EXIT_GEST", "EXECUTE", "ICONIFY", "KILL", "RECONF", "RAISE", "LOWER", "MAXIMIZE",
	"RESTORE", "TOGGLE_MAXIMIZED", "KEYPRESS", "ABORT", "LAST"};

const char *get_action_name(int action)
{
	return action_name[action];
};

enum
{
	_NET_WM_STATE_REMOVE = 0,
	_NET_WM_STATE_ADD = 1,
	_NET_WM_STATE_TOGGLE = 2
};

/*
 * Iconify the focused window at given display.
 *
 * PUBLIC
 */
void action_iconify(Display *dpy, Window w)
{
	if (w != None)
		XIconifyWindow(dpy, w, 0);

	
	return;
}

/**
 * Kill focused window at the given Display.
 *
 * PUBLIC
 */
void action_kill(Display *dpy, Window w)
{

	/* dont kill root window */
	if (w == RootWindow(dpy, DefaultScreen(dpy)))
		return;

	XSync(dpy, 0);
	XKillClient(dpy, w);
	XSync(dpy, 0);
	return;
}

/**
 * Raise the focused window at the given Display.
 *
 * PUBLIC
 */
void action_raise(Display *dpy, Window w)
{
	XRaiseWindow(dpy, w);
	return;
}

/**
 * Lower the focused window at the given Display.
 *
 * PUBLIC
 */
void action_lower(Display *dpy, Window w)
{
	XLowerWindow(dpy, w);
	return;
}

/*
 * Taken from wmctrl
 */
static int client_msg(Display *disp,
					  Window win,
					  char *msg,
					  unsigned long data0,
					  unsigned long data1,
					  unsigned long data2,
					  unsigned long data3,
					  unsigned long data4)
{
	XEvent event;
	long mask = SubstructureRedirectMask | SubstructureNotifyMask;

	event.xclient.type = ClientMessage;
	event.xclient.serial = 0;
	event.xclient.send_event = True;
	event.xclient.message_type = XInternAtom(disp, msg, False);
	event.xclient.window = win;
	event.xclient.format = 32;
	event.xclient.data.l[0] = data0;
	event.xclient.data.l[1] = data1;
	event.xclient.data.l[2] = data2;
	event.xclient.data.l[3] = data3;
	event.xclient.data.l[4] = data4;

	if (XSendEvent(disp, DefaultRootWindow(disp), False, mask, &event))
	{
		XFlush(disp);
		return EXIT_SUCCESS;
	}
	else
	{
		fprintf(stderr, "Cannot send %s event.\n", msg);
		return EXIT_FAILURE;
	}
}

/**
 * Maximize the focused window at the given Display.
 *
 * PUBLIC
 */
void action_toggle_maximized(Display *dpy, Window w)
{

	unsigned long action;
	Atom prop1 = 0;
	Atom prop2 = 0;

	action = _NET_WM_STATE_TOGGLE;

	char *tmp_prop2;
	tmp_prop2 = "_NET_WM_STATE_MAXIMIZED_HORZ";
	prop2 = XInternAtom(dpy, tmp_prop2, False);
	char *tmp_prop1 = "_NET_WM_STATE_MAXIMIZED_VERT";

	prop1 = XInternAtom(dpy, tmp_prop1, False);

	client_msg(dpy, w, "_NET_WM_STATE", action, (unsigned long)prop1, (unsigned long)prop2, 0, 0);
}

/**
 * Maximize the focused window at the given Display.
 *
 * PUBLIC
 */
void action_restore(Display *dpy, Window w)
{

	unsigned long action;
	Atom prop1 = 0;
	Atom prop2 = 0;

	action = _NET_WM_STATE_REMOVE;

	char *tmp_prop2;
	tmp_prop2 = "_NET_WM_STATE_MAXIMIZED_HORZ";
	prop2 = XInternAtom(dpy, tmp_prop2, False);
	char *tmp_prop1 = "_NET_WM_STATE_MAXIMIZED_VERT";

	prop1 = XInternAtom(dpy, tmp_prop1, False);

	client_msg(dpy, w, "_NET_WM_STATE", action, (unsigned long)prop1, (unsigned long)prop2, 0, 0);
}

/**
 * Maximize the focused window at the given Display.
 *
 * PUBLIC
 */
void action_maximize(Display *dpy, Window w)
{

	unsigned long action;
	Atom prop1 = 0;
	Atom prop2 = 0;

	action = _NET_WM_STATE_ADD;

	char *tmp_prop2;
	tmp_prop2 = "_NET_WM_STATE_MAXIMIZED_HORZ";
	prop2 = XInternAtom(dpy, tmp_prop2, False);
	char *tmp_prop1 = "_NET_WM_STATE_MAXIMIZED_VERT";

	prop1 = XInternAtom(dpy, tmp_prop1, False);

	client_msg(dpy, w, "_NET_WM_STATE", action, (unsigned long)prop1, (unsigned long)prop2, 0, 0);
}

/**
 * Fake key event
 */
void press_key(Display *dpy, KeySym key, Bool is_press)
{

	XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, key), is_press, CurrentTime);
	return;
}

/* alloc a key_press struct ???? */
struct key_press *alloc_key_press(void)
{
	struct key_press *ans = malloc(sizeof(struct key_press));
	bzero(ans, sizeof(struct key_press));
	return ans;
}

/**
 * Creates a Keysym from a char sequence
 *
 * PRIVATE
 */
struct key_press *create_key_chain(char *original_string)
{

	if (original_string == NULL)
		return NULL;

	char *end, *r, *tok;

	struct key_press *head = NULL;
	struct key_press *tail = NULL;

	KeySym k;

	r = end = strdup(original_string);

	while ((tok = strsep(&end, "+ \n")) != NULL)
	{

		/* printf("found : %s\n", token); */
		k = XStringToKeysym(tok);
		if (k == NoSymbol)
		{
			fprintf(stderr, "error converting %s to keysym\n", tok);
			exit(-1);
		}

		struct key_press *current = alloc_key_press();
		current->key = k;
		current->original_str = original_string;

		if (!tail)
		{
			head = current;
			tail = current;
		}
		else
		{
			tail->next = current;
			tail = current;
		}
	}

	free(r);
	return head;
}

void free_key_chain(struct key_press *keys)
{

	struct key_press *k = keys;

	while (k != NULL)
	{
		struct key_press *next = k->next;
		free(k);
		k = next;
	}

	return;
}

/**
 * Fake sequence key events
 */
void action_keypress(Display *dpy, char *data)
{

	struct key_press *keys = create_key_chain(data);

	struct key_press *k = keys;

	while (k != NULL)
	{
		press_key(dpy, k->key, True);
		k = k->next;
	}

	k = keys;

	while (k != NULL)
	{
		press_key(dpy, k->key, False);
		k = k->next;
	}

	free_key_chain(keys);
}
