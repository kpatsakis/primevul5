mech_rpa_auth_free(struct auth_request *auth_request)
{
	struct rpa_auth_request *request =
		(struct rpa_auth_request *)auth_request;

	safe_memset(request->pwd_md5, 0, sizeof(request->pwd_md5));

	pool_unref(&auth_request->pool);
}