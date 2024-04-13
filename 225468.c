spnego_gss_inquire_mech_for_saslname(OM_uint32 *minor_status,
                                     const gss_buffer_t sasl_mech_name,
                                     gss_OID *mech_type)
{
	if (sasl_mech_name->length == SPNEGO_SASL_NAME_LEN &&
	    memcmp(sasl_mech_name->value, SPNEGO_SASL_NAME,
		   SPNEGO_SASL_NAME_LEN) == 0) {
		if (mech_type != NULL)
			*mech_type = (gss_OID)gss_mech_spnego;
		return (GSS_S_COMPLETE);
	}

	return (GSS_S_BAD_MECH);
}