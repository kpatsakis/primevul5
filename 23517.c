static void dump_vdso_pages(struct vm_area_struct * vma)
{
	int i;

	if (!vma || test_thread_flag(TIF_32BIT)) {
		printk("vDSO32 @ %016lx:\n", (unsigned long)vdso32_kbase);
		for (i=0; i<vdso32_pages; i++) {
			struct page *pg = virt_to_page(vdso32_kbase +
						       i*PAGE_SIZE);
			struct page *upg = (vma && vma->vm_mm) ?
				follow_page(vma, vma->vm_start + i*PAGE_SIZE, 0)
				: NULL;
			dump_one_vdso_page(pg, upg);
		}
	}
	if (!vma || !test_thread_flag(TIF_32BIT)) {
		printk("vDSO64 @ %016lx:\n", (unsigned long)vdso64_kbase);
		for (i=0; i<vdso64_pages; i++) {
			struct page *pg = virt_to_page(vdso64_kbase +
						       i*PAGE_SIZE);
			struct page *upg = (vma && vma->vm_mm) ?
				follow_page(vma, vma->vm_start + i*PAGE_SIZE, 0)
				: NULL;
			dump_one_vdso_page(pg, upg);
		}
	}
}