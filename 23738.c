int unlzw(in, out)
    int in, out;    /* input and output file descriptors */
{
    REG2   char_type  *stackp;
    REG3   code_int   code;
    REG4   int        finchar;
    REG5   code_int   oldcode;
    REG6   code_int   incode;
    REG7   long       inbits;
    REG8   long       posbits;
    REG9   int        outpos;
/*  REG10  int        insize; (global) */
    REG11  unsigned   bitmask;
    REG12  code_int   free_ent;
    REG13  code_int   maxcode;
    REG14  code_int   maxmaxcode;
    REG15  int        n_bits;
    REG16  int        rsize;

#ifdef MAXSEG_64K
    tab_prefix[0] = tab_prefix0;
    tab_prefix[1] = tab_prefix1;
#endif
    maxbits = get_byte();
    block_mode = maxbits & BLOCK_MODE;
    if ((maxbits & LZW_RESERVED) != 0) {
	WARN((stderr, "\n%s: %s: warning, unknown flags 0x%x\n",
	      program_name, ifname, maxbits & LZW_RESERVED));
    }
    maxbits &= BIT_MASK;
    maxmaxcode = MAXCODE(maxbits);

    if (maxbits > BITS) {
	fprintf(stderr,
		"\n%s: %s: compressed with %d bits, can only handle %d bits\n",
		program_name, ifname, maxbits, BITS);
	exit_code = ERROR;
	return ERROR;
    }
    rsize = insize;
    maxcode = MAXCODE(n_bits = INIT_BITS)-1;
    bitmask = (1<<n_bits)-1;
    oldcode = -1;
    finchar = 0;
    outpos = 0;
    posbits = inptr<<3;

    free_ent = ((block_mode) ? FIRST : 256);

    clear_tab_prefixof(); /* Initialize the first 256 entries in the table. */

    for (code = 255 ; code >= 0 ; --code) {
	tab_suffixof(code) = (char_type)code;
    }
    do {
	REG1 int i;
	int  e;
	int  o;

    resetbuf:
	o = posbits >> 3;
	e = o <= insize ? insize - o : 0;

	for (i = 0 ; i < e ; ++i) {
	    inbuf[i] = inbuf[i+o];
	}
	insize = e;
	posbits = 0;

	if (insize < INBUF_EXTRA) {
	    rsize = read_buffer (in, (char *) inbuf + insize, INBUFSIZ);
	    if (rsize == -1) {
		read_error();
	    }
	    insize += rsize;
	    bytes_in += (off_t)rsize;
	}
	inbits = ((rsize != 0) ? ((long)insize - insize%n_bits)<<3 :
		  ((long)insize<<3)-(n_bits-1));

	while (inbits > posbits) {
	    if (free_ent > maxcode) {
		posbits = ((posbits-1) +
			   ((n_bits<<3)-(posbits-1+(n_bits<<3))%(n_bits<<3)));
		++n_bits;
		if (n_bits == maxbits) {
		    maxcode = maxmaxcode;
		} else {
		    maxcode = MAXCODE(n_bits)-1;
		}
		bitmask = (1<<n_bits)-1;
		goto resetbuf;
	    }
	    input(inbuf,posbits,code,n_bits,bitmask);
	    Tracev((stderr, "%d ", code));

	    if (oldcode == -1) {
		if (256 <= code)
		  gzip_error ("corrupt input.");
		outbuf[outpos++] = (char_type)(finchar = (int)(oldcode=code));
		continue;
	    }
	    if (code == CLEAR && block_mode) {
		clear_tab_prefixof();
		free_ent = FIRST - 1;
		posbits = ((posbits-1) +
			   ((n_bits<<3)-(posbits-1+(n_bits<<3))%(n_bits<<3)));
		maxcode = MAXCODE(n_bits = INIT_BITS)-1;
		bitmask = (1<<n_bits)-1;
		goto resetbuf;
	    }
	    incode = code;
	    stackp = de_stack;

	    if (code >= free_ent) { /* Special case for KwKwK string. */
		if (code > free_ent) {
#ifdef DEBUG
		    char_type *p;

		    posbits -= n_bits;
		    p = &inbuf[posbits>>3];
		    fprintf(stderr,
			    "code:%ld free_ent:%ld n_bits:%d insize:%u\n",
			    code, free_ent, n_bits, insize);
		    fprintf(stderr,
			    "posbits:%ld inbuf:%02X %02X %02X %02X %02X\n",
			    posbits, p[-1],p[0],p[1],p[2],p[3]);
#endif
		    if (!test && outpos > 0) {
			write_buf(out, (char*)outbuf, outpos);
			bytes_out += (off_t)outpos;
		    }
		    gzip_error (to_stdout
				? "corrupt input."
				: "corrupt input. Use zcat to recover some data.");
		}
		*--stackp = (char_type)finchar;
		code = oldcode;
	    }

	    while ((cmp_code_int)code >= (cmp_code_int)256) {
		/* Generate output characters in reverse order */
		*--stackp = tab_suffixof(code);
		code = tab_prefixof(code);
	    }
	    *--stackp =	(char_type)(finchar = tab_suffixof(code));

	    /* And put them out in forward order */
	    {
		REG1 int	i;

		if (outpos+(i = (de_stack-stackp)) >= OUTBUFSIZ) {
		    do {
			if (i > OUTBUFSIZ-outpos) i = OUTBUFSIZ-outpos;

			if (i > 0) {
			    memcpy(outbuf+outpos, stackp, i);
			    outpos += i;
			}
			if (outpos >= OUTBUFSIZ) {
			    if (!test) {
				write_buf(out, (char*)outbuf, outpos);
				bytes_out += (off_t)outpos;
			    }
			    outpos = 0;
			}
			stackp+= i;
		    } while ((i = (de_stack-stackp)) > 0);
		} else {
		    memcpy(outbuf+outpos, stackp, i);
		    outpos += i;
		}
	    }

	    if ((code = free_ent) < maxmaxcode) { /* Generate the new entry. */

		tab_prefixof(code) = (unsigned short)oldcode;
		tab_suffixof(code) = (char_type)finchar;
		free_ent = code+1;
	    }
	    oldcode = incode;	/* Remember previous code.	*/
	}
    } while (rsize != 0);

    if (!test && outpos > 0) {
	write_buf(out, (char*)outbuf, outpos);
	bytes_out += (off_t)outpos;
    }
    return OK;
}