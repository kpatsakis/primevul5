static struct auth_request *mech_rpa_auth_new(void)
{
	struct rpa_auth_request *request;
	pool_t pool;

	pool = pool_alloconly_create(MEMPOOL_GROWING"rpa_auth_request", 2048);
	request = p_new(pool, struct rpa_auth_request, 1);
	request->pool = pool;
	request->phase = 0;

	request->auth_request.pool = pool;
	return &request->auth_request;
}