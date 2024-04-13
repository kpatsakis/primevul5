spnego_gss_display_name_ext(OM_uint32 *minor_status,
			    gss_name_t name,
			    gss_OID display_as_name_type,
			    gss_buffer_t display_name)
{
	OM_uint32 ret;
	ret = gss_display_name_ext(minor_status,
				   name,
				   display_as_name_type,
				   display_name);
	return (ret);
}