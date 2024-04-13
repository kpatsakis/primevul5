static __init int vdso_do_find_sections(struct lib32_elfinfo *v32,
					struct lib64_elfinfo *v64)
{
	void *sect;

	/*
	 * Locate symbol tables & text section
	 */

	v32->dynsym = find_section32(v32->hdr, ".dynsym", &v32->dynsymsize);
	v32->dynstr = find_section32(v32->hdr, ".dynstr", NULL);
	if (v32->dynsym == NULL || v32->dynstr == NULL) {
		printk(KERN_ERR "vDSO32: required symbol section not found\n");
		return -1;
	}
	sect = find_section32(v32->hdr, ".text", NULL);
	if (sect == NULL) {
		printk(KERN_ERR "vDSO32: the .text section was not found\n");
		return -1;
	}
	v32->text = sect - vdso32_kbase;

#ifdef CONFIG_PPC64
	v64->dynsym = find_section64(v64->hdr, ".dynsym", &v64->dynsymsize);
	v64->dynstr = find_section64(v64->hdr, ".dynstr", NULL);
	if (v64->dynsym == NULL || v64->dynstr == NULL) {
		printk(KERN_ERR "vDSO64: required symbol section not found\n");
		return -1;
	}
	sect = find_section64(v64->hdr, ".text", NULL);
	if (sect == NULL) {
		printk(KERN_ERR "vDSO64: the .text section was not found\n");
		return -1;
	}
	v64->text = sect - vdso64_kbase;
#endif /* CONFIG_PPC64 */

	return 0;
}