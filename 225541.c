static int gss_spnegomechglue_init(void)
{
	struct gss_mech_config mech_spnego;

	memset(&mech_spnego, 0, sizeof(mech_spnego));
	mech_spnego.mech = &spnego_mechanism;
	mech_spnego.mechNameStr = "spnego";
	mech_spnego.mech_type = GSS_C_NO_OID;

	return gssint_register_mechinfo(&mech_spnego);
}