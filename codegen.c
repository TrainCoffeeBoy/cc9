#include "9cc.h"

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
	else if (node->kind == ND_DIV)
	{
		printf("	cqo\n");
		printf("	idiv rdi\n");
	}
	else if (node->kind == ND_EQU)
	{
		printf("	cmp rax, rdi\n");
		printf("	sete al\n");
		printf("	movzb rax, al\n");
	}
	else if (node->kind == ND_NEQ)
	{
		printf("	cmp rax, rdi\n");
		printf("	setne al\n");
		printf("	movzb rax, al\n");
	}
	else if (node->kind == ND_SMA)
	{
		printf("	cmp rax, rdi\n");
		printf("	setl al\n");
		printf("	movzb rax, al\n");
	}
	else if (node->kind == ND_SEQ)
	{
		printf("	cmp rax, rdi\n");
		printf("	setle al\n");
		printf("	movzb rax, al\n");
	}
	else if (node->kind == ND_GRE)
	{
		printf("	cmp rdi, rax\n");
		printf("	setl al\n");
		printf("	movzb rax, al\n");
	}
	else if (node->kind == ND_GEQ)
	{
		printf("	cmp rdi, rax\n");
		printf("	setle al\n");
		printf("	movzb rax, al\n");
	}
	printf("	push rax\n");
}