vmod_remove(VRT_CTX, VCL_HEADER hdr, VCL_REGEX re)
{
	struct http *hp;

	CHECK_OBJ_NOTNULL(ctx, VRT_CTX_MAGIC);
	AN(re);

	hp = VRT_selecthttp(ctx, hdr->where);
	header_http_Unset(ctx, hp, hdr->what, re);
}