static int samldb_prim_group_change(struct samldb_ctx *ac)
{
	struct ldb_context *ldb = ldb_module_get_ctx(ac->module);
	const char * const attrs[] = {
		"primaryGroupID",
		"memberOf",
		"userAccountControl",
		NULL };
	struct ldb_result *res, *group_res;
	struct ldb_message_element *el;
	struct ldb_message *msg;
	uint32_t prev_rid, new_rid, uac;
	struct dom_sid *prev_sid, *new_sid;
	struct ldb_dn *prev_prim_group_dn, *new_prim_group_dn;
	int ret;
	const char * const noattrs[] = { NULL };

	el = dsdb_get_single_valued_attr(ac->msg, "primaryGroupID",
					 ac->req->operation);
	if (el == NULL) {
		/* we are not affected */
		return LDB_SUCCESS;
	}

	/* Fetch information from the existing object */

	ret = dsdb_module_search_dn(ac->module, ac, &res, ac->msg->dn, attrs,
				    DSDB_FLAG_NEXT_MODULE, ac->req);
	if (ret != LDB_SUCCESS) {
		return ret;
	}

	uac = ldb_msg_find_attr_as_uint(res->msgs[0], "userAccountControl", 0);

	/* Finds out the DN of the old primary group */

	prev_rid = ldb_msg_find_attr_as_uint(res->msgs[0], "primaryGroupID",
					     (uint32_t) -1);
	if (prev_rid == (uint32_t) -1) {
		/* User objects do always have a mandatory "primaryGroupID"
		 * attribute. If this doesn't exist then the object is of the
		 * wrong type. This is the exact Windows error code */
		return LDB_ERR_OBJECT_CLASS_VIOLATION;
	}

	prev_sid = dom_sid_add_rid(ac, samdb_domain_sid(ldb), prev_rid);
	if (prev_sid == NULL) {
		return ldb_operr(ldb);
	}

	/* Finds out the DN of the new primary group
	 * Notice: in order to parse the primary group ID correctly we create
	 * a temporary message here. */

	msg = ldb_msg_new(ac->msg);
	if (msg == NULL) {
		return ldb_module_oom(ac->module);
	}
	ret = ldb_msg_add(msg, el, 0);
	if (ret != LDB_SUCCESS) {
		return ret;
	}
	new_rid = ldb_msg_find_attr_as_uint(msg, "primaryGroupID", (uint32_t) -1);
	talloc_free(msg);
	if (new_rid == (uint32_t) -1) {
		/* we aren't affected of any primary group change */
		return LDB_SUCCESS;
	}

	if (prev_rid == new_rid) {
		return LDB_SUCCESS;
	}

	if ((uac & UF_SERVER_TRUST_ACCOUNT) && new_rid != DOMAIN_RID_DCS) {
		ldb_asprintf_errstring(ldb,
			"%08X: samldb: UF_SERVER_TRUST_ACCOUNT requires "
			"primaryGroupID=%u!",
			W_ERROR_V(WERR_DS_CANT_MOD_PRIMARYGROUPID),
			DOMAIN_RID_DCS);
		return LDB_ERR_UNWILLING_TO_PERFORM;
	}

	if ((uac & UF_PARTIAL_SECRETS_ACCOUNT) && new_rid != DOMAIN_RID_READONLY_DCS) {
		ldb_asprintf_errstring(ldb,
			"%08X: samldb: UF_PARTIAL_SECRETS_ACCOUNT requires "
			"primaryGroupID=%u!",
			W_ERROR_V(WERR_DS_CANT_MOD_PRIMARYGROUPID),
			DOMAIN_RID_READONLY_DCS);
		return LDB_ERR_UNWILLING_TO_PERFORM;
	}

	ret = dsdb_module_search(ac->module, ac, &group_res,
				 ldb_get_default_basedn(ldb),
				 LDB_SCOPE_SUBTREE,
				 noattrs, DSDB_FLAG_NEXT_MODULE,
				 ac->req,
				 "(objectSid=%s)",
				 ldap_encode_ndr_dom_sid(ac, prev_sid));
	if (ret != LDB_SUCCESS) {
		return ret;
	}
	if (group_res->count != 1) {
		return ldb_operr(ldb);
	}
	prev_prim_group_dn = group_res->msgs[0]->dn;

	new_sid = dom_sid_add_rid(ac, samdb_domain_sid(ldb), new_rid);
	if (new_sid == NULL) {
		return ldb_operr(ldb);
	}

	ret = dsdb_module_search(ac->module, ac, &group_res,
				 ldb_get_default_basedn(ldb),
				 LDB_SCOPE_SUBTREE,
				 noattrs, DSDB_FLAG_NEXT_MODULE,
				 ac->req,
				 "(objectSid=%s)",
				 ldap_encode_ndr_dom_sid(ac, new_sid));
	if (ret != LDB_SUCCESS) {
		return ret;
	}
	if (group_res->count != 1) {
		/* Here we know if the specified new primary group candidate is
		 * valid or not. */
		return LDB_ERR_UNWILLING_TO_PERFORM;
	}
	new_prim_group_dn = group_res->msgs[0]->dn;

	/* We need to be already a normal member of the new primary
	 * group in order to be successful. */
	el = samdb_find_attribute(ldb, res->msgs[0], "memberOf",
				  ldb_dn_get_linearized(new_prim_group_dn));
	if (el == NULL) {
		return LDB_ERR_UNWILLING_TO_PERFORM;
	}

	/* Remove the "member" attribute on the new primary group */
	msg = ldb_msg_new(ac->msg);
	if (msg == NULL) {
		return ldb_module_oom(ac->module);
	}
	msg->dn = new_prim_group_dn;

	ret = samdb_msg_add_delval(ldb, msg, msg, "member",
				   ldb_dn_get_linearized(ac->msg->dn));
	if (ret != LDB_SUCCESS) {
		return ret;
	}

	ret = dsdb_module_modify(ac->module, msg, DSDB_FLAG_NEXT_MODULE, ac->req);
	if (ret != LDB_SUCCESS) {
		return ret;
	}
	talloc_free(msg);

	/* Add a "member" attribute for the previous primary group */
	msg = ldb_msg_new(ac->msg);
	if (msg == NULL) {
		return ldb_module_oom(ac->module);
	}
	msg->dn = prev_prim_group_dn;

	ret = samdb_msg_add_addval(ldb, msg, msg, "member",
				   ldb_dn_get_linearized(ac->msg->dn));
	if (ret != LDB_SUCCESS) {
		return ret;
	}

	ret = dsdb_module_modify(ac->module, msg, DSDB_FLAG_NEXT_MODULE, ac->req);
	if (ret != LDB_SUCCESS) {
		return ret;
	}
	talloc_free(msg);

	return LDB_SUCCESS;
}
