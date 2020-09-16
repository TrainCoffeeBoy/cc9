#include "9cc.h"

char	*user_input;
Token	*token;
Lvar 	*locals;

int		main(int argc, char **argv)
{
	Node	*node;
	int		i;

	if (argc != 2)
	{
		fprintf(stderr, "One argument is required.\n");
		return (1);
	}
	user_input = argv[1];
	token = tokenize(user_input);
	locals = calloc(1, sizeof(Lvar));
	locals->offset = 0;
	locals->next = NULL;
	program();
	printf(".intel_syntax noprefix\n");
	printf(".global main \n");
	printf("main:\n");
	printf("	push rbp\n");
	printf("	mov rbp, rsp\n");
	printf("	sub rsp, %d\n", 8 * 26);//26 alpabetic haracters
	i = -1;
	while (code[++i])
	{
		gen(code[i]);
		printf("	pop rax\n");
	}
	printf("	mov rsp, rbp\n");
	printf("	pop rbp\n");
	printf("	ret\n");
	return (0);
}