p11_rpc_buffer_decode_uint16 (unsigned char* data)
{
	uint16_t value = data[0] << 8 | data[1];
	return value;
}