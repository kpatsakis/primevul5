selectwhere(VRT_CTX, VCL_HTTP hp)
{
	CHECK_OBJ_NOTNULL(ctx, VRT_CTX_MAGIC);
	CHECK_OBJ_NOTNULL(hp, HTTP_MAGIC);

	if (hp == ctx->http_req)
		return (HDR_REQ);
	if (hp == ctx->http_req_top)
		return (HDR_REQ_TOP);
	if (hp == ctx->http_bereq)
		return (HDR_BEREQ);
	if (hp == ctx->http_beresp)
		return (HDR_BERESP);
	if (hp ==  ctx->http_resp)
		return (HDR_RESP);
	WRONG("impossible VCL_HTTP");
}