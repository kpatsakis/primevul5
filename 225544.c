spnego_gss_map_name_to_any(OM_uint32 *minor_status,
			   gss_name_t name,
			   int authenticated,
			   gss_buffer_t type_id,
			   gss_any_t *output)
{
	OM_uint32 ret;
	ret = gss_map_name_to_any(minor_status,
				  name,
				  authenticated,
				  type_id,
				  output);
	return (ret);
}