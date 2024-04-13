static unsigned long __init find_function64(struct lib64_elfinfo *lib,
					    const char *symname)
{
	Elf64_Sym *sym = find_symbol64(lib, symname);

	if (sym == NULL) {
		printk(KERN_WARNING "vDSO64: function %s not found !\n",
		       symname);
		return 0;
	}
#ifdef VDS64_HAS_DESCRIPTORS
	return *((u64 *)(vdso64_kbase + sym->st_value - VDSO64_LBASE)) -
		VDSO64_LBASE;
#else
	return sym->st_value - VDSO64_LBASE;
#endif
}