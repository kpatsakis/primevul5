static int samldb_find_for_defaultObjectCategory(struct samldb_ctx *ac)
{
	struct ldb_context *ldb = ldb_module_get_ctx(ac->module);
	struct ldb_result *res;
	const char * const no_attrs[] = { NULL };
	int ret;

	ac->res_dn = NULL;

	ret = dsdb_module_search(ac->module, ac, &res,
				 ac->dn, LDB_SCOPE_BASE, no_attrs,
				 DSDB_SEARCH_SHOW_DN_IN_STORAGE_FORMAT
				 | DSDB_FLAG_NEXT_MODULE,
				 ac->req,
				 "(objectClass=classSchema)");
	if (ret == LDB_ERR_NO_SUCH_OBJECT) {
		/* Don't be pricky when the DN doesn't exist if we have the */
		/* RELAX control specified */
		if (ldb_request_get_control(ac->req,
					    LDB_CONTROL_RELAX_OID) == NULL) {
			ldb_set_errstring(ldb,
					  "samldb_find_defaultObjectCategory: "
					  "Invalid DN for 'defaultObjectCategory'!");
			return LDB_ERR_CONSTRAINT_VIOLATION;
		}
	}
	if ((ret != LDB_ERR_NO_SUCH_OBJECT) && (ret != LDB_SUCCESS)) {
		return ret;
	}

	if (ret == LDB_SUCCESS) {
		/* ensure the defaultObjectCategory has a full GUID */
		struct ldb_message *m;
		m = ldb_msg_new(ac->msg);
		if (m == NULL) {
			return ldb_oom(ldb);
		}
		m->dn = ac->msg->dn;
		if (ldb_msg_add_string(m, "defaultObjectCategory",
				       ldb_dn_get_extended_linearized(m, res->msgs[0]->dn, 1)) !=
		    LDB_SUCCESS) {
			return ldb_oom(ldb);
		}
		m->elements[0].flags = LDB_FLAG_MOD_REPLACE;

		ret = dsdb_module_modify(ac->module, m,
					 DSDB_FLAG_NEXT_MODULE,
					 ac->req);
		if (ret != LDB_SUCCESS) {
			return ret;
		}
	}


	ac->res_dn = ac->dn;

	return samldb_next_step(ac);
}
