//these functions you generally dont call.
#include "environ.h"
#include "hash.h"
#include "utils.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

//DECIDE: call both setenv(3) and ms_setvar().
//replacement of setenv(3).
//overwrite = 0 won't touch the environment if var exists. Otherwise overwrite.
//abt       = {.local, .typeofvar, .readonly, .width, .length}
//returns boolean true on success, and false on malloc error or if NULL was passed as key.
bool	ms_setenv(char *key, char *value, int overwrite, t_variable_about abt, t_environ *e)
{
	size_t newlen;
	size_t idlen;
	char   *combined;
 
	if (key == NULL)
		return (ms_log("[ERROR] ms_setenv() got NULL pointer"), false);
	if (value	== NULL)
		return (ms_unsetvar(key, e), true);
	idlen    = ms_strchrlen(key, '=');
	newlen   = strlen(value);
	combined = malloc(idlen + newlen + 2);
	if (combined == NULL)
		return (false);
	memcpy(combined, key, idlen);
	combined[idlen] = '=';
	memcpy(combined + idlen + 1, value, newlen);
	combined[idlen + newlen + 1] = 0;
	return (ms_setvar(combined, idlen + newlen, (t_variable_about){.local = true}, e));
}

//also calls unsetenv(3) instead of just ms_unsetvar().
//replacement of unsetenv(3).
void	ms_unsetenv(char *key, t_environ *e)
{
	//TODO: unset value instead of the full env because there is
	//a big chance user will set it in the future.
	ms_unsetvar(key, e);
}
