#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
	TK_RESERVED,
	TK_IDENT,
	TK_NUM,
	TK_EOF,
} TokenKind;

typedef struct 	Token
{
	TokenKind 	kind;
	struct		Token *next;
	int 		val;
	char		*str;
	int			len;
}				Token;

typedef enum
{
	ND_ADD,
	ND_SUB,
	ND_MUL,
	ND_DIV,
	ND_NUM,
	ND_EQU,
	ND_NEQ,
	ND_SMA,
	ND_SEQ,
	ND_GRE,
	ND_GEQ,
	ND_ASSIGN,
	ND_LVAR,
}	NodeKind;

typedef struct	Node
{
	NodeKind	kind;
	struct Node	*lhs;
	struct Node	*rhs;
	int			val;
	int			offset;
}				Node;

extern char		*user_input;
extern Token	*token;
extern Node		*code[100];

Token	*tokenize(char *p);
void	program(void);
Node	*stmt(void);
Node	*expr(void);
Node	*assign(void);
Node	*equality(void);
Node	*relational(void);
Node	*add(void);
Node	*mul(void);
Node	*unary(void);
Node	*primary(void);
void	gen(Node *node);
void	error_at(char *loc, char* fmt, ...);
void	error(char *fmt, ...);