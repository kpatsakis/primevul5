ecryptfs_decode_from_filename(unsigned char *dst, size_t *dst_size,
			      const unsigned char *src, size_t src_size)
{
	u8 current_bit_offset = 0;
	size_t src_byte_offset = 0;
	size_t dst_byte_offset = 0;

	if (dst == NULL) {
		/* Not exact; conservatively long. Every block of 4
		 * encoded characters decodes into a block of 3
		 * decoded characters. This segment of code provides
		 * the caller with the maximum amount of allocated
		 * space that @dst will need to point to in a
		 * subsequent call. */
		(*dst_size) = (((src_size + 1) * 3) / 4);
		goto out;
	}
	while (src_byte_offset < src_size) {
		unsigned char src_byte =
				filename_rev_map[(int)src[src_byte_offset]];

		switch (current_bit_offset) {
		case 0:
			dst[dst_byte_offset] = (src_byte << 2);
			current_bit_offset = 6;
			break;
		case 6:
			dst[dst_byte_offset++] |= (src_byte >> 4);
			dst[dst_byte_offset] = ((src_byte & 0xF)
						 << 4);
			current_bit_offset = 4;
			break;
		case 4:
			dst[dst_byte_offset++] |= (src_byte >> 2);
			dst[dst_byte_offset] = (src_byte << 6);
			current_bit_offset = 2;
			break;
		case 2:
			dst[dst_byte_offset++] |= (src_byte);
			dst[dst_byte_offset] = 0;
			current_bit_offset = 0;
			break;
		}
		src_byte_offset++;
	}
	(*dst_size) = dst_byte_offset;
out:
	return;
}