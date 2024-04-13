static int __init vdso_do_func_patch32(struct lib32_elfinfo *v32,
				       struct lib64_elfinfo *v64,
				       const char *orig, const char *fix)
{
	Elf32_Sym *sym32_gen, *sym32_fix;

	sym32_gen = find_symbol32(v32, orig);
	if (sym32_gen == NULL) {
		printk(KERN_ERR "vDSO32: Can't find symbol %s !\n", orig);
		return -1;
	}
	if (fix == NULL) {
		sym32_gen->st_name = 0;
		return 0;
	}
	sym32_fix = find_symbol32(v32, fix);
	if (sym32_fix == NULL) {
		printk(KERN_ERR "vDSO32: Can't find symbol %s !\n", fix);
		return -1;
	}
	sym32_gen->st_value = sym32_fix->st_value;
	sym32_gen->st_size = sym32_fix->st_size;
	sym32_gen->st_info = sym32_fix->st_info;
	sym32_gen->st_other = sym32_fix->st_other;
	sym32_gen->st_shndx = sym32_fix->st_shndx;

	return 0;
}