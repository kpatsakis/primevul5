concat_opt_exact_info(OptExactInfo* to, OptExactInfo* add, OnigEncoding enc)
{
  int i, j, len;
  UChar *p, *end;
  OptAncInfo tanc;

  if (! to->ignore_case && add->ignore_case) {
    if (to->len >= add->len) return ;  /* avoid */

    to->ignore_case = 1;
  }

  p = add->s;
  end = p + add->len;
  for (i = to->len; p < end; ) {
    len = enclen(enc, p, end);
    if (i + len > OPT_EXACT_MAXLEN) break;
    for (j = 0; j < len && p < end; j++)
      to->s[i++] = *p++;
  }

  to->len = i;
  to->reach_end = (p == end ? add->reach_end : 0);

  concat_opt_anc_info(&tanc, &to->anc, &add->anc, 1, 1);
  if (! to->reach_end) tanc.right_anchor = 0;
  copy_opt_anc_info(&to->anc, &tanc);
}