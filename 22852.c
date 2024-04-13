lzw_result lzw_decode_init(
		struct lzw_ctx *ctx,
		const uint8_t *compressed_data,
		uint64_t compressed_data_len,
		uint64_t compressed_data_pos,
		uint8_t code_size,
		const uint8_t ** const stack_base_out,
		const uint8_t ** const stack_pos_out)
{
	struct lzw_dictionary_entry *table = ctx->table;

	/* Initialise the input reading context */
	ctx->input.data = compressed_data;
	ctx->input.data_len = compressed_data_len;
	ctx->input.data_sb_next = compressed_data_pos;

	ctx->input.sb_bit = 0;
	ctx->input.sb_bit_count = 0;

	/* Initialise the dictionary building context */
	ctx->initial_code_size = code_size;

	ctx->clear_code = (1 << code_size) + 0;
	ctx->eoi_code   = (1 << code_size) + 1;

	/* Initialise the standard dictionary entries */
	for (uint32_t i = 0; i < ctx->clear_code; ++i) {
		table[i].first_value = i;
		table[i].last_value  = i;
	}

	*stack_base_out = ctx->stack_base;
	return lzw__clear_codes(ctx, stack_pos_out);
}