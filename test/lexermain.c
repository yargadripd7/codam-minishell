#include "lexer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const char *tokenstrs[] = {
	[TOKEN_UNDEFINED]        = "TOKEN_UNDEFINED",
	[TOKEN_WORD]             = "TOKEN_WORD",
	[TOKEN_NL]               = "TOKEN_NL",
	[TOKEN_AND]              = "TOKEN_AND",
	[TOKEN_OR]               = "TOKEN_OR",
	[TOKEN_END]              = "TOKEN_END",
	[TOKEN_BG]               = "TOKEN_BG",
	[TOKEN_PIPE_OUT]         = "TOKEN_PIPE_OUT",
	[TOKEN_PIPE_OUT_AND_ERR] = "TOKEN_PIPE_OUT_AND_ERR",
	[TOKEN_FILE]             = "TOKEN_FILE",
	[TOKEN_SQUOTE]           = "TOKEN_SQUOTE",
	[TOKEN_DQUOTE]           = "TOKEN_DQUOTE",
	[TOKEN_CQUOTE]           = "TOKEN_CQUOTE",
	[TOKEN_PARAM_ASSIGN]     = "TOKEN_PARAM_ASSIGN",
	[TOKEN_PARAM_APPEND]     = "TOKEN_PARAM_APPEND",
	[TOKEN_PARAM_EXPAN]      = "TOKEN_PARAM_EXPAN",
	[TOKEN_REDIRECT_IN]      = "TOKEN_REDIRECT_IN",
	[TOKEN_REDIRECT_OUT]     = "TOKEN_REDIRECT_OUT",
	[TOKEN_HERE_DOCUMENT]    = "TOKEN_HERE_DOCUMENT",
	[TOKEN_REDIRECT_APPEND]  = "TOKEN_REDIRECT_APPEND",
	[TOKEN_EXITCODE]         = "TOKEN_EXITCODE",
	[TOKEN_PID]              = "TOKEN_PID",
	[TOKEN_TILDE]            = "TOKEN_TILDE",
	[TOKEN_PATTERN_WILD]     = "TOKEN_PATTERN_WILD",
	[TOKEN_PATTERN_CHAR]     = "TOKEN_PATTERN_CHAR",
	[TOKEN_PATTERN_OPEN]     = "TOKEN_PATTERN_OPEN",
	[TOKEN_ARITHMETIC]       = "TOKEN_ARITHMETIC",
};
//static_assert(sizeof(tokenstrs)/sizeof(*tokenstrs) == TOKEN_COUNT);

typedef struct t_lexer_test
{
	const char	*string;
	t_token		tokens[4];
} t_lexer_test;

t_lexer_test tests[] = {
	{
		.string = "var=123\n",
		.tokens = {
			{
				.id = TOKEN_PARAM_ASSIGN,
				.begin = 0,
				.end = 3,
			}, {
				.id = TOKEN_WORD,
				.begin = 4,
				.end = 6,
			}, {
				.id = TOKEN_NL,
				.begin = 7,
				.end = 7,
			}
		} 
	}, {
		.string = "hello world\n",
		.tokens = {
			{
				.id = TOKEN_UNDEFINED,
				.begin = 0,
				.end = 4,
			}, {
				.id = TOKEN_UNDEFINED,
				.begin = 6,
				.end = 10,
			}, {
				.id = TOKEN_NL,
				.begin = 11,
				.end = 11,
			}
		} 
	}
};

/*
./lexer 'echo \'Hello World\' '
./lexer 'HELLO=WORLD'
*/
int	main(int argc, const char *argv[])
{
	t_lexer	l;

	bzero(&l, sizeof(t_lexer));
	if (argc != 2) {
		for (size_t i = 0; i < sizeof(tests)/sizeof(*tests); ++i)
		{
			if (ms_lexer(&l, (char *)tests[i].string) == false)
				return (false);
			free(l.tokens);
			bzero(&l, sizeof(t_lexer));
		}
		return (0);
	}
	if (ms_lexer(&l, (char *)argv[1]) == false) return (1);
	for (size_t i = 0; i < l.size; ++i)
	{
		int		id = l.tokens[i].id;
		size_t	begin = l.tokens[i].begin;
		size_t	end = l.tokens[i].end;
		printf("'%.*s' = %s\n", (int)(end - begin), argv[1] + begin, tokenstrs[id]);
	}
	return (0);
}

