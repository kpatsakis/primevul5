spnego_gss_release_name(
			OM_uint32 *minor_status,
			gss_name_t *input_name)
{
	OM_uint32 status;

	dsyslog("Entering release_name\n");

	status = gss_release_name(minor_status, input_name);

	dsyslog("Leaving release_name\n");
	return (status);
}