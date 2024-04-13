slow_search(OnigEncoding enc, UChar* target, UChar* target_end,
	    const UChar* text, const UChar* text_end, UChar* text_range)
{
  UChar *t, *p, *s, *end;

  end = (UChar* )text_end;
  end -= target_end - target - 1;
  if (end > text_range)
    end = text_range;

  s = (UChar* )text;

  if (enc->max_enc_len == enc->min_enc_len) {
    int n = enc->max_enc_len;

    while (s < end) {
      if (*s == *target) {
	p = s + 1;
	t = target + 1;
	if (target_end == t || memcmp(t, p, target_end - t) == 0)
	  return s;
      }
      s += n;
    }
    return (UChar*)NULL;
  }
  while (s < end) {
    if (*s == *target) {
      p = s + 1;
      t = target + 1;
      if (target_end == t || memcmp(t, p, target_end - t) == 0)
	return s;
    }
    s += enclen(enc, s, text_end);
  }

  return (UChar* )NULL;
}