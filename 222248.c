p11_rpc_buffer_add_byte (p11_buffer *buf,
                         unsigned char value)
{
	p11_buffer_add (buf, &value, 1);
}