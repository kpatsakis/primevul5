mechanism_has_sane_parameters (CK_MECHANISM_TYPE type)
{
	int i;

	/* This can be set from tests, to override default set of supported */
	if (p11_rpc_mechanisms_override_supported) {
		for (i = 0; p11_rpc_mechanisms_override_supported[i] != 0; i++) {
			if (p11_rpc_mechanisms_override_supported[i] == type)
				return true;
		}

		return false;
	}

	for (i = 0; i < ELEMS(p11_rpc_mechanism_serializers); i++) {
		if (p11_rpc_mechanism_serializers[i].type == type)
			return true;
	}

	return false;
}