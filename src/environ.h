#ifndef ENVIRON_H
# define ENVIRON_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h> 

// https://www.gnu.org/software/bash/manual/html_node/Bourne-Shell-Variables.html
// Bourne shell vars are implemented.
//
// https://www.gnu.org/software/bash/manual/html_node/Bash-Variables.html
// Will stay unimplemented because what the actual fuck, there are too many and
// some are weird.
enum e_bash_envs
{
	ENV_CDPATH,
	ENV_HOME,
	ENV_IFS,
	ENV_MAIL,
	ENV_MAILPATH,
	ENV_OPTARG,
	ENV_OPTIND,
	ENV_PATH,
	ENV_PS1,
	ENV_PS2,
	ENV_BOURNE_COUNT,
	ENV__,
	ENV_BASH,
	ENV_BASHOPTS,
	ENV_BASHPID,
	ENV_BASH_ALIASES,
	ENV_BASH_ARGC,
	ENV_BASH_ARGV,
	ENV_BASH_ARGV0,
	ENV_BASH_CMDS,
	ENV_BASH_COMMAND,
	ENV_BASH_COMPAT,
	ENV_BASH_ENV,
	ENV_BASH_EXECUTION_STRING,
	ENV_BASH_LINENO,
	ENV_BASH_LOADABLES_PATH,
	ENV_BASH_REMATCH,
	ENV_BASH_SOURCE,
	ENV_BASH_SUBSHELL,
	ENV_BASH_VERSINFO,
	ENV_BASH_VERSION,
	ENV_BASH_XTRACEFD,
	ENV_CHILD_MAX,
	ENV_COLUMNS,
	ENV_COMP_CWORD,
	ENV_COMP_LINE,
	ENV_COMP_POINT,
	ENV_COMP_TYPE,
	ENV_COMP_KEY,
	ENV_COMP_WORDBREAKS,
	ENV_COMP_WORD,
	ENV_COMPREPLY,
	ENV_COPROC,
	ENV_DIRSTACK,
	ENV_EMACS,
	ENV_ENV,
	ENV_EPOCHREALTIME,
	ENV_EPOCHSECONDS,
	ENV_EUID,
	ENV_EXECIGNORE,
	ENV_FCEDIT,
	ENV_FIGNORE,
	ENV_FUNCNEST,
	ENV_GLOBIGNORE,
	ENV_GROUPS,
	ENV_histchars,
	ENV_HISTCMD,
	ENV_HISTCONTROL,
	ENV_HISTFILE,
	ENV_HISTFILESIZE,
	ENV_HISTIGNORE,
	ENV_HISTSIZE,
	ENV_HISTTIMEFORMAT,
	ENV_HOSTFILE,
	ENV_HOSTNAME,
	ENV_HOSTTYPE,
	ENV_IGNOREEOF,
	ENV_INPUTRC,
	ENV_INSIDE_EMACS,
	ENV_LANG,
	ENV_LC_ALL,
	ENV_LC_COLLATE,
	ENV_LC_CTYPE,
	ENV_LC_MESSAGES,
	ENV_LC_NUMERIC,
	ENV_LC_TIME,
	ENV_LINENO,
	ENV_LINES,
	ENV_MACHTYPE,
	ENV_MAILCHECK,
	ENV_MAPFILE,
	ENV_OLDPWD,
	ENV_OPTERR,
	ENV_OSTYPE,
	ENV_PIPESTATUS,
	ENV_POSIXLY_CORRECT,
	ENV_PPID,
	ENV_PROMPT_COMMAND,
	ENV_PROMPT_DIRTRIM,
	ENV_PS0,
	ENV_PS3,
	ENV_PS4,
	ENV_PWD,
	ENV_RANDOM,
	ENV_READLINE_ARGUMENT,
	ENV_READLINE_LINE,
	ENV_READLINE_MARK,
	ENV_READLINE_POINT,
	ENV_REPLY,
	ENV_SECONDS,
	ENV_SHELL,
	ENV_SHELLOPTS,
	ENV_SHLVL,
	ENV_SRANDOM,
	ENV_TIMEFORMAT,
	ENV_TMOUT,
	ENV_TMPDIR,
	ENV_UID,
	ENV_COUNT, //last
};

//start size of (or multiplicate of) env->envp.
#ifndef MS_ENVSIZE
#define MS_ENVSIZE 64
#endif

//how much bigger the (hashed) variable array is than the actual amount of variables.
#ifndef MS_HASH_MULT
#define MS_HASH_MULT 4
#endif

//the amount of possible duplicate hash hits until different seed.
#ifndef MS_HASH_THRESHOLD
#define MS_HASH_THRESHOLD 8
#endif

enum e_ms_env_type
{
	ENV_TYPE_STR = 0, //the default and must be NULL terminated.
	ENV_TYPE_CUSTOM,
	ENV_TYPE_SHORT,
	ENV_TYPE_USHORT,
	ENV_TYPE_INT,
	ENV_TYPE_UINT,
	ENV_TYPE_LONG,
	ENV_TYPE_ULONG,
	ENV_TYPE_LONGLONG,
	ENV_TYPE_ULONGLONG,
	ENV_TYPE_FLOAT,
	ENV_TYPE_DOUBLE,
	ENV_TYPE_LONGDOUBLE,
	ENV_TYPE_COUNT, //last
};

typedef union u_ms_env_value
{
	void               *data; //Any size struct array. DECIDE: gnu's mpfr/gmp?
	char               *str;  //most common. 0 terminated instead of width/len
	//unsigned char      *uchars; //unneeded imo
	short              *shorts;
	unsigned short     *ushorts;
	int                *ints;
	unsigned int       *uints;
	long               *longs;
	unsigned long      *ulongs;
	long long          *llongs;
	unsigned long long *ullongs;
	float              *floats;
	double             *doubles;
	long double        *ldoubles;
////DECIDE: env variable can't be a single number? Solution: use array with length one :p
////DECIDE: Not sure to use <stdint.h>. explicit size integers or the official C ones above.
//	uint8_t  *uint8s;
//	int8_t   *int8s;
//	uint16_t *uint16s;
//	int16_t  *int16s;
//	uint32_t *uint32s;
//	int32_t  *int32s;
//	uint64_t *uint64s;
//	int64_t  *int64s; 
} u_envp;

enum e_variable_about
{
	ENV_IS_LENGTH = 0,
	ENV_IS_WIDTH,
	ENV_IS_LOCAL,
	ENV_OF_ARRAY_TYPE,
	ENV_IS_READONLY,
};

//Array is in form:  [key]=[length][width][value]  where:
//  - `length` and `width` are 32bit unsigned integers.
//  - key and the `=` sign are characters.
//  - value is `length` times `width` number of bytes.
//bool readonly: see man export(1) and for details: bash(1) `/^ *readonly`
//bool local   : see man export(1) and bash(1) `/^PARAMETERS`
typedef struct t_variable_about //4 chars + 1 int = 8 bytes;
{
	bool           local;     // Whether variable is local or passed to execve(envp).
 	unsigned char  typeofvar; // see e_ms_env_type 0 means string, otherwise variable is in form:
							    // [key]=[length][width][value]. See u_ms_env_value
	bool           readonly;  // Variable is unwritable after setting. Unless overwrite is non-zero.
	unsigned char  width;     // Width of val. 0 means value is a NULL terminated str.
							    // Maximum of 256 bytes for per array item.
	unsigned int   length;    // Set to the length of [value] (array).
} t_variable_about;

//8 bytes * 4 = 32 bytes. Very nice.
typedef struct t_variable
{
//LEGACY:	struct t_variable    *next; //possible duplicate hit by hash of `key`.
	char                 *key;  //always the start that got malloc'ed
	union u_ms_env_value value; //string or array. Depends on if `detail.width` > 0
	size_t               size;  //numb of bytes malloced of `key`.
							    //equal to zero and `key` exists means outside scope.
	t_variable_about     abt;   // .length .width .local .readonly .typeofvar
} t_variable;

//OVERVIEW:
// local variables           | exported variables
// - bashrc envs             | - past variables from <stdlib.h>(extern char **environ)
// - user def without export | - via input or .rcfile with keyword: 'export'.
// - shell envs              | - via CLI at start up.
typedef struct t_environ
{
	t_variable *vars; //MS_HASH_MULT times as big as ARRAY_LEN(ms_round_to_pow_2(environ)).
	size_t     size;
	uint32_t   seed; //Used for calling ms_hash(). Initially set to 0.
					 //Can increment when MS_HASH_THRESHOLD is surpassed, which
					 //happens when an `environment variable id string hash` is the same as
					 //MS_HASH_THRESHOLD other environment variables.
} t_environ; //DECIDE: global environment or stack.

//	src/env-init-resize-setenviron-setbourne.c :
bool ms_envinit(t_environ *e);
bool ms_envresize(t_environ *e, size_t newsize);
//DECIDE: bool ms_setenviron(t_environ *e); 
//DECIDE: bool ms_setbourne(t_environ *e); 

//NOTE: difference between `env` and `var`:
//`env` are mimicking prototypes of <stdlib.h>
//`var` are this shell's internal functions.
//	src/env-get-set-unset.c :
char *ms_getenv(char *key, t_environ *e);
bool ms_setenv(char *key, char *value, int overwrite, t_variable_about f, t_environ *e); 
void ms_unsetenv(char *key, t_environ *e);

//	src/var-get-set-unset-resize-append-prepend.c :
t_variable *ms_getvar(char *key, t_environ *e);
bool       ms_setvar(char *key, size_t keysize, t_variable_about f, t_environ *e); 
bool       ms_appendvar(char *key, char *keysize, t_variable_about f, t_environ *e); 
bool       ms_prependvar(char *key, char *keysize, t_variable_about f, t_environ *e); 
void       ms_unsetvar(char *key, t_environ *e);

#endif //#ifndef ENVIRON_H
