GetCode_(gdIOCtx *fd, CODE_STATIC_DATA *scd, int code_size, int flag, int *ZeroDataBlockP)
{
	int i, j, ret;
	unsigned char count;

	if(flag) {
		scd->curbit = 0;
		scd->lastbit = 0;
		scd->last_byte = 0;
		scd->done = FALSE;
		return 0;
	}

	if((scd->curbit + code_size) >= scd->lastbit) {
		if(scd->done) {
			if(scd->curbit >= scd->lastbit) {
				/* Oh well */
			}
			return -1;
		}

		scd->buf[0] = scd->buf[scd->last_byte - 2];
		scd->buf[1] = scd->buf[scd->last_byte - 1];

		if((count = GetDataBlock(fd, &scd->buf[2], ZeroDataBlockP)) <= 0) {
			scd->done = TRUE;
		}

		scd->last_byte = 2 + count;
		scd->curbit = (scd->curbit - scd->lastbit) + 16;
		scd->lastbit = (2 + count) * 8;
	}

	ret = 0;
	for (i = scd->curbit, j = 0; j < code_size; ++i, ++j) {
		if (i < CSD_BUF_SIZE * 8) {
			ret |= ((scd->buf[i / 8] & (1 << (i % 8))) != 0) << j;
		} else {
			ret = -1;
			break;
		}
	}

	scd->curbit += code_size;

	return ret;
}