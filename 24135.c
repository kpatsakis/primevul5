static void rpa_server_response(struct rpa_auth_request *request,
				unsigned char digest[STATIC_ARRAY MD5_RESULTLEN])
{
	struct md5_context ctx;
	unsigned char tmp[MD5_RESULTLEN];
	unsigned char z[48];
	unsigned int i;

	memset(z, 0, sizeof(z));

	md5_init(&ctx);
	md5_update(&ctx, request->pwd_md5, sizeof(request->pwd_md5));
	md5_update(&ctx, z, sizeof(z));
	md5_update(&ctx, request->service_ucs2be, request->service_len);
	md5_update(&ctx, request->username_ucs2be, request->username_len);
	md5_update(&ctx, request->realm_ucs2be, request->realm_len);
	md5_update(&ctx, request->service_challenge, RPA_SCHALLENGE_LEN);
	md5_update(&ctx, request->user_challenge, request->user_challenge_len);
	md5_update(&ctx, request->service_timestamp, RPA_TIMESTAMP_LEN);
	md5_update(&ctx, request->pwd_md5, sizeof(request->pwd_md5));
	md5_final(&ctx, tmp);

	for (i = 0; i < sizeof(tmp); i++)
		tmp[i] = request->session_key[i] ^ tmp[i];

	md5_init(&ctx);
	md5_update(&ctx, request->pwd_md5, sizeof(request->pwd_md5));
	md5_update(&ctx, z, sizeof(z));
	md5_update(&ctx, request->service_ucs2be, request->service_len);
	md5_update(&ctx, request->username_ucs2be, request->username_len);
	md5_update(&ctx, request->realm_ucs2be, request->realm_len);
	md5_update(&ctx, request->session_key, sizeof(request->session_key));
	md5_update(&ctx, request->service_challenge, RPA_SCHALLENGE_LEN);
	md5_update(&ctx, request->user_challenge, request->user_challenge_len);
	md5_update(&ctx, request->service_timestamp, RPA_TIMESTAMP_LEN);
	md5_update(&ctx, tmp, sizeof(tmp));
	md5_update(&ctx, request->pwd_md5, sizeof(request->pwd_md5));
	md5_final(&ctx, digest);
}