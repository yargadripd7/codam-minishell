//unix:
#include <unistd.h>
//libc:
#include <stdbool.h> //true false bool
#include <stdlib.h> //malloc() free()
#include <string.h> //memset()
#include <stdio.h> //somehow needed for history.h
//readline:
#include <readline/history.h> //add_history()
//local:
#include "minishell.h" //t_shell, enum
#include "argv.h" //enum
#include "environ.h" //ms_envinit()
#include "prompt.h" //ms_promptinit()
#include "getline.h" //ft_getline()
#include "lexer.h" //ms_lexerinit()
#include "parser.h" //ms_parserinit()
#include "exec.h" //ms_execinit()
#include "utils.h" //ms_free()

static void	ms_free(t_shell *sh)
{
	size_t	i;

	if (sh->ps1.size > 0)
		free(sh->ps1.str);
	if (sh->ps2.size > 0)
		free(sh->ps2.str);
	if (sh->env.size > 0)
	{
		i = -1;
		while (++i < sh->env.size)
			if (sh->env.vars[i].size > 0)
			{
				ms_log("[INFO] freeing: %zu: ", i);
				ms_log("'%s'\n", sh->env.vars[i].key);
				free(sh->env.vars[i].key);
			}
		if (sh->env.size > 0)
			free(sh->env.vars);
	}
	if (sh->cmdsize > 0)
		free(sh->cmdstr);
	if (sh->lexer.size > 0)
		free(sh->lexer.tokens);
}

int	ms_display_prompt_and_read_cmd(t_shell *sh)
{
	static char       *check = NULL;
	static t_variable *var;
	
	var = ms_getvar("PS1", &sh->env);
	if (ms_prompt_update(&sh->info, &sh->ps1, var->value.str)
		|| write(STDOUT_FILENO, sh->ps1.str, sh->ps1.len) < 0)
		return (EXIT_FAILURE);
	return (ft_getline(&sh->cmdstr, &sh->cmdsize, STDIN));
}

int	main(int argc, char *argv[])
{
	t_shell		sh;
	int			ret;

	bzero(&sh, sizeof(t_shell));
	sh.info.cmdlimit = (uint32_t)-1; //unlimited
	if (!ms_envinit(&sh.env)
		|| !ms_parse_argv(&sh.info, &sh.env, argc, argv)
		|| !ms_promptinit(&sh)
		|| !ms_lexerinit(&sh.lexer)
		|| !ms_parserinit(&sh.parser)
		|| !ms_execinit(&sh))
		return (ms_free(&sh), EXIT_FAILURE);
	while (sh.info.shell_should_quit == false
	   && (sh.info.cmdlimit == (size_t)-1 || sh.info.cmdcount++ < sh.info.cmdlimit))
	{
		ret = ms_display_prompt_and_read_cmd(&sh);
		if (ret == 0)
			break;
		if (ret < 0
			|| ms_lexer(&sh.lexer, sh.cmdstr) == false
			|| ms_parse(&sh.parser, &sh.lexer) == false
			|| ms_exec(&sh.parser, &sh.info) == false)
			return (ms_free(&sh), EXIT_FAILURE);
		add_history(sh.cmdstr);
		for (size_t i = 0; i < sh.lexer.count; ++i)
			printf("[INFO] token='%.*s'\n",
				(int)(sh.lexer.tokens[i].end - sh.lexer.tokens[i].begin),
				sh.cmdstr + sh.lexer.tokens[i].begin);
	}
	printf("Goodbye!\n");
	ms_free(&sh);
	return (EXIT_SUCCESS);
}

//difficult problems:
// echo hello && ((echo hi; echo) < test.txt)
// ((echo hi; cat) < test.txt) > cat
// ((echo hi; cat) < test.txt) ; cat
// Signals:
//	signal(SIGQUIT, ms_exit);
//	signal(SIGINT, ms_exit);
//	if (EOF CTRL+d) ms_exit();
