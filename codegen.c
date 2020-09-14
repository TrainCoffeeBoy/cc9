#include "9cc.h"

void	gen_lval(Node *node)
{
	if (node->kind != ND_LVAR)
		error("Lhs is not variable.\n");
	printf("	mov rax, rbp\n");
	printf("	sub rax, %d\n", node->offset);
	printf("	push rax\n");
}

void	gen(Node *node)
{
	if (node->kind == ND_NUM)
	{
		printf("	push %d\n", node->val);
		return ;
	}
	else if (node->kind == ND_LVAR)
	{
		gen_lval(node);
		printf("	pop rax\n");
		printf("	mov rax, [rax]\n");
		printf("	push rax\n");
		return ;
	}
	else if (node->kind == ND_ASSIGN)
	{
		gen_lval(node->lhs);
		gen(node->rhs);
		printf("	pop rdi\n");
		printf("	pop rax\n");
		printf("	mov [rax], rdi\n");
		printf("	push rdi\n");
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