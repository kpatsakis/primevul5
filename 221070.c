static void ep_busy_loop(struct eventpoll *ep, int nonblock)
{
	unsigned int napi_id = READ_ONCE(ep->napi_id);

	if ((napi_id >= MIN_NAPI_ID) && net_busy_loop_on())
		napi_busy_loop(napi_id, nonblock ? NULL : ep_busy_loop_end, ep);
}