p11_rpc_buffer_encode_uint32 (unsigned char* data,
                          uint32_t value)
{
	data[0] = (value >> 24) & 0xff;
	data[1] = (value >> 16) & 0xff;
	data[2] = (value >> 8) & 0xff;
	data[3] = (value >> 0) & 0xff;
}