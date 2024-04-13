struct mempolicy *__get_vma_policy(struct vm_area_struct *vma,
						unsigned long addr)
{
	struct mempolicy *pol = NULL;

	if (vma) {
		if (vma->vm_ops && vma->vm_ops->get_policy) {
			pol = vma->vm_ops->get_policy(vma, addr);
		} else if (vma->vm_policy) {
			pol = vma->vm_policy;

			/*
			 * shmem_alloc_page() passes MPOL_F_SHARED policy with
			 * a pseudo vma whose vma->vm_ops=NULL. Take a reference
			 * count on these policies which will be dropped by
			 * mpol_cond_put() later
			 */
			if (mpol_needs_cond_ref(pol))
				mpol_get(pol);
		}
	}

	return pol;
}