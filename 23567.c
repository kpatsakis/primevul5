void print_bad_pte(struct vm_area_struct *vma, pte_t pte, unsigned long vaddr)
{
	printk(KERN_ERR "Bad pte = %08llx, process = %s, "
			"vm_flags = %lx, vaddr = %lx\n",
		(long long)pte_val(pte),
		(vma->vm_mm == current->mm ? current->comm : "???"),
		vma->vm_flags, vaddr);
	dump_stack();
}