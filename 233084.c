iasecc_erase_binary(struct sc_card *card, unsigned int offs, size_t count, unsigned long flags)
{
	struct sc_context *ctx = card->ctx;
	unsigned char *tmp = NULL;
	int rv;

	LOG_FUNC_CALLED(ctx);
	sc_log(ctx,
	       "iasecc_erase_binary(card:%p) count %"SC_FORMAT_LEN_SIZE_T"u",
	       card, count);
	if (!count)
		LOG_TEST_RET(ctx, SC_ERROR_INVALID_ARGUMENTS, "'ERASE BINARY' failed: invalid size to erase");

	tmp = malloc(count);
	if (!tmp)
		LOG_TEST_RET(ctx, SC_ERROR_OUT_OF_MEMORY, "Cannot allocate temporary buffer");
	memset(tmp, 0xFF, count);

	rv = sc_update_binary(card, offs, tmp, count, flags);
	free(tmp);
	LOG_TEST_RET(ctx, rv, "iasecc_erase_binary() update binary error");
	LOG_FUNC_RETURN(ctx, rv);
}