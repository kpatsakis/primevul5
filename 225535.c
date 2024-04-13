acc_ctx_vfy_oid(OM_uint32 *minor_status,
		spnego_gss_ctx_id_t sc, gss_OID mechoid,
		OM_uint32 *negState, send_token_flag *tokflag)
{
	OM_uint32 ret, tmpmin;
	gss_mechanism mech = NULL;
	gss_OID_set mech_set = GSS_C_NO_OID_SET;
	int present = 0;

	if (g_OID_equal(sc->internal_mech, mechoid))
		return GSS_S_COMPLETE;

	mech = gssint_get_mechanism(sc->internal_mech);
	if (mech == NULL || mech->gss_indicate_mechs == NULL) {
		*minor_status = ERR_SPNEGO_NEGOTIATION_FAILED;
		map_errcode(minor_status);
		*negState = REJECT;
		*tokflag = ERROR_TOKEN_SEND;
		return GSS_S_BAD_MECH;
	}
	ret = mech->gss_indicate_mechs(minor_status, &mech_set);
	if (ret != GSS_S_COMPLETE) {
		*tokflag = NO_TOKEN_SEND;
		map_error(minor_status, mech);
		goto cleanup;
	}
	ret = gss_test_oid_set_member(minor_status, mechoid,
				      mech_set, &present);
	if (ret != GSS_S_COMPLETE)
		goto cleanup;
	if (!present) {
		*minor_status = ERR_SPNEGO_NEGOTIATION_FAILED;
		map_errcode(minor_status);
		*negState = REJECT;
		*tokflag = ERROR_TOKEN_SEND;
		ret = GSS_S_BAD_MECH;
	}
cleanup:
	gss_release_oid_set(&tmpmin, &mech_set);
	return ret;
}