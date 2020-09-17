#include "9cc.h"

Node	*code[100];

Token	*new_token(TokenKind kind, Token *cur, char *str, int len)
{
	Token *tok;
	
	tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	tok->len = len;
	cur->next = tok;
	return (tok);
}

int		is_alnum(char c)
{
	return (('a' <= c && c <= 'z') ||
			('A' <= c && c <= 'Z') ||
			('0' <= c && c <= '9') ||
			(c == '_'));
}
Token	*tokenize(char *p)
{
	Token 	head;
	Token 	*cur;
	char	*q;

	head.next = NULL;
	cur = &head;
	while (*p)
	{
		if (isspace(*p))
		{
			p++;
			continue ;
		}
		if (memcmp(p, "==", 2) == 0 || memcmp(p, "!=", 2) == 0 || memcmp(p, "<=", 2) == 0 || memcmp(p, ">=", 2) == 0)
		{
			cur = new_token(TK_RESERVED, cur, p, 2);
			p += 2;
			continue ;
		}
		if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' || *p == '<' || *p == '>' || *p == ';' || *p == '=')
		{
			cur = new_token(TK_RESERVED, cur, p++, 1);
			continue ;
		}
		if (isdigit(*p))
		{
			cur = new_token(TK_NUM, cur, p, 0);
			q = p;
			cur->val = strtol(p, &p, 10);
			cur->len = p - q;
			continue ;
		}
		if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6]))
		{
			cur = new_token(TK_RETURN, cur, p, 6);
			p += 6;
			continue ;
		}
		if ('a' <= *p && *p <= 'z')
		{
			cur = new_token(TK_IDENT, cur, p, 0);
			q = p;
			while ('a' <= *p && *p <= 'z' )
				p++;
			cur->len = p - q;
			continue ;
		}
		error_at(p, "Cannot tokenize");
	}
	new_token(TK_EOF, cur, p, 0);
	return (head.next);
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

bool	consume(char* op)
{
	if (token->kind != TK_RESERVED ||
		strlen(op) != token->len ||
		memcmp(token->str, op, token->len))
		return (false);
	token = token->next;
	return (true);
}

bool	check_ident(void)
{
	if (token->kind != TK_IDENT)
		return (false);
	return (true);
}

bool	consume_return(char* op)
{
	if (token->kind != TK_RETURN ||
		strlen(op) != token->len ||
		memcmp(token->str, op, token->len))
		return (false);
	token = token->next;
	return (true);
}

void	expect(char *op)
{
	if (token->kind != TK_RESERVED ||
		strlen(op) != token->len ||
		memcmp(token->str, op, token->len))
		error_at(token->str, "Not '%s'", op);
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

void	program(void)
{
	int i;

	i = 0;
	while (!(at_eof()))
		code[i++] = stmt();
	code[i] = NULL;
}

Node	*stmt(void)
{
	Node *node;

	if (consume_return("return"))
		node = new_node(ND_RETURN, expr(), NULL);
	else
		node = expr();
	expect(";");
	return (node);
}

Node	*expr(void)
{
	return (assign());
}

Node	*assign(void)
{
	Node *node;

	node = equality();
	if (consume("="))
		node = new_node(ND_ASSIGN, node, assign());
	return (node);
}

Node	*equality(void)
{
	Node *node;

	node = relational();
	while (1)
	{
		if (consume("=="))
			node = new_node(ND_EQU, node, relational());
		else if (consume("!="))
			node = new_node(ND_NEQ, node, relational());
		else
			return(node);
	}
}

Node	*relational(void)
{
	Node *node;

	node = add();
	while (1)
	{
		if (consume("<"))
			node = new_node(ND_SMA, node, add());
		else if (consume("<="))
			node = new_node(ND_SEQ, node, add());
		else if (consume(">"))
			node = new_node(ND_GRE, node, add());
		else if (consume(">="))
			node = new_node(ND_GEQ, node, add());
		else
			return (node);
	}
}

Node	*add(void)
{
	Node *node;

	node = mul();
	while (1)
	{
		if (consume("+"))
			node = new_node(ND_ADD, node, mul());
		else if (consume("-"))
			node = new_node(ND_SUB, node, mul());
		else
			return (node);
	}
}

Node	*mul(void)
{
	Node *node;

	node = unary();
	while (1)
	{
		if (consume("*"))
			node = new_node(ND_MUL, node, unary());
		else if (consume("/"))
			node = new_node(ND_DIV, node, unary());
		else
			return (node);
	}
}

Node	*unary(void)
{
	if (consume("+"))
		return (primary());
	if (consume("-"))
		return (new_node(ND_SUB, new_node_num(0), primary()));
	return (primary());
}

Lvar	*find_lvar(void)
{
	Lvar *tmp;

	tmp = locals;
	while (tmp)
	{
		if (tmp->len == token->len && !memcmp(tmp->name, token->str, tmp->len))
			return (tmp);
		tmp = tmp->next;
	}
	return (NULL);
}

Node	*primary(void)
{
	Node *node;
	Lvar *curr;
	Lvar *lvar;

	if (check_ident())
	{
		node = calloc(1, sizeof(Node));
		node->kind = ND_LVAR;
		lvar = find_lvar();
		if (lvar)
		{
			node->offset = lvar->offset;
			token = token->next;
		}
		else
		{
			curr = locals;
			while (curr->next)
				curr = curr->next;
			lvar = calloc(1, sizeof(Lvar));
			lvar->next = NULL;
			lvar->name = token->str;
			lvar->len = token->len;
			lvar->offset = curr->offset + 8;
			node->offset = lvar->offset;
			curr->next = lvar;
			token = token->next;
		}
		return (node);
	}
	else if (consume("("))
	{
		Node *node = expr();
		expect(")");
		return (node);
	}
	return new_node_num(expect_number());
}