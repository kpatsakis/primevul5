static bool samldb_krbtgtnumber_available(struct samldb_ctx *ac,
					  uint32_t krbtgt_number)
{
	TALLOC_CTX *tmp_ctx = talloc_new(ac);
	struct ldb_result *res;
	const char * const no_attrs[] = { NULL };
	int ret;

	ret = dsdb_module_search(ac->module, tmp_ctx, &res,
				 ldb_get_default_basedn(ldb_module_get_ctx(ac->module)),
				 LDB_SCOPE_SUBTREE, no_attrs,
				 DSDB_FLAG_NEXT_MODULE,
				 ac->req,
				 "(msDC-SecondaryKrbTgtNumber=%u)",
				 krbtgt_number);
	if (ret == LDB_SUCCESS && res->count == 0) {
		talloc_free(tmp_ctx);
		return true;
	}
	talloc_free(tmp_ctx);
	return false;
}
