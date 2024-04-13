lzw_result lzw_context_create(struct lzw_ctx **ctx)
{
	struct lzw_ctx *c = malloc(sizeof(*c));
	if (c == NULL) {
		return LZW_NO_MEM;
	}

	*ctx = c;
	return LZW_OK;
}