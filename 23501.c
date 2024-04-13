static Elf32_Sym * __init find_symbol32(struct lib32_elfinfo *lib,
					const char *symname)
{
	unsigned int i;
	char name[MAX_SYMNAME], *c;

	for (i = 0; i < (lib->dynsymsize / sizeof(Elf32_Sym)); i++) {
		if (lib->dynsym[i].st_name == 0)
			continue;
		strlcpy(name, lib->dynstr + lib->dynsym[i].st_name,
			MAX_SYMNAME);
		c = strchr(name, '@');
		if (c)
			*c = 0;
		if (strcmp(symname, name) == 0)
			return &lib->dynsym[i];
	}
	return NULL;
}