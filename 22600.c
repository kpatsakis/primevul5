not_code_range_buf(OnigEncoding enc, BBuf* bbuf, BBuf** pbuf, ScanEnv* env)
{
  int r, i, n;
  OnigCodePoint pre, from, *data, to = 0;

  *pbuf = (BBuf* )NULL;
  if (IS_NULL(bbuf)) {
  set_all:
    return SET_ALL_MULTI_BYTE_RANGE(enc, pbuf);
  }

  data = (OnigCodePoint* )(bbuf->p);
  GET_CODE_POINT(n, data);
  data++;
  if (n <= 0) goto set_all;

  r = 0;
  pre = MBCODE_START_POS(enc);
  for (i = 0; i < n; i++) {
    from = data[i*2];
    to   = data[i*2+1];
    if (pre <= from - 1) {
      r = add_code_range_to_buf(pbuf, env, pre, from - 1);
      if (r != 0) return r;
    }
    if (to == ~((OnigCodePoint )0)) break;
    pre = to + 1;
  }
  if (to < ~((OnigCodePoint )0)) {
    r = add_code_range_to_buf(pbuf, env, to + 1, ~((OnigCodePoint )0));
  }
  return r;
}