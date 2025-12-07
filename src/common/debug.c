#include "minishell.h"
#include "environ.h"
#include "cstr_colors.h"
#include "utils.h"

#include <assert.h>
#include <stdlib.h> //free()
#include <stdio.h>	//printf()
#include <stdarg.h>
#include <unistd.h> //STDOUT_FILENO
#include <string.h>
#include <sys/ioctl.h> //ioctl()

//DECIDE: implement loglevels: //DEBUG //INFO //WARN //ERROR //FATAL //TRACE
// probably just: [DEBUG or INFO and WARN, ERROR]
void ms_log(char *fmt, ...)
{
	va_list	ls;

	va_start(ls, fmt);
	vfprintf(stderr, fmt, ls);
	va_end(ls);
	fflush(stderr);
}

void	ms_printenv(t_variable *var, t_environ *e)
{
	size_t            i;
	static const char *bools[2] = {"false", "true"};
	static const char *vartypes[] = {
		"str", "custom", "short", "unsigned short", "int", "unsigned int", "long", "unsigned long",
		"long long", "unsigned long long", "float", "double", "long double"
	};
	assert(sizeof(vartypes)/sizeof(*vartypes) == ENV_TYPE_COUNT);

	printf("[INFO] var %zu = {.key='%.*s', ", var - e->vars,
	       (int)ms_strchrlen(var->key, '='), var->key);
	if (var->abt.typeofvar == ENV_TYPE_STR)
		printf(".val='%s', size=%zu, .abt=[.local=%s, .typeofvar=<%s>, "
		       "readonly=%s, width=%u, length=%u]}\n", var->value.str,
		       var->size, bools[var->abt.local], vartypes[var->abt.typeofvar],
		       bools[var->abt.readonly], var->abt.width, var->abt.length
			   );
	else 
	{
		printf(".val=[<array items>");
		//TODO: print an array.
		i = -1;
		while (++i < var->abt.length) ;
		printf("]");
	}
	printf("}\n");
}

void	ms_printenvs(t_environ *e)
{
	size_t	i;

	i = 0;
	while (i < e->size)
	{
		if (e->vars[i].key != NULL)
			ms_printenv(e->vars + i, e);
		i++;
	}
}
void ms_perror(t_shell *sh, int inputidx, char *msg)
{
	struct winsize	w;
	int				column;

	if ((ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) != -1)
		&& inputidx >= 0 && sh->cmdstr != NULL)
	{
		column = (inputidx + strlen(sh->ps1.str));
		fprintf(stderr, "%.*s", w.ws_col, sh->cmdstr + inputidx - (column % w.ws_col));
		fprintf(stderr, "%*c\n", column % w.ws_col, '^');
		fprintf(stderr, "%*c\n", column % w.ws_col, '|');
	}
	fprintf(stderr, MS_RED"Error: "MS_RESET"%s\n", msg);
}

void	ms_error_weird_char(int c)
{
	const char	*chr_description[32] = {
					NULL,                        "SOH (start of heading)",
					"STX (start of text)",       "ETX (end of text)",
					"EOT (end of transmission)", "ENQ (enquiry)",
					"ACK (acknowledge)",         "BEL '\\a' (bell)",
					"BS  '\\b' (backspace)",     "HT  '\\t' (horizontal tab)",
					"LF  '\\n' (new line)",      "VT  '\\v' (vertical tab)",
					"FF  '\\f' (form feed)",     "CR  '\\r' (carriage ret)",
					"SO  (shift out)",           "SI  (shift in)",
					"DLE (data link escape)",    "DC1 (device control 1)",
					"DC2 (device control 2)",    "DC3 (device control 3)",
					"DC4 (device control 4)",    "NAK (negative ack.)",
					"SYN (synchronous idle)",    "ETB (end of trans. blk)",
					"CAN (cancel)",              "EM  (end of medium)",
					"SUB (substitute)",          "ESC (escape)",
					"FS  (file separator)",      "GS  (group separator)",
					"RS  (record separator)",    "US  (unit separator)",
	};

	if (c > 0 && c < ' ')
		printf("Error: encountered character: %#X:%s\n",
				c, chr_description[c]);
}

