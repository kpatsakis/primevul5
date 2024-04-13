do_modrdn(
    Operation	*op,
    SlapReply	*rs
)
{
	struct berval	dn = BER_BVNULL;
	struct berval	newrdn = BER_BVNULL;
	struct berval	newSuperior = BER_BVNULL;
	ber_int_t	deloldrdn;

	struct berval pnewSuperior = BER_BVNULL;

	struct berval nnewSuperior = BER_BVNULL;

	ber_len_t	length;

	Debug( LDAP_DEBUG_TRACE, "%s do_modrdn\n",
			op->o_log_prefix, 0, 0 );
	/*
	 * Parse the modrdn request.  It looks like this:
	 *
	 *	ModifyRDNRequest := SEQUENCE {
	 *		entry	DistinguishedName,
	 *		newrdn	RelativeDistinguishedName
	 *		deleteoldrdn	BOOLEAN,
	 *		newSuperior	[0] LDAPDN OPTIONAL (v3 Only!)
	 *	}
	 */

	if ( ber_scanf( op->o_ber, "{mmb", &dn, &newrdn, &deloldrdn )
	    == LBER_ERROR )
	{
		Debug( LDAP_DEBUG_ANY, "%s do_modrdn: ber_scanf failed\n",
			op->o_log_prefix, 0, 0 );
		send_ldap_discon( op, rs, LDAP_PROTOCOL_ERROR, "decoding error" );
		return SLAPD_DISCONNECT;
	}

	/* Check for newSuperior parameter, if present scan it */

	if ( ber_peek_tag( op->o_ber, &length ) == LDAP_TAG_NEWSUPERIOR ) {
		if ( op->o_protocol < LDAP_VERSION3 ) {
			/* Connection record indicates v2 but field 
			 * newSuperior is present: report error.
			 */
			Debug( LDAP_DEBUG_ANY,
				"%s do_modrdn: newSuperior requires LDAPv3\n",
				op->o_log_prefix, 0, 0 );

			send_ldap_discon( op, rs,
				LDAP_PROTOCOL_ERROR, "newSuperior requires LDAPv3" );
			rs->sr_err = SLAPD_DISCONNECT;
			goto cleanup;
		}

		if ( ber_scanf( op->o_ber, "m", &newSuperior ) 
		     == LBER_ERROR ) {

			Debug( LDAP_DEBUG_ANY, "%s do_modrdn: ber_scanf(\"m\") failed\n",
				op->o_log_prefix, 0, 0 );

			send_ldap_discon( op, rs,
				LDAP_PROTOCOL_ERROR, "decoding error" );
			rs->sr_err = SLAPD_DISCONNECT;
			goto cleanup;
		}
		op->orr_newSup = &pnewSuperior;
		op->orr_nnewSup = &nnewSuperior;
	}

	Debug( LDAP_DEBUG_ARGS,
	    "do_modrdn: dn (%s) newrdn (%s) newsuperior (%s)\n",
		dn.bv_val, newrdn.bv_val,
		newSuperior.bv_len ? newSuperior.bv_val : "" );

	if ( ber_scanf( op->o_ber, /*{*/ "}") == LBER_ERROR ) {
		Debug( LDAP_DEBUG_ANY, "%s do_modrdn: ber_scanf failed\n",
			op->o_log_prefix, 0, 0 );
		send_ldap_discon( op, rs,
			LDAP_PROTOCOL_ERROR, "decoding error" );
		rs->sr_err = SLAPD_DISCONNECT;
		goto cleanup;
	}

	if( get_ctrls( op, rs, 1 ) != LDAP_SUCCESS ) {
		Debug( LDAP_DEBUG_ANY, "%s do_modrdn: get_ctrls failed\n",
			op->o_log_prefix, 0, 0 );
		/* get_ctrls has sent results.	Now clean up. */
		goto cleanup;
	} 

	rs->sr_err = dnPrettyNormal( NULL, &dn, &op->o_req_dn, &op->o_req_ndn, op->o_tmpmemctx );
	if( rs->sr_err != LDAP_SUCCESS ) {
		Debug( LDAP_DEBUG_ANY, "%s do_modrdn: invalid dn (%s)\n",
			op->o_log_prefix, dn.bv_val, 0 );
		send_ldap_error( op, rs, LDAP_INVALID_DN_SYNTAX, "invalid DN" );
		goto cleanup;
	}

	/* FIXME: should have/use rdnPretty / rdnNormalize routines */

	rs->sr_err = dnPrettyNormal( NULL, &newrdn, &op->orr_newrdn, &op->orr_nnewrdn, op->o_tmpmemctx );
	if( rs->sr_err != LDAP_SUCCESS ) {
		Debug( LDAP_DEBUG_ANY, "%s do_modrdn: invalid newrdn (%s)\n",
			op->o_log_prefix, newrdn.bv_val, 0 );
		send_ldap_error( op, rs, LDAP_INVALID_DN_SYNTAX, "invalid new RDN" );
		goto cleanup;
	}

	if( rdn_validate( &op->orr_newrdn ) != LDAP_SUCCESS ) {
		Debug( LDAP_DEBUG_ANY, "%s do_modrdn: invalid rdn (%s)\n",
			op->o_log_prefix, op->orr_newrdn.bv_val, 0 );
		send_ldap_error( op, rs, LDAP_INVALID_DN_SYNTAX, "invalid new RDN" );
		goto cleanup;
	}

	if( op->orr_newSup ) {
		rs->sr_err = dnPrettyNormal( NULL, &newSuperior, &pnewSuperior,
			&nnewSuperior, op->o_tmpmemctx );
		if( rs->sr_err != LDAP_SUCCESS ) {
			Debug( LDAP_DEBUG_ANY,
				"%s do_modrdn: invalid newSuperior (%s)\n",
				op->o_log_prefix, newSuperior.bv_val, 0 );
			send_ldap_error( op, rs, LDAP_INVALID_DN_SYNTAX, "invalid newSuperior" );
			goto cleanup;
		}
	}

	Statslog( LDAP_DEBUG_STATS, "%s MODRDN dn=\"%s\"\n",
	    op->o_log_prefix, op->o_req_dn.bv_val, 0, 0, 0 );

	op->orr_deleteoldrdn = deloldrdn;
	op->orr_modlist = NULL;

	/* prepare modlist of modifications from old/new RDN */
	rs->sr_err = slap_modrdn2mods( op, rs );
	if ( rs->sr_err != LDAP_SUCCESS ) {
		send_ldap_result( op, rs );
		goto cleanup;
	}

	op->o_bd = frontendDB;
	rs->sr_err = frontendDB->be_modrdn( op, rs );

#ifdef LDAP_X_TXN
	if( rs->sr_err == LDAP_X_TXN_SPECIFY_OKAY ) {
		/* skip cleanup */
	}
#endif

cleanup:
	op->o_tmpfree( op->o_req_dn.bv_val, op->o_tmpmemctx );
	op->o_tmpfree( op->o_req_ndn.bv_val, op->o_tmpmemctx );

	op->o_tmpfree( op->orr_newrdn.bv_val, op->o_tmpmemctx );	
	op->o_tmpfree( op->orr_nnewrdn.bv_val, op->o_tmpmemctx );	

	if ( op->orr_modlist != NULL )
		slap_mods_free( op->orr_modlist, 1 );

	if ( !BER_BVISNULL( &pnewSuperior ) ) {
		op->o_tmpfree( pnewSuperior.bv_val, op->o_tmpmemctx );
	}
	if ( !BER_BVISNULL( &nnewSuperior ) ) {
		op->o_tmpfree( nnewSuperior.bv_val, op->o_tmpmemctx );
	}

	return rs->sr_err;
}