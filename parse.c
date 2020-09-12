#include "9cc.h"

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
		if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' || *p == '<' || *p == '>')
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
		return false;
	token = token->next;
	return true;
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

Node	*expr(void)
{
	return (equality());
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

Node	*primary(void)
{
	if (consume("("))
	{
		Node *node = expr();
		expect(")");
		return (node);
	}
	return new_node_num(expect_number());
}