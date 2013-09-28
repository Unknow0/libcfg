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
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>

#include <json/json.h>

#define BUF_SIZE 1024

json_object *cfg=NULL;

void merge(json_object *dest, json_object *src)
	{
	json_object_object_foreach(src, key, val)
		{
		json_object *o=json_object_object_get(dest, key);
		if(o!=NULL || !json_object_is_type(val, json_type_object) || !json_object_is_type(o, json_type_object))
			json_object_object_add(dest, key, val);
		else
			merge(o, val);
		}
	}

int cfg_aggregate_file(const char *file, char *key, json_object *base)
	{
	int f;
	json_object *o;
	ssize_t s;
	json_tokener *tok=json_tokener_new();
	char buf[BUF_SIZE];
	if(file==NULL)
		return 1;
	f=open(file, O_RDONLY);
	if(f<0)
		return 2;
	do
		{
		s=read(f, buf, BUF_SIZE);
		buf[s]=0;
		if(s>0)
			o=json_tokener_parse_ex(tok, buf, s);
		}
	while(s>0);
	if(s<0)
		return 3;

	if(base==NULL)
		base=cfg;
	if(key!=NULL)
		{
		json_object *jo=json_object_object_get(base, key);
		if(jo==NULL)
			{
			jo=json_object_new_object();
			json_object_object_add(base, key, jo);
			}
		base=jo;
		}
	merge(base, o);

	return 0;
	}

void _cfg_aggregate_dir(char **name, int len, int *alloc, json_object *obj)
	{
	DIR *dir=opendir(*name);
	struct dirent *d;
	if(dir==NULL)
		return;
	while((d=readdir(dir))!=NULL)
		{
		if(d->d_name[0]=='.')
			continue;

		int s=strlen(d->d_name);
		char *c=d->d_name;
		while(*c!=0 && *c!='.')
			c++;
		if(c[0]=='.' && d->d_type==DT_DIR || (c[0]!='.' || c[1]!='c' || c[2]!='f' || c[3]!='g' || c[4]!=0) && d->d_type!=DT_DIR)
			continue;
		if(*alloc<len+s+2)
			{
			*name=realloc(*name, len+s+2);
			*alloc=len+s+1;
			}
		strcat(*name, d->d_name);
		if(d->d_type==DT_DIR)
			{
			json_object *o=json_object_object_get(obj, d->d_name);
			if(o==NULL)
				{
				o=json_object_new_object();
				json_object_object_add(obj, d->d_name, o);
				}
			_cfg_aggregate_dir(name, len+s, alloc, o);
			}
		else
			{
			c[0]=0;
			cfg_aggregate_file(*name, d->d_name, obj);
			}
		*(*name+len)=0;
		}
	closedir(dir);
	}
void cfg_aggregate_dir(const char *name)
	{
	int len=1024;
	char *n=malloc(len);
	strcpy(n, name);
	_cfg_aggregate_dir(&n, strlen(n), &len, cfg);
	free(n);
	}

int cfg_init()
	{
	char *home, *s;
	cfg=json_object_new_object();
	cfg_aggregate_file("/etc/cfg", NULL, cfg);
	cfg_aggregate_dir("/etc/cfg.d/");

	home=__secure_getenv("HOME");
	s=malloc(strlen(home)+12);
	strcpy(s, home);
	strcat(s, "/.config/cfg");
	cfg_aggregate_file(s, NULL, cfg);

	return 0;
	} 

json_object *cfg_get(char *key)
	{
	int end=0;
	char *dup=strdup(key);
	char *c=dup;
	json_object *o=cfg;

	do
		{
		char b;
		while(*(c+end)!=0 && *(c+end)!='.')
			end++;
		b=c[end];
		c[end]=0;
		o=json_object_object_get(o, c);
		c[end]=b;
		c+=end+1;
		end=0;
		}
	while (o!=NULL && *(c-1)!=0);
	free(dup);
	return o;
	}

const char *cfg_get_string(char *key)
	{
	json_object *o=cfg_get(key);
	return json_object_get_string(o);
	}

int64_t cfg_get_int(char *key)
	{
	json_object *o=cfg_get(key);
	return json_object_get_int64(o);
	}

double cfg_get_double(char *key)
	{
	json_object *o=cfg_get(key);
	return json_object_get_double(o);
	}

int cfg_get_bool(char *key)
	{
	json_object *o=cfg_get(key);
	return json_object_get_boolean(o);
	}

int cfg_has_key(char *key)
	{
	json_object *o=cfg_get(key);
	return o!=NULL;
	}
