set_optimize_exact_info(regex_t* reg, OptExactInfo* e)
{
  int r;

  if (e->len == 0) return 0;

  if (e->ignore_case) {
    reg->exact = (UChar* )xmalloc(e->len);
    CHECK_NULL_RETURN_MEMERR(reg->exact);
    xmemcpy(reg->exact, e->s, e->len);
    reg->exact_end = reg->exact + e->len;
    reg->optimize = ONIG_OPTIMIZE_EXACT_IC;
  }
  else {
    int allow_reverse;

    reg->exact = str_dup(e->s, e->s + e->len);
    CHECK_NULL_RETURN_MEMERR(reg->exact);
    reg->exact_end = reg->exact + e->len;

    allow_reverse =
	ONIGENC_IS_ALLOWED_REVERSE_MATCH(reg->enc, reg->exact, reg->exact_end);

    if (e->len >= 3 || (e->len >= 2 && allow_reverse)) {
      r = set_bm_skip(reg->exact, reg->exact_end, reg->enc,
	              reg->map, &(reg->int_map));
      if (r) return r;

      reg->optimize = (allow_reverse != 0
		       ? ONIG_OPTIMIZE_EXACT_BM : ONIG_OPTIMIZE_EXACT_BM_NOT_REV);
    }
    else {
      reg->optimize = ONIG_OPTIMIZE_EXACT;
    }
  }

  reg->dmin = e->mmd.min;
  reg->dmax = e->mmd.max;

  if (reg->dmin != ONIG_INFINITE_DISTANCE) {
    reg->threshold_len = (int)(reg->dmin + (reg->exact_end - reg->exact));
  }

  return 0;
}