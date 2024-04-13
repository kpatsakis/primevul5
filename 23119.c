free_macro(const Macro *macro) {
	if (macro) {
		free((char *)macro->name);
		free((char *)macro->definition);
		free((int *)macro->substitutions);
		free((Macro *)macro);
	}
}