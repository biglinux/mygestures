/*
 *   Copyright 2002-2004 Peter Osterlund <petero2@telia.com>
 *   Copyright 2016      Lucas Augusto Deters
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License
 *   as published by the Free Software Foundation; either version 2
 *   of the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */


#include <X11/extensions/XInput.h>



#include "grabbing-synaptics.h"

#define SHM_SYNAPTICS 23947

#define SYNAPTICS_PROP_TAP_ACTION "Synaptics Tap Action"

static XDevice *
dp_get_device(Display *dpy)
{
	return 0;
}

typedef struct _SynapticsSHM
{
	int version; /* Driver version */

} SynapticsSHM;

static int synaptics_shm_is_equal(SynapticsSHM *s1, SynapticsSHM *s2)
{
    return 0;

}

/** Init and return SHM area or NULL on error */
static SynapticsSHM *
grabber_synaptics_shm_init(int debug)
{
	return 0;
}

void syn_print(const SynapticsSHM *cur)
{
    return 0;
}

void synaptics_disable_3fingers_tap(Grabber *self, XDevice *dev)
{
    return 0;

}

void grabber_synaptics_loop(Grabber *self, Configuration *conf)
{
    return 0;
}
