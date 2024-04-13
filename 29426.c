static unsigned get_bits(bunzip_data *bd, int bits_wanted)
{
	unsigned bits = 0;
	/* Cache bd->inbufBitCount in a CPU register (hopefully): */
	int bit_count = bd->inbufBitCount;

	/* If we need to get more data from the byte buffer, do so.  (Loop getting
	   one byte at a time to enforce endianness and avoid unaligned access.) */
	while (bit_count < bits_wanted) {

		/* If we need to read more data from file into byte buffer, do so */
		if (bd->inbufPos == bd->inbufCount) {
			/* if "no input fd" case: in_fd == -1, read fails, we jump */
			bd->inbufCount = read(bd->in_fd, bd->inbuf, IOBUF_SIZE);
			if (bd->inbufCount <= 0)
				longjmp(bd->jmpbuf, RETVAL_UNEXPECTED_INPUT_EOF);
			bd->inbufPos = 0;
		}

		/* Avoid 32-bit overflow (dump bit buffer to top of output) */
		if (bit_count >= 24) {
			bits = bd->inbufBits & ((1U << bit_count) - 1);
			bits_wanted -= bit_count;
			bits <<= bits_wanted;
			bit_count = 0;
		}

		/* Grab next 8 bits of input from buffer. */
		bd->inbufBits = (bd->inbufBits << 8) | bd->inbuf[bd->inbufPos++];
		bit_count += 8;
	}

	/* Calculate result */
	bit_count -= bits_wanted;
	bd->inbufBitCount = bit_count;
	bits |= (bd->inbufBits >> bit_count) & ((1 << bits_wanted) - 1);

	return bits;
}
