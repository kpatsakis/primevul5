p11_rpc_buffer_free (p11_buffer *buf)
{
	if (buf == NULL)
		return;

	p11_buffer_uninit (buf);
	free (buf);
}