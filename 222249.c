p11_rpc_buffer_get_uint64 (p11_buffer *buf,
                           size_t *offset,
                           uint64_t *value)
{
	size_t off = *offset;
	uint32_t a, b;
	if (!p11_rpc_buffer_get_uint32 (buf, &off, &a) ||
	    !p11_rpc_buffer_get_uint32 (buf, &off, &b))
		return false;
	if (value != NULL)
		*value = ((uint64_t)a) << 32 | b;
	*offset = off;
	return true;
}