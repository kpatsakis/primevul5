evbuffer_strspn(
	struct evbuffer_ptr *ptr, const char *chrset)
{
	int count = 0;
	struct evbuffer_chain *chain = ptr->_internal.chain;
	size_t i = ptr->_internal.pos_in_chain;

	if (!chain)
		return -1;

	while (1) {
		char *buffer = (char *)chain->buffer + chain->misalign;
		for (; i < chain->off; ++i) {
			const char *p = chrset;
			while (*p) {
				if (buffer[i] == *p++)
					goto next;
			}
			ptr->_internal.chain = chain;
			ptr->_internal.pos_in_chain = i;
			ptr->pos += count;
			return count;
		next:
			++count;
		}
		i = 0;

		if (! chain->next) {
			ptr->_internal.chain = chain;
			ptr->_internal.pos_in_chain = i;
			ptr->pos += count;
			return count;
		}

		chain = chain->next;
	}
}