static int samldb_check_sAMAccountName(struct samldb_ctx *ac)
{
	struct ldb_context *ldb = ldb_module_get_ctx(ac->module);
	const char *name;
	int ret;
	struct ldb_result *res;
	const char * const noattrs[] = { NULL };

	if (ldb_msg_find_element(ac->msg, "sAMAccountName") == NULL) {
		ret = samldb_generate_sAMAccountName(ldb, ac->msg);
		if (ret != LDB_SUCCESS) {
			return ret;
		}
	}

	name = ldb_msg_find_attr_as_string(ac->msg, "sAMAccountName", NULL);
	if (name == NULL) {
		/* The "sAMAccountName" cannot be nothing */
		ldb_set_errstring(ldb,
				  "samldb: Empty account names aren't allowed!");
		return LDB_ERR_CONSTRAINT_VIOLATION;
	}

	ret = dsdb_module_search(ac->module, ac, &res,
				 ldb_get_default_basedn(ldb), LDB_SCOPE_SUBTREE, noattrs,
				 DSDB_FLAG_NEXT_MODULE,
				 ac->req,
				 "(sAMAccountName=%s)",
				 ldb_binary_encode_string(ac, name));
	if (ret != LDB_SUCCESS) {
		return ret;
	}
	if (res->count != 0) {
		ldb_asprintf_errstring(ldb,
				       "samldb: Account name (sAMAccountName) '%s' already in use!",
				       name);
		talloc_free(res);
		return LDB_ERR_ENTRY_ALREADY_EXISTS;
	}
	talloc_free(res);

	return samldb_next_step(ac);
}
