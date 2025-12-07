#ifndef MS_PROMPT_H
# define MS_PROMPT_H

#ifndef MS_PROMPT_SIZE
#define MS_PROMPT_SIZE 1024
#endif

#include "minishell.h"
#include <stdbool.h>

typedef struct t_prompt
{
	t_variable *var;
	char       *str;
	size_t     size;
	size_t     len;
} t_prompt;

typedef struct t_shell t_shell; //why??
bool ms_promptinit(t_shell *sh);
bool ms_prompt_update(t_info *info, t_prompt *p, char *new);

#endif //#ifndef MS_PROMPT_H
