static int __init vdso_do_func_patch64(struct lib32_elfinfo *v32,
				       struct lib64_elfinfo *v64,
				       const char *orig, const char *fix)
{
	Elf64_Sym *sym64_gen, *sym64_fix;

	sym64_gen = find_symbol64(v64, orig);
	if (sym64_gen == NULL) {
		printk(KERN_ERR "vDSO64: Can't find symbol %s !\n", orig);
		return -1;
	}
	if (fix == NULL) {
		sym64_gen->st_name = 0;
		return 0;
	}
	sym64_fix = find_symbol64(v64, fix);
	if (sym64_fix == NULL) {
		printk(KERN_ERR "vDSO64: Can't find symbol %s !\n", fix);
		return -1;
	}
	sym64_gen->st_value = sym64_fix->st_value;
	sym64_gen->st_size = sym64_fix->st_size;
	sym64_gen->st_info = sym64_fix->st_info;
	sym64_gen->st_other = sym64_fix->st_other;
	sym64_gen->st_shndx = sym64_fix->st_shndx;

	return 0;
}