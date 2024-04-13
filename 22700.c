bm_search(regex_t* reg, const UChar* target, const UChar* target_end,
	  const UChar* text, const UChar* text_end, const UChar* text_range)
{
  const UChar *s, *t, *p, *end;
  const UChar *tail;

#ifdef ONIG_DEBUG_SEARCH
  fprintf(stderr, "bm_search: text: %"PRIuPTR", text_end: %"PRIuPTR", text_range: %"PRIuPTR"\n",
	  text, text_end, text_range);
#endif

  end = text_range + (target_end - target) - 1;
  if (end > text_end)
    end = text_end;

  tail = target_end - 1;
  s = text + (target_end - target) - 1;
  if (IS_NULL(reg->int_map)) {
    while (s < end) {
      p = s;
      t = tail;
#ifdef ONIG_DEBUG_SEARCH
  fprintf(stderr, "bm_search_loop: pos: %d %s\n",
	  (int)(s - text), s);
#endif
      while (*p == *t) {
	if (t == target) return (UChar* )p;
	p--; t--;
      }
      s += reg->map[*s];
    }
  }
  else { /* see int_map[] */
    while (s < end) {
      p = s;
      t = tail;
      while (*p == *t) {
	if (t == target) return (UChar* )p;
	p--; t--;
      }
      s += reg->int_map[*s];
    }
  }
  return (UChar* )NULL;
}