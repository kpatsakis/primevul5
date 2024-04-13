evbuffer_chain_insert_new(struct evbuffer *buf, size_t datlen)
{
	struct evbuffer_chain *chain;
	if ((chain = evbuffer_chain_new(datlen)) == NULL)
		return NULL;
	evbuffer_chain_insert(buf, chain);
	return chain;
}