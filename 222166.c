evbuffer_getchr(struct evbuffer_ptr *it)
{
	struct evbuffer_chain *chain = it->_internal.chain;
	size_t off = it->_internal.pos_in_chain;

	return chain->buffer[chain->misalign + off];
}