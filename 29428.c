int FAST_FUNC read_bunzip(bunzip_data *bd, char *outbuf, int len)
{
	const uint32_t *dbuf;
	int pos, current, previous;
	uint32_t CRC;

	/* If we already have error/end indicator, return it */
	if (bd->writeCount < 0)
		return bd->writeCount;

	dbuf = bd->dbuf;

	/* Register-cached state (hopefully): */
	pos = bd->writePos;
	current = bd->writeCurrent;
	CRC = bd->writeCRC; /* small loss on x86-32 (not enough regs), win on x86-64 */

	/* We will always have pending decoded data to write into the output
	   buffer unless this is the very first call (in which case we haven't
	   Huffman-decoded a block into the intermediate buffer yet). */
	if (bd->writeCopies) {

 dec_writeCopies:
		/* Inside the loop, writeCopies means extra copies (beyond 1) */
		--bd->writeCopies;

		/* Loop outputting bytes */
		for (;;) {

			/* If the output buffer is full, save cached state and return */
			if (--len < 0) {
				/* Unlikely branch.
				 * Use of "goto" instead of keeping code here
				 * helps compiler to realize this. */
				goto outbuf_full;
			}

			/* Write next byte into output buffer, updating CRC */
			*outbuf++ = current;
			CRC = (CRC << 8) ^ bd->crc32Table[(CRC >> 24) ^ current];

			/* Loop now if we're outputting multiple copies of this byte */
			if (bd->writeCopies) {
				/* Unlikely branch */
				/*--bd->writeCopies;*/
				/*continue;*/
				/* Same, but (ab)using other existing --writeCopies operation
				 * (and this if() compiles into just test+branch pair): */
				goto dec_writeCopies;
			}
 decode_next_byte:
			if (--bd->writeCount < 0)
				break; /* input block is fully consumed, need next one */

			/* Follow sequence vector to undo Burrows-Wheeler transform */
			previous = current;
			pos = dbuf[pos];
			current = (uint8_t)pos;
			pos >>= 8;

			/* After 3 consecutive copies of the same byte, the 4th
			 * is a repeat count.  We count down from 4 instead
			 * of counting up because testing for non-zero is faster */
			if (--bd->writeRunCountdown != 0) {
				if (current != previous)
					bd->writeRunCountdown = 4;
			} else {
				/* Unlikely branch */
				/* We have a repeated run, this byte indicates the count */
				bd->writeCopies = current;
				current = previous;
				bd->writeRunCountdown = 5;

				/* Sometimes there are just 3 bytes (run length 0) */
				if (!bd->writeCopies) goto decode_next_byte;

				/* Subtract the 1 copy we'd output anyway to get extras */
				--bd->writeCopies;
			}
		} /* for(;;) */

		/* Decompression of this input block completed successfully */
		bd->writeCRC = CRC = ~CRC;
		bd->totalCRC = ((bd->totalCRC << 1) | (bd->totalCRC >> 31)) ^ CRC;

		/* If this block had a CRC error, force file level CRC error */
		if (CRC != bd->headerCRC) {
			bd->totalCRC = bd->headerCRC + 1;
			return RETVAL_LAST_BLOCK;
		}
	}

	/* Refill the intermediate buffer by Huffman-decoding next block of input */
	{
		int r = get_next_block(bd);
		if (r) { /* error/end */
			bd->writeCount = r;
			return (r != RETVAL_LAST_BLOCK) ? r : len;
		}
	}

	CRC = ~0;
	pos = bd->writePos;
	current = bd->writeCurrent;
	goto decode_next_byte;

 outbuf_full:
	/* Output buffer is full, save cached state and return */
	bd->writePos = pos;
	bd->writeCurrent = current;
	bd->writeCRC = CRC;

	bd->writeCopies++;

	return 0;
}
