onig_match(regex_t* reg, const UChar* str, const UChar* end, const UChar* at, OnigRegion* region,
	    OnigOptionType option)
{
  long r;
  UChar *prev;
  OnigMatchArg msa;

#if defined(USE_RECOMPILE_API) && defined(USE_MULTI_THREAD_SYSTEM)
 start:
  THREAD_ATOMIC_START;
  if (ONIG_STATE(reg) >= ONIG_STATE_NORMAL) {
    ONIG_STATE_INC(reg);
    if (IS_NOT_NULL(reg->chain) && ONIG_STATE(reg) == ONIG_STATE_NORMAL) {
      onig_chain_reduce(reg);
      ONIG_STATE_INC(reg);
    }
  }
  else {
    int n;

    THREAD_ATOMIC_END;
    n = 0;
    while (ONIG_STATE(reg) < ONIG_STATE_NORMAL) {
      if (++n > THREAD_PASS_LIMIT_COUNT)
	return ONIGERR_OVER_THREAD_PASS_LIMIT_COUNT;
      THREAD_PASS;
    }
    goto start;
  }
  THREAD_ATOMIC_END;
#endif /* USE_RECOMPILE_API && USE_MULTI_THREAD_SYSTEM */

  MATCH_ARG_INIT(msa, option, region, at);
#ifdef USE_COMBINATION_EXPLOSION_CHECK
  {
    int offset = at - str;
    STATE_CHECK_BUFF_INIT(msa, end - str, offset, reg->num_comb_exp_check);
  }
#endif

  if (region
#ifdef USE_POSIX_API_REGION_OPTION
      && !IS_POSIX_REGION(option)
#endif
      ) {
    r = onig_region_resize_clear(region, reg->num_mem + 1);
  }
  else
    r = 0;

  if (r == 0) {
    prev = (UChar* )onigenc_get_prev_char_head(reg->enc, str, at, end);
    r = match_at(reg, str, end,
#ifdef USE_MATCH_RANGE_MUST_BE_INSIDE_OF_SPECIFIED_RANGE
		 end,
#endif
		 at, prev, &msa);
  }

  MATCH_ARG_FREE(msa);
  ONIG_STATE_DEC_THREAD(reg);
  return r;
}