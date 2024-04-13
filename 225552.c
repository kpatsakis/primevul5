spnego_gss_get_name_attribute(OM_uint32 *minor_status,
			      gss_name_t name,
			      gss_buffer_t attr,
			      int *authenticated,
			      int *complete,
			      gss_buffer_t value,
			      gss_buffer_t display_value,
			      int *more)
{
	OM_uint32 ret;
	ret = gss_get_name_attribute(minor_status,
				     name,
				     attr,
				     authenticated,
				     complete,
				     value,
				     display_value,
				     more);
	return (ret);
}