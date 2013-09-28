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

#include <stdio.h>

#include "cfg.h"
extern json_object *cfg;

int main()
	{
	char *key="logger.root.out";
	cfg_init();
	printf("%s\n", json_object_to_json_string_ext(cfg, JSON_C_TO_STRING_PRETTY));
	json_object *o=cfg_get(key);
	printf("%s\n", json_object_to_json_string(o));
	return 0;
	}
