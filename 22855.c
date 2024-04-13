static lzw_result lzw__clear_codes(
		struct lzw_ctx *ctx,
		const uint8_t ** const stack_pos_out)
{
	uint32_t code;
	uint8_t *stack_pos;

	/* Reset dictionary building context */
	ctx->current_code_size = ctx->initial_code_size + 1;
	ctx->current_code_size_max = (1 << ctx->current_code_size) - 1;;
	ctx->current_entry = (1 << ctx->initial_code_size) + 2;

	/* There might be a sequence of clear codes, so process them all */
	do {
		lzw_result res = lzw__next_code(&ctx->input,
				ctx->current_code_size, &code);
		if (res != LZW_OK) {
			return res;
		}
	} while (code == ctx->clear_code);

	/* The initial code must be from the initial dictionary. */
	if (code > ctx->clear_code) {
		return LZW_BAD_ICODE;
	}

	/* Record this initial code as "previous" code, needed during decode. */
	ctx->previous_code = code;
	ctx->previous_code_first = code;

	/* Reset the stack, and add first non-clear code added as first item. */
	stack_pos = ctx->stack_base;
	*stack_pos++ = code;

	*stack_pos_out = stack_pos;
	return LZW_OK;
}