int arch_setup_additional_pages(struct linux_binprm *bprm,
				int executable_stack)
{
	struct mm_struct *mm = current->mm;
	struct page **vdso_pagelist;
	unsigned long vdso_pages;
	unsigned long vdso_base;
	int rc;

	if (!vdso_ready)
		return 0;

#ifdef CONFIG_PPC64
	if (test_thread_flag(TIF_32BIT)) {
		vdso_pagelist = vdso32_pagelist;
		vdso_pages = vdso32_pages;
		vdso_base = VDSO32_MBASE;
	} else {
		vdso_pagelist = vdso64_pagelist;
		vdso_pages = vdso64_pages;
		vdso_base = VDSO64_MBASE;
	}
#else
	vdso_pagelist = vdso32_pagelist;
	vdso_pages = vdso32_pages;
	vdso_base = VDSO32_MBASE;
#endif

	current->mm->context.vdso_base = 0;

	/* vDSO has a problem and was disabled, just don't "enable" it for the
	 * process
	 */
	if (vdso_pages == 0)
		return 0;
	/* Add a page to the vdso size for the data page */
	vdso_pages ++;

	/*
	 * pick a base address for the vDSO in process space. We try to put it
	 * at vdso_base which is the "natural" base for it, but we might fail
	 * and end up putting it elsewhere.
	 */
	down_write(&mm->mmap_sem);
	vdso_base = get_unmapped_area(NULL, vdso_base,
				      vdso_pages << PAGE_SHIFT, 0, 0);
	if (IS_ERR_VALUE(vdso_base)) {
		rc = vdso_base;
		goto fail_mmapsem;
	}

	/*
	 * our vma flags don't have VM_WRITE so by default, the process isn't
	 * allowed to write those pages.
	 * gdb can break that with ptrace interface, and thus trigger COW on
	 * those pages but it's then your responsibility to never do that on
	 * the "data" page of the vDSO or you'll stop getting kernel updates
	 * and your nice userland gettimeofday will be totally dead.
	 * It's fine to use that for setting breakpoints in the vDSO code
	 * pages though
	 *
	 * Make sure the vDSO gets into every core dump.
	 * Dumping its contents makes post-mortem fully interpretable later
	 * without matching up the same kernel and hardware config to see
	 * what PC values meant.
	 */
	rc = install_special_mapping(mm, vdso_base, vdso_pages << PAGE_SHIFT,
				     VM_READ|VM_EXEC|
				     VM_MAYREAD|VM_MAYWRITE|VM_MAYEXEC|
				     VM_ALWAYSDUMP,
				     vdso_pagelist);
	if (rc)
		goto fail_mmapsem;

	/* Put vDSO base into mm struct */
	current->mm->context.vdso_base = vdso_base;

	up_write(&mm->mmap_sem);
	return 0;

 fail_mmapsem:
	up_write(&mm->mmap_sem);
	return rc;
}