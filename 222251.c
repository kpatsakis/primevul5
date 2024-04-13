p11_rpc_buffer_add_mechanism (p11_buffer *buffer, const CK_MECHANISM *mech)
{
	p11_rpc_mechanism_serializer *serializer = NULL;
	size_t i;

	/* The mechanism type */
	p11_rpc_buffer_add_uint32 (buffer, mech->mechanism);

	if (mechanism_has_no_parameters (mech->mechanism)) {
		p11_rpc_buffer_add_byte_array (buffer, NULL, 0);
		return;
	}

	assert (mechanism_has_sane_parameters (mech->mechanism));

	for (i = 0; i < ELEMS (p11_rpc_mechanism_serializers); i++) {
		if (p11_rpc_mechanism_serializers[i].type == mech->mechanism) {
			serializer = &p11_rpc_mechanism_serializers[i];
			break;
		}
	}

	if (serializer == NULL)
		serializer = &p11_rpc_byte_array_mechanism_serializer;

	serializer->encode (buffer, mech->pParameter, mech->ulParameterLen);
}