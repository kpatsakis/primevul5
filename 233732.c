static int fault_in_user_writeable(u32 __user *uaddr)
{
	struct mm_struct *mm = current->mm;
	int ret;

	down_read(&mm->mmap_sem);
	ret = get_user_pages(current, mm, (unsigned long)uaddr,
			     1, 1, 0, NULL, NULL);
	up_read(&mm->mmap_sem);

	return ret < 0 ? ret : 0;
}