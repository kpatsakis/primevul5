p11_rpc_buffer_add_uint64 (p11_buffer *buffer,
                           uint64_t value)
{
	p11_rpc_buffer_add_uint32 (buffer, ((value >> 32) & 0xffffffff));
	p11_rpc_buffer_add_uint32 (buffer, (value & 0xffffffff));
}