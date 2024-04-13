is_not_included(Node* x, Node* y, regex_t* reg)
{
  int i;
  OnigDistance len;
  OnigCodePoint code;
  UChar *p, c;
  int ytype;

 retry:
  ytype = NTYPE(y);
  switch (NTYPE(x)) {
  case NT_CTYPE:
    {
      switch (ytype) {
      case NT_CTYPE:
	if (NCTYPE(y)->ctype == NCTYPE(x)->ctype &&
	    NCTYPE(y)->not   != NCTYPE(x)->not)
	  return 1;
	else
	  return 0;
	break;

      case NT_CCLASS:
      swap:
	{
	  Node* tmp;
	  tmp = x; x = y; y = tmp;
	  goto retry;
	}
	break;

      case NT_STR:
	goto swap;
	break;

      default:
	break;
      }
    }
    break;

  case NT_CCLASS:
    {
      CClassNode* xc = NCCLASS(x);
      switch (ytype) {
      case NT_CTYPE:
	switch (NCTYPE(y)->ctype) {
	case ONIGENC_CTYPE_WORD:
	  if (NCTYPE(y)->not == 0) {
	    if (IS_NULL(xc->mbuf) && !IS_NCCLASS_NOT(xc)) {
	      for (i = 0; i < SINGLE_BYTE_SIZE; i++) {
		if (BITSET_AT(xc->bs, i)) {
		  if (IS_CODE_SB_WORD(reg->enc, i)) return 0;
		}
	      }
	      return 1;
	    }
	    return 0;
	  }
	  else {
	    for (i = 0; i < SINGLE_BYTE_SIZE; i++) {
	      if (! IS_CODE_SB_WORD(reg->enc, i)) {
		if (!IS_NCCLASS_NOT(xc)) {
		  if (BITSET_AT(xc->bs, i))
		    return 0;
		}
		else {
		  if (! BITSET_AT(xc->bs, i))
		    return 0;
		}
	      }
	    }
	    return 1;
	  }
	  break;

	default:
	  break;
	}
	break;

      case NT_CCLASS:
	{
	  int v;
	  CClassNode* yc = NCCLASS(y);

	  for (i = 0; i < SINGLE_BYTE_SIZE; i++) {
	    v = BITSET_AT(xc->bs, i);
	    if ((v != 0 && !IS_NCCLASS_NOT(xc)) ||
                (v == 0 && IS_NCCLASS_NOT(xc))) {
	      v = BITSET_AT(yc->bs, i);
	      if ((v != 0 && !IS_NCCLASS_NOT(yc)) ||
                  (v == 0 && IS_NCCLASS_NOT(yc)))
		return 0;
	    }
	  }
	  if ((IS_NULL(xc->mbuf) && !IS_NCCLASS_NOT(xc)) ||
	      (IS_NULL(yc->mbuf) && !IS_NCCLASS_NOT(yc)))
	    return 1;
	  return 0;
	}
	break;

      case NT_STR:
	goto swap;
	break;

      default:
	break;
      }
    }
    break;

  case NT_STR:
    {
      StrNode* xs = NSTR(x);
      if (NSTRING_LEN(x) == 0)
	break;

      c = *(xs->s);
      switch (ytype) {
      case NT_CTYPE:
	switch (NCTYPE(y)->ctype) {
	case ONIGENC_CTYPE_WORD:
	  if (ONIGENC_IS_MBC_WORD(reg->enc, xs->s, xs->end))
	    return NCTYPE(y)->not;
	  else
	    return !(NCTYPE(y)->not);
	  break;
	default:
	  break;
	}
	break;

      case NT_CCLASS:
	{
	  CClassNode* cc = NCCLASS(y);

	  code = ONIGENC_MBC_TO_CODE(reg->enc, xs->s,
				     xs->s + ONIGENC_MBC_MAXLEN(reg->enc));
	  return (onig_is_code_in_cc(reg->enc, code, cc) != 0 ? 0 : 1);
	}
	break;

      case NT_STR:
	{
	  UChar *q;
	  StrNode* ys = NSTR(y);
	  len = NSTRING_LEN(x);
	  if (len > NSTRING_LEN(y)) len = NSTRING_LEN(y);
	  if (NSTRING_IS_AMBIG(x) || NSTRING_IS_AMBIG(y)) {
            /* tiny version */
            return 0;
	  }
	  else {
	    for (i = 0, p = ys->s, q = xs->s; (OnigDistance)i < len; i++, p++, q++) {
	      if (*p != *q) return 1;
	    }
	  }
	}
	break;

      default:
	break;
      }
    }
    break;

  default:
    break;
  }

  return 0;
}