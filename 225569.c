ReInitLZW(gif_source_ptr sinfo)
/* (Re)initialize LZW state; shared code for startup and Clear processing */
{
  sinfo->code_size = sinfo->input_code_size + 1;
  sinfo->limit_code = sinfo->clear_code << 1;   /* 2^code_size */
  sinfo->max_code = sinfo->clear_code + 2;      /* first unused code value */
  sinfo->sp = sinfo->symbol_stack;              /* init stack to empty */
}