static int rtas_name_matches(char *s1, char *s2)
{
	struct kvm_rtas_token_args args;
	return !strncmp(s1, s2, sizeof(args.name));
}