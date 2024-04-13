static struct ldb_dn_component ldb_dn_copy_component(
						TALLOC_CTX *mem_ctx,
						struct ldb_dn_component *src)
{
	struct ldb_dn_component dst;

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

	if (src->cf_value.data) {
		dst.cf_value = ldb_val_dup(mem_ctx, &(src->cf_value));
		if (dst.cf_value.data == NULL) {
			LDB_FREE(dst.value.data);
			LDB_FREE(dst.name);
			return dst;
		}

		dst.cf_name = talloc_strdup(mem_ctx, src->cf_name);
		if (dst.cf_name == NULL) {
			LDB_FREE(dst.cf_name);
			LDB_FREE(dst.value.data);
			LDB_FREE(dst.name);
			return dst;
		}
	} else {
		dst.cf_value.data = NULL;
		dst.cf_name = NULL;
	}

	return dst;
}
