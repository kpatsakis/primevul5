next_state_val(CClassNode* cc, OnigCodePoint *vs, OnigCodePoint v,
	       int* vs_israw, int v_israw,
	       enum CCVALTYPE intype, enum CCVALTYPE* type,
	       enum CCSTATE* state, ScanEnv* env)
{
  int r;

  switch (*state) {
  case CCS_VALUE:
    if (*type == CCV_SB)
      BITSET_SET_BIT_CHKDUP(cc->bs, (int )(*vs));
    else if (*type == CCV_CODE_POINT) {
      r = add_code_range(&(cc->mbuf), env, *vs, *vs);
      if (r < 0) return r;
    }
    break;

  case CCS_RANGE:
    if (intype == *type) {
      if (intype == CCV_SB) {
        if (*vs > 0xff || v > 0xff)
          return ONIGERR_INVALID_CODE_POINT_VALUE;

	if (*vs > v) {
	  if (IS_SYNTAX_BV(env->syntax, ONIG_SYN_ALLOW_EMPTY_RANGE_IN_CC))
	    goto ccs_range_end;
	  else
	    return ONIGERR_EMPTY_RANGE_IN_CHAR_CLASS;
	}
	bitset_set_range(env, cc->bs, (int )*vs, (int )v);
      }
      else {
	r = add_code_range(&(cc->mbuf), env, *vs, v);
	if (r < 0) return r;
      }
    }
    else {
#if 0
      if (intype == CCV_CODE_POINT && *type == CCV_SB) {
#endif
	if (*vs > v) {
	  if (IS_SYNTAX_BV(env->syntax, ONIG_SYN_ALLOW_EMPTY_RANGE_IN_CC))
	    goto ccs_range_end;
	  else
	    return ONIGERR_EMPTY_RANGE_IN_CHAR_CLASS;
	}
	bitset_set_range(env, cc->bs, (int )*vs, (int )(v < 0xff ? v : 0xff));
	r = add_code_range(&(cc->mbuf), env, (OnigCodePoint )*vs, v);
	if (r < 0) return r;
#if 0
      }
      else
	return ONIGERR_MISMATCH_CODE_LENGTH_IN_CLASS_RANGE;
#endif
    }
  ccs_range_end:
    *state = CCS_COMPLETE;
    break;

  case CCS_COMPLETE:
  case CCS_START:
    *state = CCS_VALUE;
    break;

  default:
    break;
  }

  *vs_israw = v_israw;
  *vs       = v;
  *type     = intype;
  return 0;
}