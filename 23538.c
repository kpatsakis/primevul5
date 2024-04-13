int migrate_vmas(struct mm_struct *mm, const nodemask_t *to,
	const nodemask_t *from, unsigned long flags)
{
 	struct vm_area_struct *vma;
 	int err = 0;

 	for(vma = mm->mmap; vma->vm_next && !err; vma = vma->vm_next) {
 		if (vma->vm_ops && vma->vm_ops->migrate) {
 			err = vma->vm_ops->migrate(vma, to, from, flags);
 			if (err)
 				break;
 		}
 	}
 	return err;
}