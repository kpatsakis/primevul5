static struct ldb_dn_ext_component ldb_dn_ext_copy_component(
						TALLOC_CTX *mem_ctx,
						struct ldb_dn_ext_component *src)
{
	struct ldb_dn_ext_component dst;

	memset(&dst, 0, sizeof(dst));

	if (src == NULL) {
		return dst;
	}

	dst.value = ldb_val_dup(mem_ctx, &(src->value));
	if (dst.value.data == NULL) {
		return dst;
	}

	dst.name = talloc_strdup(mem_ctx, src->name);
	if (dst.name == NULL) {
		LDB_FREE(dst.value.data);
		return dst;
	}

	return dst;
}
