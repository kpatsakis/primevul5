static int samldb_schema_info_update(struct samldb_ctx *ac)
{
	int ret;
	struct ldb_context *ldb;
	struct dsdb_schema *schema;

	/* replicated update should always go through */
	if (ldb_request_get_control(ac->req,
				    DSDB_CONTROL_REPLICATED_UPDATE_OID)) {
		return LDB_SUCCESS;
	}

	/* do not update schemaInfo during provisioning */
	if (ldb_request_get_control(ac->req, LDB_CONTROL_RELAX_OID)) {
		return LDB_SUCCESS;
	}

	ldb = ldb_module_get_ctx(ac->module);
	schema = dsdb_get_schema(ldb, NULL);
	if (!schema) {
		ldb_debug_set(ldb, LDB_DEBUG_FATAL,
			      "samldb_schema_info_update: no dsdb_schema loaded");
		DEBUG(0,(__location__ ": %s\n", ldb_errstring(ldb)));
		return ldb_operr(ldb);
	}

	ret = dsdb_module_schema_info_update(ac->module, schema,
					     DSDB_FLAG_NEXT_MODULE|
					     DSDB_FLAG_AS_SYSTEM,
					     ac->req);
	if (ret != LDB_SUCCESS) {
		ldb_asprintf_errstring(ldb,
				       "samldb_schema_info_update: dsdb_module_schema_info_update failed with %s",
				       ldb_errstring(ldb));
		return ret;
	}

	return LDB_SUCCESS;
}
