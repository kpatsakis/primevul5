p11_rpc_buffer_add_attribute (p11_buffer *buffer, const CK_ATTRIBUTE *attr)
{
	unsigned char validity;
	p11_rpc_attribute_serializer *serializer;
	p11_rpc_value_type value_type;

	/* The attribute type */
	if (attr->type > UINT32_MAX) {
		p11_buffer_fail (buffer);
		return;
	}
	p11_rpc_buffer_add_uint32 (buffer, attr->type);

	/* Write out the attribute validity */
	validity = (((CK_LONG)attr->ulValueLen) == -1) ? 0 : 1;
	p11_rpc_buffer_add_byte (buffer, validity);

	if (!validity)
		return;

	/* The attribute length */
	if (attr->ulValueLen > UINT32_MAX) {
		p11_buffer_fail (buffer);
		return;
	}
	p11_rpc_buffer_add_uint32 (buffer, attr->ulValueLen);

	/* The attribute value */
	value_type = map_attribute_to_value_type (attr->type);
	assert (value_type < ELEMS (p11_rpc_attribute_serializers));
	serializer = &p11_rpc_attribute_serializers[value_type];
	assert (serializer != NULL);
	serializer->encode (buffer, attr->pValue, attr->ulValueLen);
}