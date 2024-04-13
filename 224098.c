static int prepare_arg_pages(struct linux_binprm *bprm,
			struct user_arg_ptr argv, struct user_arg_ptr envp)
{
	unsigned long limit, ptr_size;

	bprm->argc = count(argv, MAX_ARG_STRINGS);
	if (bprm->argc < 0)
		return bprm->argc;

	bprm->envc = count(envp, MAX_ARG_STRINGS);
	if (bprm->envc < 0)
		return bprm->envc;

	/*
	 * Limit to 1/4 of the max stack size or 3/4 of _STK_LIM
	 * (whichever is smaller) for the argv+env strings.
	 * This ensures that:
	 *  - the remaining binfmt code will not run out of stack space,
	 *  - the program will have a reasonable amount of stack left
	 *    to work from.
	 */
	limit = _STK_LIM / 4 * 3;
	limit = min(limit, bprm->rlim_stack.rlim_cur / 4);
	/*
	 * We've historically supported up to 32 pages (ARG_MAX)
	 * of argument strings even with small stacks
	 */
	limit = max_t(unsigned long, limit, ARG_MAX);
	/*
	 * We must account for the size of all the argv and envp pointers to
	 * the argv and envp strings, since they will also take up space in
	 * the stack. They aren't stored until much later when we can't
	 * signal to the parent that the child has run out of stack space.
	 * Instead, calculate it here so it's possible to fail gracefully.
	 */
	ptr_size = (bprm->argc + bprm->envc) * sizeof(void *);
	if (limit <= ptr_size)
		return -E2BIG;
	limit -= ptr_size;

	bprm->argmin = bprm->p - limit;
	return 0;
}