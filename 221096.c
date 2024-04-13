static inline void ep_reset_busy_poll_napi_id(struct eventpoll *ep)
{
	if (ep->napi_id)
		ep->napi_id = 0;
}