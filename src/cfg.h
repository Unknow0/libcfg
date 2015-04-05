/*******************************************************************************
 * This file is part of libcfg.
 *
 * libcfg is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * libcfg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along
 * with libcfg; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 ******************************************************************************/
#ifndef _CFG_H
#define _CFG_H

#include <json-c/json.h>

/** init default configuration by loading file /etc/cfg and ~/.config/cfg  and all *.cfg under /etc/cfg.d/ */
int cfg_init(char **other_path);

/** clean up */
void cfg_deinit();

/** return NULL if we can't find a key */
json_object *cfg_get(char *key);

const char *cfg_get_string(char *key);

/** return 0 if doesn't exist and 0 + errno = EINVAL if it's not a int */
int64_t cfg_get_int(char *key);

/** return 0 if key don't exist, 0 and errno = EINVAL if it's not a double
 * closest to infinity and ERANGE if value too big to fit a double.
 */
double cfg_get_double(char *key);

/** retrun false if key don't exist, convert int/double to boolean the c way if it's a string return false if empty. */
int cfg_get_bool(char *key);

int cfg_has_key(char *key);

/**
 * aggregate file under the key part in base.
 * if base == NULL the root cfg will be taken.
 * if key == NULL data will be direcly merged under base else if key don't existe under base it will be created.
 * return 0 on sucess, 1 if file is null, 2 if open(2) throw an error (see errno) and 3 if read(2) does.
 */
int cfg_aggregate_file(const char *file, char *key, json_object *base);

/**
 * aggregate a directory recursively under root cfg.
 */
void cfg_aggregate_dir(const char *name);

#endif
