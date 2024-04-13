static unsigned long __init find_function32(struct lib32_elfinfo *lib,
					    const char *symname)
{
	Elf32_Sym *sym = find_symbol32(lib, symname);

	if (sym == NULL) {
		printk(KERN_WARNING "vDSO32: function %s not found !\n",
		       symname);
		return 0;
	}
	return sym->st_value - VDSO32_LBASE;
}