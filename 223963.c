vmod_copy(VRT_CTX, VCL_HEADER src, VCL_HEADER dst)
{
	struct http *src_hp;

	CHECK_OBJ_NOTNULL(ctx, VRT_CTX_MAGIC);

	src_hp = VRT_selecthttp(ctx, src->where);
	header_http_cphdr(ctx, src_hp, src->what, dst);
}