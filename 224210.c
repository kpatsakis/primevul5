int do_execve_file(struct file *file, void *__argv, void *__envp)
{
	struct user_arg_ptr argv = { .ptr.native = __argv };
	struct user_arg_ptr envp = { .ptr.native = __envp };

	return __do_execve_file(AT_FDCWD, NULL, argv, envp, 0, file);
}