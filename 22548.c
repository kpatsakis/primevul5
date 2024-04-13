str_node_split_last_char(StrNode* sn, OnigEncoding enc)
{
  const UChar *p;
  Node* n = NULL_NODE;

  if (sn->end > sn->s) {
    p = onigenc_get_prev_char_head(enc, sn->s, sn->end, sn->end);
    if (p && p > sn->s) { /* can be splitted. */
      n = node_new_str(p, sn->end);
      if ((sn->flag & NSTR_RAW) != 0)
	NSTRING_SET_RAW(n);
      sn->end = (UChar* )p;
    }
  }
  return n;
}