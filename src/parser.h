#ifndef MS_PARSER_H
#define MS_PARSER_H

#include "lexer.h"
#include <stdbool.h>

typedef struct t_parser
{
	int	unimplemented;
} t_parser;

bool	ms_parserinit(t_parser *parser);
bool	ms_parse(t_parser *parser, t_lexer *lexer);
void	ms_parse_print(t_parser *parser);

#endif //#ifndef MS_PARSER_H
