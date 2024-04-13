get_ctype_code_range(OnigCtype ctype, OnigCodePoint* sb_out,
		     const OnigCodePoint* ranges[], OnigEncoding enc ARG_UNUSED)
{
  if (ctype <= ONIGENC_MAX_STD_CTYPE) {
    return ONIG_NO_SUPPORT_CONFIG;
  }
  else {
    *sb_out = 0x80;

    PROPERTY_LIST_INIT_CHECK;

    ctype -= (ONIGENC_MAX_STD_CTYPE + 1);
    if (ctype >= (OnigCtype )PropertyListNum)
      return ONIGERR_TYPE_BUG;

    *ranges = PropertyList[ctype];
    return 0;
  }
}