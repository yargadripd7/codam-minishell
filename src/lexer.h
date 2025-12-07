#ifndef MS_LEXER_H
# define MS_LEXER_H

/* NOTES:
word: starts with [A-Za-z_] and could continue with [A-Za-z_0-9].
special tokens:
	CMD_EXPLICIT = "./path" or "/path"
	CMD          = "path" 
	CQUOTE       = "$'"
	PARAM_ASSIGN = "word="
	PARAM_EXPAN  = "$word"
	FILE_EXPAN   = "*"
BI_ = builtin
CO_ = control operator = \n && || ; & | |& ( )
*/

//start buffer size of token array.
#ifndef MS_TOKEN_SIZE
# define MS_TOKEN_SIZE 64
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MS_METACHARS "|&;()<>"

//control tokens: [\n, &&, ||, ;, &, |, |&]
//redirect tokens: [<, >, <<, >>]
enum e_tokenid
{
	TOKEN_UNDEFINED = 0,    //edge-case?
	TOKEN_WORD,             //word (string or file, obliges IFS env)
	TOKEN_NL,               //is `\n`
	TOKEN_AND,              //is `&&`
	TOKEN_OR,               //is `||`
	TOKEN_END,              //is `;`
	TOKEN_BG,               //is `&`
	TOKEN_PIPE_OUT,         //is `|`
	TOKEN_PIPE_OUT_AND_ERR, //is `|&`
	TOKEN_FILE,             //starts with `./` or `/`
	TOKEN_SQUOTE,           //starts with `'` ends with `'`
	TOKEN_DQUOTE,           //starts with `"` ends with `"`
	TOKEN_CQUOTE,           //starts with `$'` ends with `'`
	TOKEN_PARAM_ASSIGN,     //is `word=`
	TOKEN_PARAM_APPEND,     //is `word+=`
	TOKEN_PARAM_EXPAN,      //is `$word`
	TOKEN_REDIRECT_IN,      //is `<`
	TOKEN_REDIRECT_OUT,     //is `>`
	TOKEN_HERE_DOCUMENT,    //is `<<`
	TOKEN_REDIRECT_APPEND,  //is `>>`
	TOKEN_EXITCODE,         //is `$?`
	TOKEN_PID,              //is `$$`
	TOKEN_TILDE,            //is `~`
	TOKEN_PATTERN_WILD,     //is `*`
	TOKEN_PATTERN_CHAR,     //is `?`
	TOKEN_PATTERN_OPEN,     //is `?`
	TOKEN_ARITHMETIC,       //is `$[`
	//TOKEN_HIST_LAST,        //is `!!`
	//TOKEN_HIST_MINUS,       //is `!-`
	//TOKEN_HIST_STARTS_WITH, //is ``
	//TOKEN_HIST_ENDS_OR_CONTAINS, //starts with `!
	//TOKEN_HIST_REPLACE,     //"
	TOKEN_COUNT, //must be last
};

typedef struct t_token
{
	int8_t	id;
	size_t	begin;
	size_t	end;
} t_token;

//table = 128 * 128 for the first 2 characters.
typedef struct t_lexer
{
	t_token	*tokens;
	size_t	count;
	size_t	size;
	uint8_t	table[128 * 128];
	char	*ifs;
} t_lexer;

bool	ms_lexerinit(t_lexer *lexer);
bool	ms_lexer(t_lexer *l, char *input);

#endif //#ifndef MS_LEXER_H
