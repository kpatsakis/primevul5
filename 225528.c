spnego_gss_inquire_name(OM_uint32 *minor_status,
			gss_name_t name,
			int *name_is_MN,
			gss_OID *MN_mech,
			gss_buffer_set_t *attrs)
{
	OM_uint32 ret;
	ret = gss_inquire_name(minor_status,
			       name,
			       name_is_MN,
			       MN_mech,
			       attrs);
	return (ret);
}