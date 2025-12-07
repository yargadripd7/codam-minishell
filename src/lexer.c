/////////////////////////////////////////////////////////////////
/// Lexer.                  ///
/////////////////////////////////////////////////////////////////

#include <stdlib.h> //malloc()
#include <string.h> //memset() bzero()
#include "lexer.h"
#include "utils.h" //ms_strchrlen()

static void	set_token(char *str, size_t *i, t_lexer *l, size_t current)
{
	if (l->table[str[*i] * 128 + str[*i + 1]] != 0)
	{
		REACHED("%*s", 2, str);
		l->tokens[current].id = l->table[str[*i] * 128 + str[*i + 1]];
		l->tokens[current].begin = (*i)++;
		l->tokens[current].end = (*i)++;
	}
	else if (l->table[str[*i] * 128] != 0)
	{
		REACHED("%*s", 1, str);
		l->tokens[current].id = l->table[str[*i] * 128];
		l->tokens[current].begin = *i;
		l->tokens[current].end = (*i)++;
	}
	else if (ms_strchrlen(str + *i, '=') != strlen(str + *i)) //assignment
	{
		REACHED("%*s", (int)ms_strchrlen(str + *i, '='), str);
		l->tokens[current].id = TOKEN_PARAM_ASSIGN;
		l->tokens[current].begin = *i;
		l->tokens[current].end = (*i += ms_strchrlen(str + *i, '='));
	}
	else if (current != 0 && l->tokens[current - 1].id == TOKEN_PARAM_ASSIGN)
	{
		l->tokens[current].id = TOKEN_WORD;
		l->tokens[current].begin = *i;
		while (str[*i] && !strchr(l->ifs, str[*i]) && !strchr(MS_METACHARS, str[*i]))
			++(*i);
		REACHED("%*s", (int)(*i - l->tokens[current].begin), str + l->tokens[current].begin);
		l->tokens[current].end = *i;
		while (str[*i] && strchr(l->ifs, str[*i]))
			++(*i);
	}
	else
	{
		//printf("str=%s\n", str);
		//fflush(stdout);
		while (str[*i] && l->ifs && strchr(l->ifs, str[*i]))
			++(*i);
		l->tokens[current].id = TOKEN_FILE;
		l->tokens[current].begin = *i;
		while (str[*i] && l->ifs && !strchr(l->ifs, str[*i]) && !strchr(MS_METACHARS, str[*i]))
			++(*i);
		REACHED("%*s", (int)(*i - l->tokens[current].begin), str + l->tokens[current].begin);
		fflush(stdout);
		l->tokens[current].end = *i;
		while (str[*i] && l->ifs && strchr(l->ifs, str[*i]))
			++(*i);
	}
}

bool	ms_lexer(t_lexer *l, char *input)
{
	size_t	current;
	size_t	i;

	current = 0;
	i = 0;
	while (input[i] != '\0')
	{
		if (current == l->size && ms_expand_array((void **)&l->tokens, sizeof(t_token),
		         (void *)&l->size, l->size + MS_TOKEN_SIZE) == false)
			return (false);
		set_token(input, &i, l, current++);
	}
	return (true);
}

//	l->table['!' * 128 + '?'] = TOKEN_HIST_ENDS,  //"!?string"
//	l->table['!' * 128] = TOKEN_HIST_N,           //"!n"
//	see: https://www.gnu.org/software/bash/manual/bash.html#Event-Designators
void	ms_initlexer_bonus(t_lexer *l)
{
	l->table[';' * 128] = TOKEN_END;
	l->table['*' * 128] = TOKEN_PATTERN_WILD;
	l->table['?' * 128] = TOKEN_PATTERN_CHAR;
	l->table['[' * 128] = TOKEN_PATTERN_OPEN;
	l->table['$' * 128 + '$']  = TOKEN_PID;
	l->table['&' * 128 + '&']  = TOKEN_AND;
	l->table['|' * 128 + '|']  = TOKEN_OR;
	l->table['$' * 128 + '[']  = TOKEN_ARITHMETIC;
	l->table['$' * 128 + '\''] = TOKEN_CQUOTE;
	l->table['|' * 128 + '&']  = TOKEN_PIPE_OUT_AND_ERR;
	//l->table['!' * 128 + '!'] = TOKEN_HIST_LAST;        //"!!"
	//l->table['!' * 128 + '-'] = TOKEN_HIST_MINUS;       //"!-"
	//l->table['!' * 128]       = TOKEN_HIST_N_OR_STARTS_WITH; //"!n" or "!string"
	//l->table['!' * 128 + '?'] = TOKEN_HIST_ENDS_OR_CONTAINS; //"!?string?" or "!?string"
	//l->table['^' * 128]       = TOKEN_HIST_REPLACE;     //"^target^replacement"
}

bool	ms_lexerinit(t_lexer *l)
{
	memset(&l->table, 0, 128 * 128); //2 char token.
	l->table['\n' * 128] = TOKEN_NL;
	l->table['&'  * 128] = TOKEN_BG;
	l->table['|'  * 128] = TOKEN_PIPE_OUT;
	l->table['<'  * 128] = TOKEN_REDIRECT_IN;
	l->table['>'  * 128] = TOKEN_REDIRECT_OUT;
	l->table['/'  * 128] = TOKEN_FILE;
	l->table['\'' * 128] = TOKEN_SQUOTE; 
	l->table['"'  * 128] = TOKEN_DQUOTE;
	l->table['~'  * 128] = TOKEN_TILDE;
	l->table['<'  * 128 + '<'] = TOKEN_HERE_DOCUMENT;
	l->table['>'  * 128 + '>'] = TOKEN_REDIRECT_APPEND;
	l->table['+'  * 128 + '='] = TOKEN_PARAM_APPEND;
	l->table['$'  * 128 + '?'] = TOKEN_EXITCODE;
	l->table['.'  * 128 + '/'] = TOKEN_FILE;
	ms_initlexer_bonus(l);
	return (ms_expand_array((void **)&l->tokens, sizeof(t_token),
	                        (void *)&l->size, MS_TOKEN_SIZE));
}

