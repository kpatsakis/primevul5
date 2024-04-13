
static int io_unregister_personality(struct io_ring_ctx *ctx, unsigned id)
{
	const struct cred *creds;

	creds = xa_erase(&ctx->personalities, id);
	if (creds) {
		put_cred(creds);
		return 0;
	}

	return -EINVAL;