asmlinkage long sys_getresgid(gid_t __user *rgid, gid_t __user *egid, gid_t __user *sgid)
{
	int retval;

	if (!(retval = put_user(current->gid, rgid)) &&
	    !(retval = put_user(current->egid, egid)))
		retval = put_user(current->sgid, sgid);

	return retval;
}