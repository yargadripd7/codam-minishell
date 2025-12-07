//functionality on top of src/env-get-set-unset.c
#include "environ.h"
#include "utils.h"
#include "hash.h"
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pwd.h> //line 57
#include <assert.h>

typedef struct t_ms_env
{
	char	*id;
	char	*val;
} t_ms_env;

enum e_ms_envs
{
	CDPATH = 0, //=".:"
	HOME,       //=getpwuid(getuid())->pw_dir
	IFS,        //=" \t\n"
	MAIL,       //=
	MAILPATH,   //=
	OPTARG,     //=""
	OPTIND,     //=""
	PATH,       //="/usr/local/bin:/usr/local/sbin:/usr/bin:/usr/sbin:/bin:/sbin:"
	PS1,        //="\\s-\\v\\$ "
	PS2,        //="> "
	SHELL,      //=getpwuid(getuid())->pw_shell
	ENV_TOTAL,  //last
};

//DECIDE: Create defaults for some of these bourne variables
//Visit:  https://www.gnu.org/software/bash/manual/html_node/Bourne-Shell-Variables.html
//Or see: docs/BOURNE_SHELL.md
bool	ms_setenv_bourne(t_environ *e)
{
	uint32_t    	i;
	t_variable  	*var;
	const t_ms_env	envs[] = {
		[CDPATH]   = {.id = "CDPATH",   .val = ".:"},
		[IFS]      = {.id = "IFS",      .val = " \t\n"},
		[HOME]     = {.id = "HOME",     .val = getpwuid(getuid())->pw_dir},
		[MAIL]     = {.id = "MAIL",     .val = ""}, 
		[MAILPATH] = {.id = "MAILPATH", .val = ""}, 
		[OPTARG]   = {.id = "OPTARG",   .val = ""}, 
		[OPTIND]   = {.id = "OPTIND",   .val = ""}, 
		[PATH]     = {.id = "PATH",     .val = "/usr/local/bin:/usr/local/sbin"
		                                    ":/usr/bin:/usr/sbin:/bin:/sbin:"},
		[PS1]      = {.id = "PS1",      .val = "\\s-\\v\\$ "},
		[PS2]      = {.id = "PS2",      .val = "> "},
		[SHELL]    = {.id = "SHELL",    .val = getpwuid(getuid())->pw_shell},
	}; assert(sizeof(envs)/sizeof(*envs) == ENV_TOTAL);

	i = -1;
	while (++i < (sizeof(envs)/sizeof(*envs)) )
	{
		var = ms_getvar(envs[i].id, e);
		if (var == NULL || (var->key == NULL && !ms_setenv(envs[i].id,
				envs[i].val, false, (t_variable_about){0}, e)) == false) {
			return (false);
		}
	}
	return (true);
}

extern char **environ;
//copies <stdlib.h>'s environ.
bool	ms_setenv_environ(t_environ *e)
{
	size_t	i;
	size_t	len;
	size_t	size;

	len = 0;
	while (environ[len])
		len++;
	size = (len / MS_ENVSIZE + 1) * MS_ENVSIZE + MS_ENVSIZE;
//	ms_log("[INFO] ms_setenv_environ() len=%zu size=%zu\n", len, size);
	if (ms_envresize(e, size) == false)
		return (ms_log("[ERROR] ms_setenv_environ() ms_envresize() malloc() failed"), false);
	i = -1;
	while (environ[++i])
		if (!ms_setvar(environ[i], 0, (t_variable_about){.length = strlen(environ[i])}, e))
			return (false);
	return (true);
}
	//ms_log("[INFO] setting environ '%.*s' environ i=%u\n", (int)ms_strchrlen(environ[i], '='), environ[i], i);
	//fflush(stdout);

#include <assert.h>
bool	ms_envinit(t_environ *e)
{
	assert(MS_ENVSIZE > 0);
	assert(MS_HASH_MULT > 0);
	assert(MS_HASH_THRESHOLD > 0);
	if (ms_setenv_environ(e) == false)
		return (false);
	if (ms_setenv_bourne(e))
		return (false);
	return (false);
}

//DECIDE: Cant generalise this because won't know where string will be from void *
//		    It also must match on the string until '='. Cant use ptr address
//		    because var.key may be realloc'ed.
bool	ms_envresize(t_environ *e, size_t newsize)
{
	size_t		     i;
	size_t		     len;
	uint32_t	     hash;
	uint32_t	     seed;
	t_variable       *new;
	const t_variable *old = e->vars;

	ms_log("[INFO] ms_envresize() got newsize=%zu\n", newsize);
	new = malloc(newsize * sizeof(t_variable));
	if (new == NULL)
		return (ms_log("[ERROR] ms_extendenv() malloc() failed"), false);
	memset(new, '\0', newsize * sizeof(t_variable));
	if (old == NULL)
		return (e->size = newsize,  old = new,  true);
	i = -1;
	while (++i < e->size)
	{
		if (old[i].key)
		{
			len  = ms_strchrlen(old[i].key, '=');
			seed = e->seed;
			do {
				if (seed - e->seed >= MS_HASH_THRESHOLD) //if vars don't fit or need new seed
					return (free(new), ms_envresize(e, newsize + MS_ENVSIZE)); //try
				hash = ms_hash(old[i].key, len, seed);
			} while (new[hash % newsize].key != NULL && ++seed);
			ms_log("[INFO] ms_envresize() resizing var'%.*s' old=[%zu, %zu] to new=[%zu, %zu]\n",
					(int)len, old[i].key, e->size, hash % newsize);
			fflush(stdout);
			memcpy(new + (hash % newsize), old + (hash % e->size), sizeof(t_variable));
		}
	}
	if (e->size)
		free((t_variable *)old);
	e->size = newsize;
	e->vars = new;
	return (true);
} 

