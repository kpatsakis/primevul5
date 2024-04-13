p11_rpc_buffer_new (size_t reserve)
{
	return p11_rpc_buffer_new_full (reserve, log_allocator, free);
}