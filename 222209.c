p11_rpc_buffer_encode_uint16 (unsigned char* data,
                              uint16_t value)
{
	data[0] = (value >> 8) & 0xff;
	data[1] = (value >> 0) & 0xff;
}