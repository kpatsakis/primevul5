compile_length_string_node(Node* node, regex_t* reg)
{
  int rlen, r, len, prev_len, slen, ambig;
  OnigEncoding enc = reg->enc;
  UChar *p, *prev;
  StrNode* sn;

  sn = NSTR(node);
  if (sn->end <= sn->s)
    return 0;

  ambig = NSTRING_IS_AMBIG(node);

  p = prev = sn->s;
  prev_len = enclen(enc, p, sn->end);
  p += prev_len;
  slen = 1;
  rlen = 0;

  for (; p < sn->end; ) {
    len = enclen(enc, p, sn->end);
    if (len == prev_len) {
      slen++;
    }
    else {
      r = add_compile_string_length(prev, prev_len, slen, reg, ambig);
      rlen += r;
      prev = p;
      slen = 1;
      prev_len = len;
    }
    p += len;
  }
  r = add_compile_string_length(prev, prev_len, slen, reg, ambig);
  rlen += r;
  return rlen;
}