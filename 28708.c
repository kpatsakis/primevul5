static int samldb_add_handle_msDS_IntId(struct samldb_ctx *ac)
{
	int ret;
	bool id_exists;
	uint32_t msds_intid;
	int32_t system_flags;
	struct ldb_context *ldb;
	struct ldb_result *ldb_res;
	struct ldb_dn *schema_dn;
	struct samldb_msds_intid_persistant *msds_intid_struct;
	struct dsdb_schema *schema;

	ldb = ldb_module_get_ctx(ac->module);
	schema_dn = ldb_get_schema_basedn(ldb);

	/* replicated update should always go through */
	if (ldb_request_get_control(ac->req,
				    DSDB_CONTROL_REPLICATED_UPDATE_OID)) {
		return LDB_SUCCESS;
	}

	/* msDS-IntId is handled by system and should never be
	 * passed by clients */
	if (ldb_msg_find_element(ac->msg, "msDS-IntId")) {
		return LDB_ERR_UNWILLING_TO_PERFORM;
	}

	/* do not generate msDS-IntId if Relax control is passed */
	if (ldb_request_get_control(ac->req, LDB_CONTROL_RELAX_OID)) {
		return LDB_SUCCESS;
	}

	/* check Functional Level */
	if (dsdb_functional_level(ldb) < DS_DOMAIN_FUNCTION_2003) {
		return LDB_SUCCESS;
	}

	/* check systemFlags for SCHEMA_BASE_OBJECT flag */
	system_flags = ldb_msg_find_attr_as_int(ac->msg, "systemFlags", 0);
	if (system_flags & SYSTEM_FLAG_SCHEMA_BASE_OBJECT) {
		return LDB_SUCCESS;
	}
	schema = dsdb_get_schema(ldb, NULL);
	if (!schema) {
		ldb_debug_set(ldb, LDB_DEBUG_FATAL,
			      "samldb_schema_info_update: no dsdb_schema loaded");
		DEBUG(0,(__location__ ": %s\n", ldb_errstring(ldb)));
		return ldb_operr(ldb);
	}

	msds_intid_struct = (struct samldb_msds_intid_persistant*) ldb_get_opaque(ldb, SAMLDB_MSDS_INTID_OPAQUE);
	if (!msds_intid_struct) {
		msds_intid_struct = talloc(ldb, struct samldb_msds_intid_persistant);
		/* Generate new value for msDs-IntId
		* Value should be in 0x80000000..0xBFFFFFFF range */
		msds_intid = generate_random() % 0X3FFFFFFF;
		msds_intid += 0x80000000;
		msds_intid_struct->msds_intid = msds_intid;
		msds_intid_struct->usn = schema->loaded_usn;
		DEBUG(2, ("No samldb_msds_intid_persistant struct, allocating a new one\n"));
	} else {
		msds_intid = msds_intid_struct->msds_intid;
	}

	/* probe id values until unique one is found */
	do {
		uint64_t current_usn;
		msds_intid++;
		if (msds_intid > 0xBFFFFFFF) {
			msds_intid = 0x80000001;
		}
		/*
		 * Alternative strategy to a costly (even indexed search) to the
		 * database.
		 * We search in the schema if we have already this intid (using dsdb_attribute_by_attributeID_id because
		 * in the range 0x80000000 0xBFFFFFFFF, attributeID is a DSDB_ATTID_TYPE_INTID).
		 * If so generate another random value.
		 * If not check if the highest USN in the database for the schema partition is the
		 * one that we know.
		 * If so it means that's only this ldb context that is touching the schema in the database.
		 * If not it means that's someone else has modified the database while we are doing our changes too
		 * (this case should be very bery rare) in order to be sure do the search in the database.
		 */
		if (dsdb_attribute_by_attributeID_id(schema, msds_intid)) {
			msds_intid = generate_random() % 0X3FFFFFFF;
			msds_intid += 0x80000000;
			continue;
		}

		ret = dsdb_module_load_partition_usn(ac->module, schema_dn,
						     &current_usn, NULL, NULL);
		if (ret != LDB_SUCCESS) {
			ldb_debug_set(ldb, LDB_DEBUG_ERROR,
				      __location__": Searching for schema USN failed: %s\n",
				      ldb_errstring(ldb));
			return ldb_operr(ldb);
		}

		/* current_usn can be lesser than msds_intid_struct-> if there is
		 * uncommited changes.
		 */
		if (current_usn > msds_intid_struct->usn) {
			/* oups something has changed, someone/something
			 * else is modifying or has modified the schema
			 * we'd better check this intid is the database directly
			 */

			DEBUG(2, ("Schema has changed, searching the database for the unicity of %d\n",
					msds_intid));

			ret = dsdb_module_search(ac->module, ac,
						&ldb_res,
						schema_dn, LDB_SCOPE_ONELEVEL, NULL,
						DSDB_FLAG_NEXT_MODULE,
						ac->req,
						"(msDS-IntId=%d)", msds_intid);
			if (ret != LDB_SUCCESS) {
				ldb_debug_set(ldb, LDB_DEBUG_ERROR,
					__location__": Searching for msDS-IntId=%d failed - %s\n",
					msds_intid,
					ldb_errstring(ldb));
				return ldb_operr(ldb);
			}
			id_exists = (ldb_res->count > 0);
			talloc_free(ldb_res);
		} else {
			id_exists = 0;
		}

	} while(id_exists);
	msds_intid_struct->msds_intid = msds_intid;
	ldb_set_opaque(ldb, SAMLDB_MSDS_INTID_OPAQUE, msds_intid_struct);

	return samdb_msg_add_int(ldb, ac->msg, ac->msg, "msDS-IntId",
				 msds_intid);
}
