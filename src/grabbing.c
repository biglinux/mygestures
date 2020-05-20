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

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/extensions/XTest.h>	/* emulating device events */
#include <X11/extensions/XInput2.h> /* capturing device events */


#include "grabbing.h"
#include "actions.h"

#ifndef MAX_STROKES_PER_CAPTURE
#define MAX_STROKES_PER_CAPTURE 1 /*TODO*/
#endif

const char stroke_representations[] = {' '};

static void grabber_open_display(Grabber *self)
{

	self->dpy = XOpenDisplay(NULL);

	if (!XQueryExtension(self->dpy, "XInputExtension", &(self->opcode),
						 &(self->event), &(self->error)))
	{
		printf("X Input extension not available.\n");
		exit(-1);
	}

	int major = 2, minor = 0;
	if (XIQueryVersion(self->dpy, &major, &minor) == BadRequest)
	{
		printf("XI2 not available. Server supports %d.%d\n", major, minor);
		exit(-1);
	}
}

static struct brush_image_t *get_brush_image(char *color)
{

	return;
}

static void grabber_init_drawing(Grabber *self)
{

    return;
}

static Status fetch_window_title(Display *dpy, Window w, char **out_window_title)
{


	return 1;
}

/*
 * Return a window_info struct for the focused window at a given Display.
 */
static ActiveWindowInfo *get_active_window_info(Display *dpy, Window win)
{


	return 1;
}

static Window get_parent_window(Display *dpy, Window w)
{

	return 1;
}

void grabbing_xinput_grab_start(Grabber *self)
{


	int screen;


		Window rootwindow = RootWindow(self->dpy, screen);

		if (!self->is_direct_touch)
		{

				self->button = 3;
			

			unsigned char mask_data[2] = {
				0,
			};
			XIEventMask mask = {
				XIAllDevices, sizeof(mask_data), mask_data};

			int nmods = 0;
			XIGrabModifiers mods[0] = {
				{0, 0},					 // no modifiers
				{LockMask, 0},			 // Caps lock
				{Mod2Mask, 0},			 // Num lock
				{LockMask | Mod2Mask, 0} // Caps & Num lock
			};

			nmods = 1;
			mods[0].modifiers = XIAnyModifier;

			int res = XIGrabButton(self->dpy, self->deviceid, self->button,
								   rootwindow, None,
								   GrabModeAsync, GrabModeAsync, False, &mask, nmods, mods);
		}
	
}

void grabbing_xinput_grab_stop(Grabber *self)
{

	int count = XScreenCount(self->dpy);

	int screen;
	for (screen = 0; screen < count; screen++)
	{

		Window rootwindow = RootWindow(self->dpy, screen);

		if (self->is_direct_touch)
		{

			int status = XIUngrabDevice(self->dpy, self->deviceid, CurrentTime);
		}
		else
		{
			XIGrabModifiers modifiers[4] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};
			XIGrabModifiers mods = {
				XIAnyModifier};
			XIUngrabButton(self->dpy, self->deviceid, self->button, rootwindow,
						   1, &mods);
		}
	}
}

static void mouse_click(Display *display, int button, int x, int y)
{
    XTestFakeButtonEvent(display, 3, True, 0);
	XTestFakeButtonEvent(display, 3, False, 0);
}

static Window get_window_under_pointer(Display *dpy)
{

return 0;
}

static Window get_focused_window(Display *dpy)
{

	return 0;
}

static void execute_action(Display *dpy, Action *action, Window focused_window)
{

	return;
}

static void free_grabbed(Capture *free_me)
{
return 1;
}

static char get_fine_direction_from_deltas(int x_delta, int y_delta)
{

	return 0;
}

static char get_direction_from_deltas(int x_delta, int y_delta)
{
    return 0;
}

static void movement_add_direction(char *stroke_sequence, char direction)
{
return;
}

static int get_touch_status(XIDeviceInfo *device)
{


	return 1;
}

static void grabber_xinput_open_devices(Grabber *self, int verbose)
{
return 1;
}

/**
 * Clear previous movement data.
 */
void grabbing_start_movement(Grabber *self, int new_x, int new_y)
{

	return;
}

void grabbing_update_movement(Grabber *self, int new_x, int new_y)
{

	return;
}

/**
 *
 */
void grabbing_end_movement(Grabber *self, int new_x, int new_y,
						   char *device_name, Configuration *conf)
{

		if (!(self->synaptics))
		{

			printf("\nEmulating click\n");

			grabbing_xinput_grab_stop(self);
			mouse_click(self->dpy, self->button, new_x, new_y);
			grabbing_xinput_grab_start(self);
            
            
		}
	

	grabbing_xinput_grab_start(self);

}

void grabber_set_button(Grabber *self, int button)
{
	self->button = button;
}

void grabber_set_device(Grabber *self, char *device_name)
{
	self->devicename = device_name;


}

void grabber_set_brush_color(Grabber *self, char *brush_color)
{
	return;
}

Grabber *grabber_new(char *device_name, int button)
{

	Grabber *self = malloc(sizeof(Grabber));
	return self;
}

char *get_device_name_from_event(Grabber *self, XIDeviceEvent *data)
{
return;
}

void grabber_list_devices(Grabber *self)
{
return;
};

void grabber_xinput_loop(Grabber *self, Configuration *conf)
{

	XEvent ev;

	grabbing_xinput_grab_start(self);

	//while (!self->shut_down)
	//{

		XNextEvent(self->dpy, &ev);

		if (ev.xcookie.type == GenericEvent && ev.xcookie.extension == self->opcode && XGetEventData(self->dpy, &ev.xcookie))
		{

			XIDeviceEvent *data = NULL;

			switch (ev.xcookie.evtype)
			{

			case XI_ButtonPress:
				data = (XIDeviceEvent *)ev.xcookie.data;

				char *device_name = get_device_name_from_event(self, data);

				grabbing_xinput_grab_stop(self);
				grabbing_end_movement(self, data->root_x, data->root_y,
									  device_name, conf);
                //break;
			}

		//}

	}
}

void grabber_loop(Grabber *self, Configuration *conf)
{

	grabber_open_display(self);

	grabber_init_drawing(self);

	grabber_xinput_loop(self, conf);


	printf("Grabbing loop finished for device '%s'.\n", self->devicename);
}

char *grabber_get_device_name(Grabber *self)
{
	return self->devicename;
}

