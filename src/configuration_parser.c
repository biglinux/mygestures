/*
 Copyright 2015-2016 Lucas Augusto Deters

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

#define _GNU_SOURCE /* needed by asprintf */

#include <string.h>
#include <libxml/tree.h>


#include "assert.h"

#include "config.h"
#include "actions.h"
#include "configuration_parser.h"

const char * CONFIG_FILE_NAME = "mygestures.xml";

void xml_parse_action(xmlNode *node, Gesture * gest) {
    return;

}

static Gesture * xml_parse_gesture(xmlNode *node, Context * context) {
	return 0;

}

static Context * xml_parse_context(xmlNode *node, Configuration * eng) {
	return 0;

}

void xml_parse_movement(xmlNode *node, Configuration * eng) {
    return 0;
}

void xml_parse_root(xmlNode *node, Configuration * eng) {
    return 0;
}

static int configuration_parse_file(Configuration * conf, char * filename) {
	return 0;

}

static char * get_config_dir() {

	return 0;
}

char * xml_get_template_filename() {

	return 0;
}

int file_copy(const char *from, const char *to) {
	return 0;
}

void test_create_dir(char* dir) {
return 0;
}

char* configuration_get_default_filename() {
return 0;
}

void configuration_load_from_defaults(Configuration * configuration) {
return 0;
}

void configuration_load_from_file(Configuration * configuration, char * filename) {
return 0;
}
