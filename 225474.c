spnego_gss_delete_name_attribute(OM_uint32 *minor_status,
				 gss_name_t name,
				 gss_buffer_t attr)
{
	OM_uint32 ret;
	ret = gss_delete_name_attribute(minor_status,
					name,
					attr);
	return (ret);
}