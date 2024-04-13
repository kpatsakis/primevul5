static int samldb_prim_group_set(struct samldb_ctx *ac)
{
	struct ldb_context *ldb = ldb_module_get_ctx(ac->module);
	uint32_t rid;

	rid = ldb_msg_find_attr_as_uint(ac->msg, "primaryGroupID", (uint32_t) -1);
	if (rid == (uint32_t) -1) {
		/* we aren't affected of any primary group set */
		return LDB_SUCCESS;

	} else if (!ldb_request_get_control(ac->req, LDB_CONTROL_RELAX_OID)) {
		ldb_set_errstring(ldb,
				  "The primary group isn't settable on add operations!");
		return LDB_ERR_UNWILLING_TO_PERFORM;
	}

	return samldb_prim_group_tester(ac, rid);
}
