#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
	TK_RESERVED,
	TK_NUM,
	TK_EOF,
} TokenKind;

typedef struct Token
{
	TokenKind kind;
	struct Token *next;
	int val;
	char *str;
}	Token;

Token *token;

void	error(char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit (1);
}

bool	consume(char op)
{
	if (token->kind != TK_RESERVED || token->str[0] != op)
		return false;
	token = token->next;
	return true;
}

void	expect(char op)
{
	if (token->kind != TK_RESERVED || token->str[0] != op)
		error("Not '%c'", op);
	token = token->next;
}

int		expect_number(void)
{
	int	val;

	if (token->kind != TK_NUM)
		error("Not number.");
	val = token->val;
	token = token->next;
	return (val);
}

bool	at_eof(void)
{
	return (token->kind == TK_EOF);
}

Token	*new_token(TokenKind kind, Token *cur, char *str)
{
	Token *tok;
	
	tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	cur->next = tok;
	return (tok);
}

Token	*tokenize(char *p)
{
	Token head;
	Token *cur;

	head.next = NULL;
	cur = &head;
	while (*p)
	{
		if (isspace(*p))
		{
			p++;
			continue;
		}
		if (*p == '+' || *p == '-')
		{
			cur = new_token(TK_RESERVED, cur, p++);
			continue;
		}
		if (isdigit(*p))
		{
			cur = new_token(TK_NUM, cur, p);
			cur->val = strtol(p, &p, 10);
			continue;
		}
		error("Cannot tokenize.");
	}
	new_token(TK_EOF, cur, p);
	return head.next;
}

int		main(int argc, char **argv)
{
	char *p;

	if (argc != 2)
	{
		fprintf(stderr, "One argument is required.\n");
		return (1);
	}
	token = tokenize(argv[1]);
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");
	printf("	mov rax, %d\n", expect_number());
	while (!at_eof())
	{
		if (consume('+'))
		{
			printf("	add rax, %d\n", expect_number());
			continue;
		}
		expect('-');
		printf("	sub rax, %d\n", expect_number());
	}
	printf("	ret\n");
	return (0);
}