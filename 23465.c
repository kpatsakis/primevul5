asmlinkage long sys_getpgrp(void)
{
	/* SMP - assuming writes are word atomic this is fine */
	return process_group(current);
}