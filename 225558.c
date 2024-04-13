InitLZWCode(gif_source_ptr sinfo)
/* Initialize for a series of LZWReadByte (and hence GetCode) calls */
{
  /* GetCode initialization */
  sinfo->last_byte = 2;         /* make safe to "recopy last two bytes" */
  sinfo->code_buf[0] = 0;
  sinfo->code_buf[1] = 0;
  sinfo->last_bit = 0;          /* nothing in the buffer */
  sinfo->cur_bit = 0;           /* force buffer load on first call */
  sinfo->first_time = TRUE;
  sinfo->out_of_blocks = FALSE;

  /* LZWReadByte initialization: */
  /* compute special code values (note that these do not change later) */
  sinfo->clear_code = 1 << sinfo->input_code_size;
  sinfo->end_code = sinfo->clear_code + 1;
  ReInitLZW(sinfo);
}