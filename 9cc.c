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

typedef struct 	Token
{
	TokenKind 	kind;
	struct		Token *next;
	int 		val;
	char		*str;
}				Token;

typedef enum
{
	ND_ADD,
	ND_SUB,
	ND_MUL,
	ND_DIV,
	ND_NUM,
}	NodeKind;

typedef struct	Node
{
	NodeKind	kind;
	struct Node	*lhs;
	struct Node	*rhs;
	int			val;
}				Node;

Token 	*token;
char	*user_input;
Node	*expr(void);

void	error_at(char *loc, char* fmt, ...)
{
	va_list ap;
	int		pos;

	va_start(ap, fmt);
	pos = loc - user_input;
	fprintf(stderr, "%s\n", user_input);
	fprintf(stderr, "%*s", pos, "");
	fprintf(stderr, "^ ");
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
		error_at(token->str, "Not '%c'", op);
	token = token->next;
}

int		expect_number(void)
{
	int	val;

	if (token->kind != TK_NUM)
		error_at(token->str, "Not a number");
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
		if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')')
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
		error_at(p, "Cannot tokenize");
	}
	new_token(TK_EOF, cur, p);
	return head.next;
}

Node	*new_node(NodeKind kind, Node *lhs, Node *rhs)
{
	Node *node;
	
	node = calloc(1, sizeof(Node));
	node->kind = kind;
	node->lhs = lhs;
	node->rhs = rhs;
	return (node);
}

Node	*new_node_num(int val)
{
	Node *node;
	
	node = calloc(1, sizeof(Node));
	node->kind = ND_NUM;
	node->val = val;
	return (node);
}

Node	*primary(void)
{
	if (consume('('))
	{
		Node *node = expr();
		expect(')');
		return (node);
	}
	return new_node_num(expect_number());
}

Node	*unary(void)
{
	if (consume('+'))
		return (primary());
	if (consume('-'))
		return (new_node(ND_SUB, new_node_num(0), primary()));
	return (primary());
}

Node	*mul(void)
{
	Node *node;

	node = unary();
	while (1)
	{
		if (consume('*'))
			node = new_node(ND_MUL, node, unary());
		else if (consume('/'))
			node = new_node(ND_DIV, node, unary());
		else
			return (node);
	}
}

Node	*expr(void)
{
	Node *node;

	node = mul();
	while (1)
	{
		if (consume('+'))
			node = new_node(ND_ADD, node, mul());
		else if (consume('-'))
			node = new_node(ND_SUB, node, mul());
		else
			return (node);
	}
}

void	gen(Node *node)
{
	if (node->kind == ND_NUM)
	{
		printf("	push %d\n", node->val);
		return ;
	}
	gen(node->lhs);
	gen(node->rhs);
	printf("	pop rdi\n");
	printf("	pop rax\n");
	if (node->kind == ND_ADD)
		printf("	add rax, rdi\n");
	else if (node->kind == ND_SUB)
		printf("	sub rax, rdi\n");
	else if (node->kind == ND_MUL)
		printf("	imul rax, rdi\n");
	else
		{
			printf("	cqo\n");
			printf("	idiv rdi\n");
		}
	printf("	push rax\n");
}

int		main(int argc, char **argv)
{
	Node *node;

	if (argc != 2)
	{
		fprintf(stderr, "One argument is required.\n");
		return (1);
	}
	user_input = argv[1];
	token = tokenize(user_input);
	node = expr();
	printf(".intel_syntax noprefix\n");
	printf(".global main \n");
	printf("main:\n");
	gen(node);
	printf("	pop rax\n");
	printf("	ret\n");
	return (0);
}