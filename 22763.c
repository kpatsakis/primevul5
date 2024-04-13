add_ctype_to_cc(CClassNode* cc, int ctype, int not, ScanEnv* env)
{
  int c, r;
  const OnigCodePoint *ranges;
  OnigCodePoint sb_out;
  OnigEncoding enc = env->enc;

  switch (ctype) {
  case ONIGENC_CTYPE_D:
  case ONIGENC_CTYPE_S:
  case ONIGENC_CTYPE_W:
    ctype ^= ONIGENC_CTYPE_SPECIAL_MASK;
    if (not != 0) {
      for (c = 0; c < SINGLE_BYTE_SIZE; c++) {
	if (! ONIGENC_IS_ASCII_CODE_CTYPE((OnigCodePoint )c, ctype))
	  BITSET_SET_BIT_CHKDUP(cc->bs, c);
      }
      ADD_ALL_MULTI_BYTE_RANGE(enc, cc->mbuf);
    }
    else {
      for (c = 0; c < SINGLE_BYTE_SIZE; c++) {
	if (ONIGENC_IS_ASCII_CODE_CTYPE((OnigCodePoint )c, ctype))
	  BITSET_SET_BIT_CHKDUP(cc->bs, c);
      }
    }
    return 0;
    break;
  }

  r = ONIGENC_GET_CTYPE_CODE_RANGE(enc, ctype, &sb_out, &ranges);
  if (r == 0) {
    return add_ctype_to_cc_by_range(cc, ctype, not, env, sb_out, ranges);
  }
  else if (r != ONIG_NO_SUPPORT_CONFIG) {
    return r;
  }

  r = 0;
  switch (ctype) {
  case ONIGENC_CTYPE_ALPHA:
  case ONIGENC_CTYPE_BLANK:
  case ONIGENC_CTYPE_CNTRL:
  case ONIGENC_CTYPE_DIGIT:
  case ONIGENC_CTYPE_LOWER:
  case ONIGENC_CTYPE_PUNCT:
  case ONIGENC_CTYPE_SPACE:
  case ONIGENC_CTYPE_UPPER:
  case ONIGENC_CTYPE_XDIGIT:
  case ONIGENC_CTYPE_ASCII:
  case ONIGENC_CTYPE_ALNUM:
    if (not != 0) {
      for (c = 0; c < SINGLE_BYTE_SIZE; c++) {
	if (! ONIGENC_IS_CODE_CTYPE(enc, (OnigCodePoint )c, ctype))
	  BITSET_SET_BIT_CHKDUP(cc->bs, c);
      }
      ADD_ALL_MULTI_BYTE_RANGE(enc, cc->mbuf);
    }
    else {
      for (c = 0; c < SINGLE_BYTE_SIZE; c++) {
	if (ONIGENC_IS_CODE_CTYPE(enc, (OnigCodePoint )c, ctype))
	  BITSET_SET_BIT_CHKDUP(cc->bs, c);
      }
    }
    break;

  case ONIGENC_CTYPE_GRAPH:
  case ONIGENC_CTYPE_PRINT:
    if (not != 0) {
      for (c = 0; c < SINGLE_BYTE_SIZE; c++) {
	if (! ONIGENC_IS_CODE_CTYPE(enc, (OnigCodePoint )c, ctype))
	  BITSET_SET_BIT_CHKDUP(cc->bs, c);
      }
    }
    else {
      for (c = 0; c < SINGLE_BYTE_SIZE; c++) {
	if (ONIGENC_IS_CODE_CTYPE(enc, (OnigCodePoint )c, ctype))
	  BITSET_SET_BIT_CHKDUP(cc->bs, c);
      }
      ADD_ALL_MULTI_BYTE_RANGE(enc, cc->mbuf);
    }
    break;

  case ONIGENC_CTYPE_WORD:
    if (not == 0) {
      for (c = 0; c < SINGLE_BYTE_SIZE; c++) {
	if (IS_CODE_SB_WORD(enc, c)) BITSET_SET_BIT_CHKDUP(cc->bs, c);
      }
      ADD_ALL_MULTI_BYTE_RANGE(enc, cc->mbuf);
    }
    else {
      for (c = 0; c < SINGLE_BYTE_SIZE; c++) {
        if ((ONIGENC_CODE_TO_MBCLEN(enc, c) > 0) /* check invalid code point */
	    && ! ONIGENC_IS_CODE_WORD(enc, c))
	  BITSET_SET_BIT_CHKDUP(cc->bs, c);
      }
    }
    break;

  default:
    return ONIGERR_PARSER_BUG;
    break;
  }

  return r;
}