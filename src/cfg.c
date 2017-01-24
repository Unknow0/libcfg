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
#include <stdlib.h>

#include "cfg.h"

#define BUF_SIZE 1024

char *DEFAULT_PATH[4]={"/etc/cfg", "/etc/cfg.d/", "~/.config/cfg", NULL};

json_object *cfg=NULL;

void merge(json_object *dest, json_object *src)
	{
	json_object_object_foreach(src, key, val)
		{
		json_object *o=NULL;
		json_object_object_get_ex(dest, key, &o);
		if(o==NULL || !json_object_is_type(val, json_type_object) || !json_object_is_type(o, json_type_object))
			{
			json_object_get(val);
			json_object_object_add(dest, key, val);
			}
		else
			merge(o, val);
		}
	}

int cfg_aggregate_file(const char *file, char *key, json_object *base)
	{
	int f;
	json_object *o;
	ssize_t s;
	json_tokener *tok;
	enum json_tokener_error jerr;
	char buf[BUF_SIZE];
	if(file==NULL)
		return 1;
	f=open(file, O_RDONLY);
	if(f<0)
		return 2;
	tok=json_tokener_new();
	do
		{
		s=read(f, buf, BUF_SIZE);
		buf[s]=0;
		if(s>0)
			o=json_tokener_parse_ex(tok, buf, s);
		}
	while(s>0 && (jerr = json_tokener_get_error(tok)) == json_tokener_continue);
	close(f);
	json_tokener_free(tok);
	if(s<0)
		{
		json_object_put(o);
		return 3;
		}
	if(jerr != json_tokener_success) // TODO return json error?
		{
		json_object_put(o);
		return 4;
		}

	if(base==NULL)
		base=cfg;
	if(key!=NULL)
		{
		json_object *jo=NULL;
		json_object_object_get_ex(base, key, &jo);
		if(jo==NULL)
			{
			jo=json_object_new_object();
			json_object_object_add(base, key, jo);
			}
		base=jo;
		}
	merge(base, o);
	json_object_put(o);

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
			json_object *o=NULL;
			json_object_object_get_ex(obj, d->d_name, &o);
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

void cfg_deinit()
	{
	if(cfg!=NULL)
		{
		json_object_put(cfg);
		cfg=NULL;
		}
	}

int cfg_aggregate(char *path)
	{
	char *home=getenv("HOME");
	char *p=NULL;
	size_t len=strlen(path);
	if(path[0]=='~' && home)
		{
		len=strlen(home)+strlen(path)-1;
		p=malloc(len+1);
		strcpy(p, home);
		strcpy(p, path+1);
		path=p;
		}
	if(path[len-1]=='/')
		cfg_aggregate_dir(path);
	else
		cfg_aggregate_file(path, NULL, cfg);
	}

int cfg_init(char **path)
	{
	if(cfg!=NULL)
		return 1;
	cfg=json_object_new_object();
	if(!path)
		path=DEFAULT_PATH;
	while(*path)
		{
		cfg_aggregate(*path);
		path++;
		}
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
		json_object *jo=NULL;
		char b;
		while(*(c+end)!=0 && *(c+end)!='.')
			end++;
		b=c[end];
		c[end]=0;
		json_object_object_get_ex(o, c, &jo);
		o=jo;
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
