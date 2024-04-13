int gss_spnegoint_lib_init(void)
{
	int err;

	err = k5_key_register(K5_KEY_GSS_SPNEGO_STATUS, NULL);
	if (err)
		return err;

#ifdef _GSS_STATIC_LINK
	return gss_spnegomechglue_init();
#else
	return 0;
#endif
}