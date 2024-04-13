static void print_pack_info(struct cgit_context *ctx)
{
	struct packed_git *pack;
	int ofs;

	ctx->page.mimetype = "text/plain";
	ctx->page.filename = "objects/info/packs";
	cgit_print_http_headers(ctx);
	ofs = strlen(ctx->repo->path) + strlen("/objects/pack/");
	prepare_packed_git();
	for (pack = packed_git; pack; pack = pack->next)
		if (pack->pack_local)
			htmlf("P %s\n", pack->pack_name + ofs);
}