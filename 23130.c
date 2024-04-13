create_macro(const char *name, const widechar *definition, int definition_length,
		const int *substitutions, int substitution_count, int argument_count) {
	Macro *m = malloc(sizeof(Macro));
	m->name = strdup(name);
	widechar *definition_copy = malloc(definition_length * sizeof(widechar));
	memcpy(definition_copy, definition, definition_length * sizeof(widechar));
	m->definition = definition_copy;
	m->definition_length = definition_length;
	int *substitutions_copy = malloc(2 * substitution_count * sizeof(int));
	memcpy(substitutions_copy, substitutions, 2 * substitution_count * sizeof(int));
	m->substitutions = substitutions_copy;
	m->substitution_count = substitution_count;
	m->argument_count = argument_count;
	return m;
}