unsigned long thp_get_unmapped_area(struct file *filp, unsigned long addr,
		unsigned long len, unsigned long pgoff, unsigned long flags)
{
	unsigned long ret;
	loff_t off = (loff_t)pgoff << PAGE_SHIFT;

	if (!IS_DAX(filp->f_mapping->host) || !IS_ENABLED(CONFIG_FS_DAX_PMD))
		goto out;

	ret = __thp_get_unmapped_area(filp, addr, len, off, flags, PMD_SIZE);
	if (ret)
		return ret;
out:
	return current->mm->get_unmapped_area(filp, addr, len, pgoff, flags);
}