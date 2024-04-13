fe_op_modrdn( Operation *op, SlapReply *rs )
{
	struct berval	dest_ndn = BER_BVNULL, dest_pndn, pdn = BER_BVNULL;
	BackendDB	*op_be, *bd = op->o_bd;
	ber_slen_t	diff;
	
	if( op->o_req_ndn.bv_len == 0 ) {
		Debug( LDAP_DEBUG_ANY, "%s do_modrdn: root dse!\n",
			op->o_log_prefix, 0, 0 );
		send_ldap_error( op, rs, LDAP_UNWILLING_TO_PERFORM,
			"cannot rename the root DSE" );
		goto cleanup;

	} else if ( bvmatch( &op->o_req_ndn, &frontendDB->be_schemandn ) ) {
		Debug( LDAP_DEBUG_ANY, "%s do_modrdn: subschema subentry: %s (%ld)\n",
			op->o_log_prefix, frontendDB->be_schemandn.bv_val, (long)frontendDB->be_schemandn.bv_len );

		send_ldap_error( op, rs, LDAP_UNWILLING_TO_PERFORM,
			"cannot rename subschema subentry" );
		goto cleanup;
	}

	if( op->orr_nnewSup ) {
		dest_pndn = *op->orr_nnewSup;
	} else {
		dnParent( &op->o_req_ndn, &dest_pndn );
	}
	build_new_dn( &dest_ndn, &dest_pndn, &op->orr_nnewrdn, op->o_tmpmemctx );

	diff = (ber_slen_t) dest_ndn.bv_len - (ber_slen_t) op->o_req_ndn.bv_len;
	if ( diff > 0 ? dnIsSuffix( &dest_ndn, &op->o_req_ndn )
		: diff < 0 && dnIsSuffix( &op->o_req_ndn, &dest_ndn ) )
	{
		send_ldap_error( op, rs, LDAP_UNWILLING_TO_PERFORM,
			diff > 0 ? "cannot place an entry below itself"
			: "cannot place an entry above itself" );
		goto cleanup;
	}

	/*
	 * We could be serving multiple database backends.  Select the
	 * appropriate one, or send a referral to our "referral server"
	 * if we don't hold it.
	 */
	op->o_bd = select_backend( &op->o_req_ndn, 1 );
	if ( op->o_bd == NULL ) {
		op->o_bd = bd;
		rs->sr_ref = referral_rewrite( default_referral,
			NULL, &op->o_req_dn, LDAP_SCOPE_DEFAULT );
		if (!rs->sr_ref) rs->sr_ref = default_referral;

		if ( rs->sr_ref != NULL ) {
			rs->sr_err = LDAP_REFERRAL;
			send_ldap_result( op, rs );

			if (rs->sr_ref != default_referral) ber_bvarray_free( rs->sr_ref );
		} else {
			send_ldap_error( op, rs, LDAP_UNWILLING_TO_PERFORM,
				"no global superior knowledge" );
		}
		goto cleanup;
	}

	/* If we've got a glued backend, check the real backend */
	op_be = op->o_bd;
	if ( SLAP_GLUE_INSTANCE( op->o_bd )) {
		op->o_bd = select_backend( &op->o_req_ndn, 0 );
	}

	/* check restrictions */
	if( backend_check_restrictions( op, rs, NULL ) != LDAP_SUCCESS ) {
		send_ldap_result( op, rs );
		goto cleanup;
	}

	/* check for referrals */
	if ( backend_check_referrals( op, rs ) != LDAP_SUCCESS ) {
		goto cleanup;
	}

	/* check that destination DN is in the same backend as source DN */
	if ( select_backend( &dest_ndn, 0 ) != op->o_bd ) {
			send_ldap_error( op, rs, LDAP_AFFECTS_MULTIPLE_DSAS,
				"cannot rename between DSAs" );
			goto cleanup;
	}

	/*
	 * do the modrdn if 1 && (2 || 3)
	 * 1) there is a modrdn function implemented in this backend;
	 * 2) this backend is the provider for what it holds;
	 * 3) it's a replica and the dn supplied is the update_ndn.
	 */
	if ( op->o_bd->be_modrdn ) {
		/* do the update here */
		int repl_user = be_isupdate( op );
		if ( !SLAP_SINGLE_SHADOW(op->o_bd) || repl_user )
		{
			op->o_bd = op_be;
			op->o_bd->be_modrdn( op, rs );

			if ( op->o_bd->be_delete ) {
				struct berval	org_req_dn = BER_BVNULL;
				struct berval	org_req_ndn = BER_BVNULL;
				struct berval	org_dn = BER_BVNULL;
				struct berval	org_ndn = BER_BVNULL;
				int		org_managedsait;

				org_req_dn = op->o_req_dn;
				org_req_ndn = op->o_req_ndn;
				org_dn = op->o_dn;
				org_ndn = op->o_ndn;
				org_managedsait = get_manageDSAit( op );
				op->o_dn = op->o_bd->be_rootdn;
				op->o_ndn = op->o_bd->be_rootndn;
				op->o_managedsait = SLAP_CONTROL_NONCRITICAL;

				while ( rs->sr_err == LDAP_SUCCESS &&
						op->o_delete_glue_parent ) {
					op->o_delete_glue_parent = 0;
					if ( !be_issuffix( op->o_bd, &op->o_req_ndn )) {
						slap_callback cb = { NULL };
						cb.sc_response = slap_null_cb;
						dnParent( &op->o_req_ndn, &pdn );
						op->o_req_dn = pdn;
						op->o_req_ndn = pdn;
						op->o_callback = &cb;
						op->o_bd->be_delete( op, rs );
					} else {
						break;
					}
				}
				op->o_managedsait = org_managedsait;
				op->o_dn = org_dn;
				op->o_ndn = org_ndn;
				op->o_req_dn = org_req_dn;
				op->o_req_ndn = org_req_ndn;
				op->o_delete_glue_parent = 0;
			}

		} else {
			BerVarray defref = op->o_bd->be_update_refs
				? op->o_bd->be_update_refs : default_referral;

			if ( defref != NULL ) {
				rs->sr_ref = referral_rewrite( defref,
					NULL, &op->o_req_dn, LDAP_SCOPE_DEFAULT );
				if (!rs->sr_ref) rs->sr_ref = defref;

				rs->sr_err = LDAP_REFERRAL;
				send_ldap_result( op, rs );

				if (rs->sr_ref != defref) ber_bvarray_free( rs->sr_ref );
			} else {
				send_ldap_error( op, rs, LDAP_UNWILLING_TO_PERFORM,
					"shadow context; no update referral" );
			}
		}
	} else {
		send_ldap_error( op, rs, LDAP_UNWILLING_TO_PERFORM,
			"operation not supported within namingContext" );
	}

cleanup:;
	if ( dest_ndn.bv_val != NULL )
		ber_memfree_x( dest_ndn.bv_val, op->o_tmpmemctx );
	op->o_bd = bd;
	return rs->sr_err;
}