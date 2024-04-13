p11_rpc_buffer_decode_uint32 (unsigned char* ptr)
{
	uint32_t val = (uint32_t) ptr[0] << 24 | ptr[1] << 16 | ptr[2] << 8 | ptr[3];
	return val;
}