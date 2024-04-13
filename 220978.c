inflate_generic_header (MonoMethodHeader *header, MonoGenericContext *context)
{
	MonoMethodHeader *res;
	int i;
	res = g_malloc0 (MONO_SIZEOF_METHOD_HEADER + sizeof (gpointer) * header->num_locals);
	res->code = header->code;
	res->code_size = header->code_size;
	res->max_stack = header->max_stack;
	res->num_clauses = header->num_clauses;
	res->init_locals = header->init_locals;
	res->num_locals = header->num_locals;
	res->clauses = header->clauses;
	for (i = 0; i < header->num_locals; ++i)
		res->locals [i] = mono_class_inflate_generic_type (header->locals [i], context);
	if (res->num_clauses) {
		res->clauses = g_memdup (header->clauses, sizeof (MonoExceptionClause) * res->num_clauses);
		for (i = 0; i < header->num_clauses; ++i) {
			MonoExceptionClause *clause = &res->clauses [i];
			if (clause->flags != MONO_EXCEPTION_CLAUSE_NONE)
				continue;
			clause->data.catch_class = mono_class_inflate_generic_class (clause->data.catch_class, context);
		}
	}
	return res;
}