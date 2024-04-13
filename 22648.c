fetch_name(OnigCodePoint start_code, UChar** src, UChar* end,
	   UChar** rname_end, ScanEnv* env, int* rback_num, int ref)
{
  int r, is_num, sign;
  OnigCodePoint end_code;
  OnigCodePoint c = 0;
  OnigEncoding enc = env->enc;
  UChar *name_end;
  UChar *pnum_head;
  UChar *p = *src;
  PFETCH_READY;

  *rback_num = 0;

  end_code = get_name_end_code_point(start_code);

  name_end = end;
  pnum_head = *src;
  r = 0;
  is_num = 0;
  sign = 1;
  if (PEND) {
    return ONIGERR_EMPTY_GROUP_NAME;
  }
  else {
    PFETCH(c);
    if (c == end_code)
      return ONIGERR_EMPTY_GROUP_NAME;

    if (ONIGENC_IS_CODE_DIGIT(enc, c)) {
      if (ref == 1)
	is_num = 1;
      else {
	r = ONIGERR_INVALID_GROUP_NAME;
	is_num = 0;
      }
    }
    else if (c == '-') {
      if (ref == 1) {
	is_num = 2;
	sign = -1;
	pnum_head = p;
      }
      else {
	r = ONIGERR_INVALID_GROUP_NAME;
	is_num = 0;
      }
    }
    else if (!ONIGENC_IS_CODE_WORD(enc, c)) {
      r = ONIGERR_INVALID_CHAR_IN_GROUP_NAME;
    }
  }

  if (r == 0) {
    while (!PEND) {
      name_end = p;
      PFETCH(c);
      if (c == end_code || c == ')') {
	if (is_num == 2) 	r = ONIGERR_INVALID_GROUP_NAME;
	break;
      }

      if (is_num != 0) {
	if (ONIGENC_IS_CODE_DIGIT(enc, c)) {
	  is_num = 1;
	}
	else {
	  if (!ONIGENC_IS_CODE_WORD(enc, c))
	    r = ONIGERR_INVALID_CHAR_IN_GROUP_NAME;
	  else
	    r = ONIGERR_INVALID_GROUP_NAME;

	  is_num = 0;
	}
      }
      else {
	if (!ONIGENC_IS_CODE_WORD(enc, c)) {
	  r = ONIGERR_INVALID_CHAR_IN_GROUP_NAME;
	}
      }
    }

    if (c != end_code) {
      r = ONIGERR_INVALID_GROUP_NAME;
      name_end = end;
    }

    if (is_num != 0) {
      *rback_num = onig_scan_unsigned_number(&pnum_head, name_end, enc);
      if (*rback_num < 0) return ONIGERR_TOO_BIG_NUMBER;
      else if (*rback_num == 0) {
	r = ONIGERR_INVALID_GROUP_NAME;
	goto err;
      }

      *rback_num *= sign;
    }

    *rname_end = name_end;
    *src = p;
    return 0;
  }
  else {
    while (!PEND) {
      name_end = p;
      PFETCH(c);
      if (c == end_code || c == ')')
	break;
    }
    if (PEND)
      name_end = end;

  err:
    onig_scan_env_set_error_string(env, r, *src, name_end);
    return r;
  }
}