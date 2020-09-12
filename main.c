#include "9cc.h"

char	*user_input;
Token	*token;

int		main(int argc, char **argv)
{
	Node	*node;

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