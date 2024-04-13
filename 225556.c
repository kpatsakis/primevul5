GetCode(gif_source_ptr sinfo)
/* Fetch the next code_size bits from the GIF data */
/* We assume code_size is less than 16 */
{
  register int accum;
  int offs, count;

  while (sinfo->cur_bit + sinfo->code_size > sinfo->last_bit) {
    /* Time to reload the buffer */
    /* First time, share code with Clear case */
    if (sinfo->first_time) {
      sinfo->first_time = FALSE;
      return sinfo->clear_code;
    }
    if (sinfo->out_of_blocks) {
      WARNMS(sinfo->cinfo, JWRN_GIF_NOMOREDATA);
      return sinfo->end_code;   /* fake something useful */
    }
    /* preserve last two bytes of what we have -- assume code_size <= 16 */
    sinfo->code_buf[0] = sinfo->code_buf[sinfo->last_byte-2];
    sinfo->code_buf[1] = sinfo->code_buf[sinfo->last_byte-1];
    /* Load more bytes; set flag if we reach the terminator block */
    if ((count = GetDataBlock(sinfo, &sinfo->code_buf[2])) == 0) {
      sinfo->out_of_blocks = TRUE;
      WARNMS(sinfo->cinfo, JWRN_GIF_NOMOREDATA);
      return sinfo->end_code;   /* fake something useful */
    }
    /* Reset counters */
    sinfo->cur_bit = (sinfo->cur_bit - sinfo->last_bit) + 16;
    sinfo->last_byte = 2 + count;
    sinfo->last_bit = sinfo->last_byte * 8;
  }

  /* Form up next 24 bits in accum */
  offs = sinfo->cur_bit >> 3;   /* byte containing cur_bit */
  accum = UCH(sinfo->code_buf[offs + 2]);
  accum <<= 8;
  accum |= UCH(sinfo->code_buf[offs + 1]);
  accum <<= 8;
  accum |= UCH(sinfo->code_buf[offs]);

  /* Right-align cur_bit in accum, then mask off desired number of bits */
  accum >>= (sinfo->cur_bit & 7);
  sinfo->cur_bit += sinfo->code_size;
  return accum & ((1 << sinfo->code_size) - 1);
}