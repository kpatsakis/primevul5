p11_rpc_buffer_get_mechanism (p11_buffer *buffer,
			      size_t *offset,
			      CK_MECHANISM *mech)
{
	uint32_t mechanism;
	p11_rpc_mechanism_serializer *serializer = NULL;
	size_t i;

	/* The mechanism type */
	if (!p11_rpc_buffer_get_uint32 (buffer, offset, &mechanism))
		return false;

	mech->mechanism = mechanism;

	for (i = 0; i < ELEMS (p11_rpc_mechanism_serializers); i++) {
		if (p11_rpc_mechanism_serializers[i].type == mech->mechanism) {
			serializer = &p11_rpc_mechanism_serializers[i];
			break;
		}
	}

	if (serializer == NULL)
		serializer = &p11_rpc_byte_array_mechanism_serializer;

	if (!serializer->decode (buffer, offset,
				 mech->pParameter, &mech->ulParameterLen))
		return false;

	return true;
}