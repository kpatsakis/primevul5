spnego_gss_inquire_saslname_for_mech(OM_uint32 *minor_status,
                                     const gss_OID desired_mech,
                                     gss_buffer_t sasl_mech_name,
                                     gss_buffer_t mech_name,
                                     gss_buffer_t mech_description)
{
	*minor_status = 0;

	if (!g_OID_equal(desired_mech, gss_mech_spnego))
		return (GSS_S_BAD_MECH);

	if (!g_make_string_buffer(SPNEGO_SASL_NAME, sasl_mech_name) ||
	    !g_make_string_buffer("spnego", mech_name) ||
	    !g_make_string_buffer("Simple and Protected GSS-API "
				  "Negotiation Mechanism", mech_description))
		goto fail;

	return (GSS_S_COMPLETE);

fail:
	*minor_status = ENOMEM;
	return (GSS_S_FAILURE);
}