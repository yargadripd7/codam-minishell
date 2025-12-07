//ERRORold code from ms_envinit():
	//ptrsize = 0;
	//while (environ[ptrsize])
	//	ptrsize++;
	//ptrsize = ms_round_to_pow_2(ptrsize) * 2;
	//e->vars = malloc(ptrsize * sizeof(t_variable));
	//if (e->vars == NULL)
	//	return (perror("Error: ms_envinit() malloc() failed"), false);
	//memset(&e->vars, 0, ptrsize * sizeof(t_variable));
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "utils.h"
#include "environ.h"
#include "hash.h"

//returns where env will go or where it is. or NULL on malloc() error.
t_variable	*ms_getvar(char *key, t_environ *e)
{
	t_variable	*var;
	size_t		len;
	uint32_t	seed;
	uint32_t	hash;
	
	len  = ms_strchrlen(key, '=');
	seed = e->seed;
	do {
		hash = ms_hash(key, len, seed) % e->size;
		var = e->vars + hash;
	} while (var != NULL
			&& var->key != NULL //segv NULL read wtf???
			&& strncmp(key, var->key, len)
			&& ++seed < e->seed + MS_HASH_THRESHOLD
			);
	if (seed >= e->seed + MS_HASH_THRESHOLD
			&& var->key != NULL
			&& ms_envresize(e, e->size + MS_ENVSIZE) == false)
		return (ms_log("[ERROR] ms_getvar() ms_envresize() failed"), NULL);
	ms_log("[INFO] ms_getvar() % 4zu key='%.*s'\n", hash, (int)len, key);
	if (var->key)
		ms_log("got: key='%.*s', value='%s', size=%zu, abt=[.local=%d, .typeofvar=%d, "
			".readonly=%d, .width=%d, .length=%d]}\n",
			(int)ms_strchrlen(var->key, '='), var->key,
			var->value.str, var->size, var->abt.local, var->abt.typeofvar,
			var->abt.readonly, var->abt.width, var->abt.length
		);
	return (ret); //return either the env or a new env slot.
}

//keysize > 0 means internal (minishell) variable. which can be freed and realloced.
//fills a variable slot, `str` is in form: [key]=[value]
//                                     or: [key]=[width][length][array]
//                           depending on: `abt.typeofvar`
//                           char *key, char *value, uchar width, uint length,
//                           u_ms_env_value *array.
//by hash until '='. if '=' is not encountered take full string as id, set an empty value.
bool	ms_setvar(char *key, size_t keysize, t_variable_about abt, t_environ *e)
{
	t_variable *var;
	int        len;

	if (key == NULL || e == NULL)
		return (ms_log("[ERROR] ms_setenv_str() got NULL pointer"), false);
	if (key[ms_strchrlen(key, '=')] != '=')
		return (ms_log("[ERROR] ms_setenv_str() didn't get an = (equalsign)"), false);
	len  = ms_strchrlen(key, '=');
	var = ms_getvar(key, e); //TODO: segv
	if (var == NULL)
	{
		ms_log("[ERROR] ms_setenv_slot() ms_getvar() ms_envresize() failed");
		return (false);
	}
	ms_log("[INFO] ms_setvar() % 4lu from: [key='%s', size=%zu, "
		"abt={l=%u, w=%u, loc=%d, readonly=%d, typeofvar=%d}]\n% 28s: "
		"[key='%s', size=%zu, abt={l=%u, w=%u, loc=%d, readonly=%d, typeofvar=%d}]\n",
		var - e->vars,  var->key,  var->size,  var->abt.length,  var->abt.width,
		var->abt.local,  var->abt.readonly,  var->abt.typeofvar, "to",
		key,  keysize,  abt.length,  abt.width,  abt.local,  abt.readonly, abt.typeofvar
	);
	if (var->key && strncmp(key, var->key, len))
	{
		ms_log("[WARN] ms_setvar(): ms_unsetvar() did his job incorrectly. "
				"not touching variable '%.*s' with given key '%.*s'...",
				var->key, (int)ms_strchrlen(var->key, '='), key, len
		);
		return (true);
	}
	if (var->abt.readonly == true && var->key != NULL)
		return (true);
	if (var->size > 0)
		free(var->key);
	var->value.str = key + len + (key[len] == '=');
	var->size = keysize;
//	if (keysize == 0)
//		key  = strdup(key);
	var->key = key; //DECIDE: strdup(key)?  var->size could mean an external free.
	return (true);
}

//TODO: TEST ENV- PREPEND APPEND UNSET
bool	ms_prependvar(char *id, char *new, t_variable_about abt, t_environ *e)
{
	size_t newlen;
	size_t oldlen;
	size_t idlen;
	char   *combined;
 
	if (id == NULL || *id == '\0')
		return (ms_log("[ERROR] ms_prependenv() got NULL pointer or empty key"), false);
	if (new == NULL || *new == '\0')
		return (ms_setvar(id, 0, abt, e)); //take id as-is, do nothing
	idlen  = ms_strchrlen(id, '=');
	oldlen = strlen(id + idlen);
	newlen = strlen(new);
	combined = malloc(idlen + oldlen + newlen + 2); //2 because '\0' and '='
	if (combined == NULL)
		return (false);
	combined[idlen + oldlen + newlen + 1] = '\0';
	memcpy(combined, id, idlen);
	combined[idlen] = '=';
	memcpy(combined + idlen + 1, new, newlen);
	memcpy(combined + idlen + 1 + newlen, id + idlen + 1, oldlen);
	return (ms_setvar(combined, idlen + newlen, (t_variable_about){0}, e));
}

bool	ms_appendvar(char *id, char *new, t_variable_about abt, t_environ *e)
{
	size_t newlen;
	size_t oldlen;
	size_t idlen;
	char   *combined;
 
	if (id == NULL)
		return (ms_log("[ERROR] ms_appendenv() got NULL pointer"), false);
	if (new == NULL)
		return (ms_setvar(id, 0, (t_variable_about){0}, e)); //take id as-is, do nothing
	idlen  = ms_strchrlen(id, '=');
	oldlen = strlen(id + idlen);
	newlen = strlen(new);
	combined = malloc(idlen + oldlen + newlen + 2); //2 because '\0' and '='
	if (combined == NULL)
		return (false);
	combined[idlen + oldlen + newlen + 1] = '\0';
	memcpy(combined, id, idlen);
	combined[idlen] = '=';
	memcpy(combined + idlen + 1, id + idlen + 1, oldlen);
	memcpy(combined + idlen + 1 + oldlen, new, newlen);
	return (ms_setvar(combined, idlen + newlen, (t_variable_about){0}, e));
}

void	ms_unsetvar(char *key, t_environ *e)
{
	t_variable *var;

	var = ms_getvar(key, e);
	if (var == NULL || var->key == NULL || strncmp(var->key, key, ms_strchrlen(key, '=')))
		return ; //can not unset if not exist
	//TODO: remember when unsetting to update a possible next seeded match.
	if (var->size)
		free(var->key);
	size_t index = (&var[0] - &e->vars[0]) / sizeof(t_variable *);
	printf("[INFO] ms_unsetvar(): index=%zu\n", index);
	memset(var, '\0', sizeof(t_variable));
}
