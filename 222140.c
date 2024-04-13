evbuffer_chain_free(struct evbuffer_chain *chain)
{
	if (CHAIN_PINNED(chain)) {
		chain->flags |= EVBUFFER_DANGLING;
		return;
	}
	if (chain->flags & (EVBUFFER_MMAP|EVBUFFER_SENDFILE|
		EVBUFFER_REFERENCE)) {
		if (chain->flags & EVBUFFER_REFERENCE) {
			struct evbuffer_chain_reference *info =
			    EVBUFFER_CHAIN_EXTRA(
				    struct evbuffer_chain_reference,
				    chain);
			if (info->cleanupfn)
				(*info->cleanupfn)(chain->buffer,
				    chain->buffer_len,
				    info->extra);
		}
#ifdef _EVENT_HAVE_MMAP
		if (chain->flags & EVBUFFER_MMAP) {
			struct evbuffer_chain_fd *info =
			    EVBUFFER_CHAIN_EXTRA(struct evbuffer_chain_fd,
				chain);
			if (munmap(chain->buffer, chain->buffer_len) == -1)
				event_warn("%s: munmap failed", __func__);
			if (close(info->fd) == -1)
				event_warn("%s: close(%d) failed",
				    __func__, info->fd);
		}
#endif
#ifdef USE_SENDFILE
		if (chain->flags & EVBUFFER_SENDFILE) {
			struct evbuffer_chain_fd *info =
			    EVBUFFER_CHAIN_EXTRA(struct evbuffer_chain_fd,
				chain);
			if (close(info->fd) == -1)
				event_warn("%s: close(%d) failed",
				    __func__, info->fd);
		}
#endif
	}

	mm_free(chain);
}