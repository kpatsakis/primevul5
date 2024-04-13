print_compiled_byte_code_list(FILE* f, regex_t* reg)
{
  int ncode;
  UChar* bp = reg->p;
  UChar* end = reg->p + reg->used;

  fprintf(f, "code length: %d\n", reg->used);

  ncode = -1;
  while (bp < end) {
    ncode++;
    if (bp > reg->p) {
      if (ncode % 5 == 0)
	fprintf(f, "\n%ld:", bp-reg->p);
      else
	fprintf(f, " %ld:", bp-reg->p);
    }
    onig_print_compiled_byte_code(f, bp, end, &bp, reg->enc);
  }

  fprintf(f, "\n");
}