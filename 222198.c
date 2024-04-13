p11_rpc_mechanism_is_supported (CK_MECHANISM_TYPE mech)
{
	if (mechanism_has_no_parameters (mech) ||
	    mechanism_has_sane_parameters (mech))
		return true;
	return false;
}