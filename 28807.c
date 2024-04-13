int iconvbufs(iconv_t ic, xbuf *in, xbuf *out, int flags)
{
	ICONV_CONST char *ibuf;
	size_t icnt, ocnt, opos;
	char *obuf;

	if (!out->size && flags & ICB_EXPAND_OUT) {
		size_t siz = ROUND_UP_1024(in->len * 2);
		alloc_xbuf(out, siz);
	} else if (out->len+1 >= out->size) {
		/* There is no room to even start storing data. */
		if (!(flags & ICB_EXPAND_OUT) || flags & ICB_CIRCULAR_OUT) {
			errno = E2BIG;
			return -1;
		}
		realloc_xbuf(out, out->size + ROUND_UP_1024(in->len * 2));
	}

	if (flags & ICB_INIT)
		iconv(ic, NULL, 0, NULL, 0);

	ibuf = in->buf + in->pos;
	icnt = in->len;

	opos = out->pos + out->len;
	if (flags & ICB_CIRCULAR_OUT) {
		if (opos >= out->size) {
			opos -= out->size;
			/* We know that out->pos is not 0 due to the "no room" check
			 * above, so this can't go "negative". */
			ocnt = out->pos - opos - 1;
		} else {
			/* Allow the use of all bytes to the physical end of the buffer
			 * unless pos is 0, in which case we reserve our trailing '\0'. */
			ocnt = out->size - opos - (out->pos ? 0 : 1);
		}
	} else
		ocnt = out->size - opos - 1;
	obuf = out->buf + opos;

	while (icnt) {
		while (iconv(ic, &ibuf, &icnt, &obuf, &ocnt) == (size_t)-1) {
			if (errno == EINTR)
				continue;
			if (errno == EINVAL) {
				if (!(flags & ICB_INCLUDE_INCOMPLETE))
					goto finish;
				if (!ocnt)
					goto e2big;
			} else if (errno == EILSEQ) {
				if (!(flags & ICB_INCLUDE_BAD))
					goto finish;
				if (!ocnt)
					goto e2big;
			} else if (errno == E2BIG) {
				size_t siz;
			  e2big:
				opos = obuf - out->buf;
				if (flags & ICB_CIRCULAR_OUT && out->pos > 1 && opos > out->pos) {
					/* We are in a divided circular buffer at the physical
					 * end with room to wrap to the start.  If iconv() refused
					 * to use one or more trailing bytes in the buffer, we
					 * set the size to ignore the unused bytes. */
					if (opos < out->size)
						reduce_iobuf_size(out, opos);
					obuf = out->buf;
					ocnt = out->pos - 1;
					continue;
				}
				if (!(flags & ICB_EXPAND_OUT) || flags & ICB_CIRCULAR_OUT) {
					errno = E2BIG;
					goto finish;
				}
				siz = ROUND_UP_1024(in->len * 2);
				realloc_xbuf(out, out->size + siz);
				obuf = out->buf + opos;
				ocnt += siz;
				continue;
			} else {
				rsyserr(FERROR, errno, "unexpected error from iconv()");
				exit_cleanup(RERR_UNSUPPORTED);
			}
			*obuf++ = *ibuf++;
			ocnt--, icnt--;
			if (!icnt)
				break;
		}
	}

	errno = 0;

  finish:
	opos = obuf - out->buf;
	if (flags & ICB_CIRCULAR_OUT && opos < out->pos)
		opos += out->size;
	out->len = opos - out->pos;

	in->len = icnt;
	in->pos = ibuf - in->buf;

	return errno ? -1 : 0;
}
