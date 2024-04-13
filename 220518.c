static int check_attach_modify_return(struct bpf_prog *prog, unsigned long addr)
{
	if (within_error_injection_list(addr) ||
	    !strncmp(SECURITY_PREFIX, prog->aux->attach_func_name,
		     sizeof(SECURITY_PREFIX) - 1))
		return 0;

	return -EINVAL;
}