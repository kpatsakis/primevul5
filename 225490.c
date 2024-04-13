spnego_gss_import_name(
		    OM_uint32 *minor_status,
		    gss_buffer_t input_name_buffer,
		    gss_OID input_name_type,
		    gss_name_t *output_name)
{
	OM_uint32 status;

	dsyslog("Entering import_name\n");

	status = gss_import_name(minor_status, input_name_buffer,
			input_name_type, output_name);

	dsyslog("Leaving import_name\n");
	return (status);
}