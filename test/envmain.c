#include <stdio.h>
#include "environ.h"

////	src/env-get-set-unset.c <stdlib.h>'s functions:
//bool ms_setenv(char *id, char *value, int overwrite, t_environ *e);
//char *ms_getenv(char *id, t_environ *e);
//void ms_unsetenv(char *id, t_environ *e); //same as ms_unsetenv_slot()
////	src/envslot-get-set-unset-append-prepend.c:
//t_variable *ms_getenv_slot(char *id, t_environ *e);
//bool       ms_setenv_slot(char *ptr, size_t ptrsize, t_ms_env_detail d, t_environ *e);
//void       ms_unsetenv_slot(char *id, t_environ *e);
//bool       ms_appendenv_slot(char *id, char *val, t_ms_env_detail d, t_environ *e);
//bool       ms_prependenv_slot(char *id, char *val, t_ms_env_detail d, t_environ *e);
int main(void)
{
	t_variable	varval[1] = {0};
	t_environ	envval = {.vars = varval};
	t_environ	*e = &envval;
	t_variable	*var = e->vars;
	char *str = "HI=123";
	size_t strsize = sizeof(*str);
	t_variable_about abt = {0};

	printf("[INFO] setting env % 3lu from: [key='%s', size=%zu, "
	       "abt={l=%u, w=%u, loc=%d, readonly=%d, arraytype=%d}]\n% 33s: "
		   "[key='%s', size=%zu, abt={l=%u, w=%u, loc=%d, readonly=%d}]\n",
		   var - e->vars,  var->key,  var->size,  var->abt.length,  var->abt.width,
		   var->abt.local,  var->abt.readonly,  var->abt.arraytype, "to",
		   str,  strsize,  abt.length,  abt.width,  abt.local,  abt.readonly,
		   abt.arraytype);
	printf("[environment variables tester]\n");

	fflush(stdout);
//	t_environ	env;
//	env = {0};

	//ms_setenv_slot("Var1=123", );
	return (0);
}
