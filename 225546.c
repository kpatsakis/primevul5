spnego_gss_release_any_name_mapping(OM_uint32 *minor_status,
				    gss_name_t name,
				    gss_buffer_t type_id,
				    gss_any_t *input)
{
	OM_uint32 ret;
	ret = gss_release_any_name_mapping(minor_status,
					   name,
					   type_id,
					   input);
	return (ret);
}