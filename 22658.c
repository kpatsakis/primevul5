onig_node_str_cat(Node* node, const UChar* s, const UChar* end)
{
  ptrdiff_t addlen = end - s;

  if (addlen > 0) {
    ptrdiff_t len  = NSTR(node)->end - NSTR(node)->s;

    if (NSTR(node)->capa > 0 || (len + addlen > NODE_STR_BUF_SIZE - 1)) {
      UChar* p;
      ptrdiff_t capa = len + addlen + NODE_STR_MARGIN;

      if (capa <= NSTR(node)->capa) {
	onig_strcpy(NSTR(node)->s + len, s, end);
      }
      else {
	if (NSTR(node)->s == NSTR(node)->buf)
	  p = strcat_capa_from_static(NSTR(node)->s, NSTR(node)->end,
				      s, end, capa);
	else
	  p = strcat_capa(NSTR(node)->s, NSTR(node)->end, s, end, capa);

	CHECK_NULL_RETURN_MEMERR(p);
	NSTR(node)->s    = p;
	NSTR(node)->capa = (int)capa;
      }
    }
    else {
      onig_strcpy(NSTR(node)->s + len, s, end);
    }
    NSTR(node)->end = NSTR(node)->s + len + addlen;
  }

  return 0;
}