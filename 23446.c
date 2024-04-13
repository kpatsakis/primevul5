asmlinkage long sys_umask(int mask)
{
	mask = xchg(&current->fs->umask, mask & S_IRWXUGO);
	return mask;
}