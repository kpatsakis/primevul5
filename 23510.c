void print_vma_addr(char *prefix, unsigned long ip)
{
	struct mm_struct *mm = current->mm;
	struct vm_area_struct *vma;

	/*
	 * Do not print if we are in atomic
	 * contexts (in exception stacks, etc.):
	 */
	if (preempt_count())
		return;

	down_read(&mm->mmap_sem);
	vma = find_vma(mm, ip);
	if (vma && vma->vm_file) {
		struct file *f = vma->vm_file;
		char *buf = (char *)__get_free_page(GFP_KERNEL);
		if (buf) {
			char *p, *s;

			p = d_path(&f->f_path, buf, PAGE_SIZE);
			if (IS_ERR(p))
				p = "?";
			s = strrchr(p, '/');
			if (s)
				p = s+1;
			printk("%s%s[%lx+%lx]", prefix, p,
					vma->vm_start,
					vma->vm_end - vma->vm_start);
			free_page((unsigned long)buf);
		}
	}
	up_read(&current->mm->mmap_sem);
}