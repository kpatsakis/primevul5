LZWReadByte(gif_source_ptr sinfo)
/* Read an LZW-compressed byte */
{
  register int code;            /* current working code */
  int incode;                   /* saves actual input code */

  /* If any codes are stacked from a previously read symbol, return them */
  if (sinfo->sp > sinfo->symbol_stack)
    return (int)(*(--sinfo->sp));

  /* Time to read a new symbol */
  code = GetCode(sinfo);

  if (code == sinfo->clear_code) {
    /* Reinit state, swallow any extra Clear codes, and */
    /* return next code, which is expected to be a raw byte. */
    ReInitLZW(sinfo);
    do {
      code = GetCode(sinfo);
    } while (code == sinfo->clear_code);
    if (code > sinfo->clear_code) { /* make sure it is a raw byte */
      WARNMS(sinfo->cinfo, JWRN_GIF_BADDATA);
      code = 0;                 /* use something valid */
    }
    /* make firstcode, oldcode valid! */
    sinfo->firstcode = sinfo->oldcode = code;
    return code;
  }

  if (code == sinfo->end_code) {
    /* Skip the rest of the image, unless GetCode already read terminator */
    if (!sinfo->out_of_blocks) {
      SkipDataBlocks(sinfo);
      sinfo->out_of_blocks = TRUE;
    }
    /* Complain that there's not enough data */
    WARNMS(sinfo->cinfo, JWRN_GIF_ENDCODE);
    /* Pad data with 0's */
    return 0;                   /* fake something usable */
  }

  /* Got normal raw byte or LZW symbol */
  incode = code;                /* save for a moment */

  if (code >= sinfo->max_code) { /* special case for not-yet-defined symbol */
    /* code == max_code is OK; anything bigger is bad data */
    if (code > sinfo->max_code) {
      WARNMS(sinfo->cinfo, JWRN_GIF_BADDATA);
      incode = 0;               /* prevent creation of loops in symbol table */
    }
    /* this symbol will be defined as oldcode/firstcode */
    *(sinfo->sp++) = (UINT8)sinfo->firstcode;
    code = sinfo->oldcode;
  }

  /* If it's a symbol, expand it into the stack */
  while (code >= sinfo->clear_code) {
    *(sinfo->sp++) = sinfo->symbol_tail[code]; /* tail is a byte value */
    code = sinfo->symbol_head[code]; /* head is another LZW symbol */
  }
  /* At this point code just represents a raw byte */
  sinfo->firstcode = code;      /* save for possible future use */

  /* If there's room in table... */
  if ((code = sinfo->max_code) < LZW_TABLE_SIZE) {
    /* Define a new symbol = prev sym + head of this sym's expansion */
    sinfo->symbol_head[code] = (UINT16)sinfo->oldcode;
    sinfo->symbol_tail[code] = (UINT8)sinfo->firstcode;
    sinfo->max_code++;
    /* Is it time to increase code_size? */
    if (sinfo->max_code >= sinfo->limit_code &&
        sinfo->code_size < MAX_LZW_BITS) {
      sinfo->code_size++;
      sinfo->limit_code <<= 1;  /* keep equal to 2^code_size */
    }
  }

  sinfo->oldcode = incode;      /* save last input symbol for future use */
  return sinfo->firstcode;      /* return first byte of symbol's expansion */
}