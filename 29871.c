struct ldb_dn *ldb_dn_get_parent(TALLOC_CTX *mem_ctx, struct ldb_dn *dn)
{
	struct ldb_dn *new_dn;

	new_dn = ldb_dn_copy(mem_ctx, dn);
	if ( !new_dn ) {
		return NULL;
	}

	if ( ! ldb_dn_remove_child_components(new_dn, 1)) {
		talloc_free(new_dn);
		return NULL;
	}

	return new_dn;
}
