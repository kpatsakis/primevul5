void FLAC__bitreader_dump(const FLAC__BitReader *br, FILE *out)
{
	uint32_t i, j;
	if(br == 0) {
		fprintf(out, "bitreader is NULL\n");
	}
	else {
		fprintf(out, "bitreader: capacity=%u words=%u bytes=%u consumed: words=%u, bits=%u\n", br->capacity, br->words, br->bytes, br->consumed_words, br->consumed_bits);

		for(i = 0; i < br->words; i++) {
			fprintf(out, "%08X: ", i);
			for(j = 0; j < FLAC__BITS_PER_WORD; j++)
				if(i < br->consumed_words || (i == br->consumed_words && j < br->consumed_bits))
					fprintf(out, ".");
				else
					fprintf(out, "%01d", br->buffer[i] & ((brword)1 << (FLAC__BITS_PER_WORD-j-1)) ? 1:0);
			fprintf(out, "\n");
		}
		if(br->bytes > 0) {
			fprintf(out, "%08X: ", i);
			for(j = 0; j < br->bytes*8; j++)
				if(i < br->consumed_words || (i == br->consumed_words && j < br->consumed_bits))
					fprintf(out, ".");
				else
					fprintf(out, "%01d", br->buffer[i] & ((brword)1 << (br->bytes*8-j-1)) ? 1:0);
			fprintf(out, "\n");
		}
	}
}