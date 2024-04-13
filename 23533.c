static __init int vdso_fixup_datapage(struct lib32_elfinfo *v32,
				       struct lib64_elfinfo *v64)
{
	Elf32_Sym *sym32;
#ifdef CONFIG_PPC64
	Elf64_Sym *sym64;

       	sym64 = find_symbol64(v64, "__kernel_datapage_offset");
	if (sym64 == NULL) {
		printk(KERN_ERR "vDSO64: Can't find symbol "
		       "__kernel_datapage_offset !\n");
		return -1;
	}
	*((int *)(vdso64_kbase + sym64->st_value - VDSO64_LBASE)) =
		(vdso64_pages << PAGE_SHIFT) -
		(sym64->st_value - VDSO64_LBASE);
#endif /* CONFIG_PPC64 */

	sym32 = find_symbol32(v32, "__kernel_datapage_offset");
	if (sym32 == NULL) {
		printk(KERN_ERR "vDSO32: Can't find symbol "
		       "__kernel_datapage_offset !\n");
		return -1;
	}
	*((int *)(vdso32_kbase + (sym32->st_value - VDSO32_LBASE))) =
		(vdso32_pages << PAGE_SHIFT) -
		(sym32->st_value - VDSO32_LBASE);

	return 0;
}