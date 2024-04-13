spnego_gss_duplicate_name(
			OM_uint32 *minor_status,
			const gss_name_t input_name,
			gss_name_t *output_name)
{
	OM_uint32 status;

	dsyslog("Entering duplicate_name\n");

	status = gss_duplicate_name(minor_status, input_name, output_name);

	dsyslog("Leaving duplicate_name\n");
	return (status);
}