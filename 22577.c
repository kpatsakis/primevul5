bm_search_notrev(regex_t* reg, const UChar* target, const UChar* target_end,
		 const UChar* text, const UChar* text_end,
		 const UChar* text_range)
{
  const UChar *s, *se, *t, *p, *end;
  const UChar *tail;
  ptrdiff_t skip, tlen1;

#ifdef ONIG_DEBUG_SEARCH
  fprintf(stderr, "bm_search_notrev: text: %"PRIuPTR", text_end: %"PRIuPTR", text_range: %"PRIuPTR"\n",
	  text, text_end, text_range);
#endif

  tail = target_end - 1;
  tlen1 = tail - target;
  end = text_range;
  if (end + tlen1 > text_end)
    end = text_end - tlen1;

  s = text;

  if (IS_NULL(reg->int_map)) {
    while (s < end) {
      p = se = s + tlen1;
      t = tail;
      while (*p == *t) {
	if (t == target) return (UChar* )s;
	p--; t--;
      }
      skip = reg->map[*se];
      t = s;
      do {
        s += enclen(reg->enc, s, end);
      } while ((s - t) < skip && s < end);
    }
  }
  else {
    while (s < end) {
      p = se = s + tlen1;
      t = tail;
      while (*p == *t) {
	if (t == target) return (UChar* )s;
	p--; t--;
      }
      skip = reg->int_map[*se];
      t = s;
      do {
        s += enclen(reg->enc, s, end);
      } while ((s - t) < skip && s < end);
    }
  }

  return (UChar* )NULL;
}