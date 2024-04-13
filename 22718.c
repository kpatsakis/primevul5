onigenc_unicode_mbc_case_fold(OnigEncoding enc,
    OnigCaseFoldType flag ARG_UNUSED, const UChar** pp, const UChar* end,
    UChar* fold)
{
  CodePointList3 *to;
  OnigCodePoint code;
  int i, len, rlen;
  const UChar *p = *pp;

  if (CaseFoldInited == 0) init_case_fold_table();

  code = ONIGENC_MBC_TO_CODE(enc, p, end);
  len = enclen(enc, p, end);
  *pp += len;

#ifdef USE_UNICODE_CASE_FOLD_TURKISH_AZERI
  if ((flag & ONIGENC_CASE_FOLD_TURKISH_AZERI) != 0) {
    if (code == 0x0049) {
      return ONIGENC_CODE_TO_MBC(enc, 0x0131, fold);
    }
    else if (code == 0x0130) {
      return ONIGENC_CODE_TO_MBC(enc, 0x0069, fold);
    }
  }
#endif

  if (onig_st_lookup(FoldTable, (st_data_t )code, (void* )&to) != 0) {
    if (to->n == 1) {
      return ONIGENC_CODE_TO_MBC(enc, to->code[0], fold);
    }
#if 0
    /* NO NEEDS TO CHECK */
    else if ((flag & INTERNAL_ONIGENC_CASE_FOLD_MULTI_CHAR) != 0)
#else
    else
#endif
    {
      rlen = 0;
      for (i = 0; i < to->n; i++) {
	len = ONIGENC_CODE_TO_MBC(enc, to->code[i], fold);
	fold += len;
	rlen += len;
      }
      return rlen;
    }
  }

  for (i = 0; i < len; i++) {
    *fold++ = *p++;
  }
  return len;
}