gb18030_left_adjust_char_head(const UChar* start, const UChar* s, const UChar* end, OnigEncoding enc)
{
  const UChar *p;
  enum state state = S_START;

  DEBUG_GB18030(("----------------\n"));
  for (p = s; p >= start; p--) {
    DEBUG_GB18030(("state %d --(%02x)-->\n", state, *p));
    switch (state) {
    case S_START:
      switch (GB18030_MAP[*p]) {
      case C1:
	return (UChar *)s;
      case C2:
	state = S_one_C2; /* C2 */
	break;
      case C4:
	state = S_one_C4; /* C4 */
	break;
      case CM:
	state = S_one_CM; /* CM */
	break;
      }
      break;
    case S_one_C2: /* C2 */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
      case C4:
	return (UChar *)s;
      case CM:
	state = S_odd_CM_one_CX; /* CM C2 */
	break;
      }
      break;
    case S_one_C4: /* C4 */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
      case C4:
	return (UChar *)s;
      case CM:
	state = S_one_CMC4;
	break;
      }
      break;
    case S_one_CM: /* CM */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
	return (UChar *)s;
      case C4:
	state = S_odd_C4CM;
	break;
      case CM:
	state = S_odd_CM_one_CX; /* CM CM */
	break;
      }
      break;

    case S_odd_CM_one_CX: /* CM C2 */ /* CM CM */ /* CM CM CM C4 */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
      case C4:
	return (UChar *)(s - 1);
      case CM:
	state = S_even_CM_one_CX;
	break;
      }
      break;
    case S_even_CM_one_CX: /* CM CM C2 */ /* CM CM CM */ /* CM CM C4 */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
      case C4:
	return (UChar *)s;
      case CM:
	state = S_odd_CM_one_CX;
	break;
      }
      break;

    case S_one_CMC4: /* CM C4 */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
	return (UChar *)(s - 1);
      case C4:
	state = S_one_C4_odd_CMC4; /* C4 CM C4 */
	break;
      case CM:
	state = S_even_CM_one_CX; /* CM CM C4 */
	break;
      }
      break;
    case S_odd_CMC4: /* CM C4 CM C4 CM C4 */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
	return (UChar *)(s - 1);
      case C4:
	state = S_one_C4_odd_CMC4;
	break;
      case CM:
	state = S_odd_CM_odd_CMC4;
	break;
      }
      break;
    case S_one_C4_odd_CMC4: /* C4 CM C4 */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
      case C4:
	return (UChar *)(s - 1);
      case CM:
	state = S_even_CMC4; /* CM C4 CM C4 */
	break;
      }
      break;
    case S_even_CMC4: /* CM C4 CM C4 */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
	return (UChar *)(s - 3);
      case C4:
	state = S_one_C4_even_CMC4;
	break;
      case CM:
	state = S_odd_CM_even_CMC4;
	break;
      }
      break;
    case S_one_C4_even_CMC4: /* C4 CM C4 CM C4 */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
      case C4:
	return (UChar *)(s - 3);
      case CM:
	state = S_odd_CMC4;
	break;
      }
      break;

    case S_odd_CM_odd_CMC4: /* CM CM C4 CM C4 CM C4 */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
      case C4:
	return (UChar *)(s - 3);
      case CM:
	state = S_even_CM_odd_CMC4;
	break;
      }
      break;
    case S_even_CM_odd_CMC4: /* CM CM CM C4 CM C4 CM C4 */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
      case C4:
	return (UChar *)(s - 1);
      case CM:
	state = S_odd_CM_odd_CMC4;
	break;
      }
      break;

    case S_odd_CM_even_CMC4: /* CM CM C4 CM C4 */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
      case C4:
	return (UChar *)(s - 1);
      case CM:
	state = S_even_CM_even_CMC4;
	break;
      }
      break;
    case S_even_CM_even_CMC4: /* CM CM CM C4 CM C4 */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
      case C4:
	return (UChar *)(s - 3);
      case CM:
	state = S_odd_CM_even_CMC4;
	break;
      }
      break;

    case S_odd_C4CM: /* C4 CM */  /* C4 CM C4 CM C4 CM*/
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
      case C4:
	return (UChar *)s;
      case CM:
	state = S_one_CM_odd_C4CM; /* CM C4 CM */
	break;
      }
      break;
    case S_one_CM_odd_C4CM: /* CM C4 CM */ /* CM C4 CM C4 CM C4 CM */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
	return (UChar *)(s - 2); /* |CM C4 CM */
      case C4:
	state = S_even_C4CM;
	break;
      case CM:
	state = S_even_CM_odd_C4CM;
	break;
      }
      break;
    case S_even_C4CM: /* C4 CM C4 CM */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
      case C4:
	return (UChar *)(s - 2);  /* C4|CM C4 CM */
      case CM:
	state = S_one_CM_even_C4CM;
	break;
      }
      break;
    case S_one_CM_even_C4CM: /* CM C4 CM C4 CM */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
	return (UChar *)(s - 0);  /*|CM C4 CM C4|CM */
      case C4:
	state = S_odd_C4CM;
	break;
      case CM:
	state = S_even_CM_even_C4CM;
	break;
      }
      break;

    case S_even_CM_odd_C4CM: /* CM CM C4 CM */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
      case C4:
	return (UChar *)(s - 0); /* |CM CM|C4|CM */
      case CM:
	state = S_odd_CM_odd_C4CM;
	break;
      }
      break;
    case S_odd_CM_odd_C4CM: /* CM CM CM C4 CM */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
      case C4:
	return (UChar *)(s - 2); /* |CM CM|CM C4 CM */
      case CM:
	state = S_even_CM_odd_C4CM;
	break;
      }
      break;

    case S_even_CM_even_C4CM: /* CM CM C4 CM C4 CM */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
      case C4:
	return (UChar *)(s - 2); /* |CM CM|C4|CM C4 CM */
      case CM:
	state = S_odd_CM_even_C4CM;
	break;
      }
      break;
    case S_odd_CM_even_C4CM: /* CM CM CM C4 CM C4 CM */
      switch (GB18030_MAP[*p]) {
      case C1:
      case C2:
      case C4:
	return (UChar *)(s - 0);  /* |CM CM|CM C4 CM C4|CM */
      case CM:
	state = S_even_CM_even_C4CM;
	break;
      }
      break;
    }
  }

  DEBUG_GB18030(("state %d\n", state));
  switch (state) {
  case S_START:             return (UChar *)(s - 0);
  case S_one_C2:            return (UChar *)(s - 0);
  case S_one_C4:            return (UChar *)(s - 0);
  case S_one_CM:            return (UChar *)(s - 0);

  case S_odd_CM_one_CX:     return (UChar *)(s - 1);
  case S_even_CM_one_CX:    return (UChar *)(s - 0);

  case S_one_CMC4:          return (UChar *)(s - 1);
  case S_odd_CMC4:          return (UChar *)(s - 1);
  case S_one_C4_odd_CMC4:   return (UChar *)(s - 1);
  case S_even_CMC4:         return (UChar *)(s - 3);
  case S_one_C4_even_CMC4:  return (UChar *)(s - 3);

  case S_odd_CM_odd_CMC4:   return (UChar *)(s - 3);
  case S_even_CM_odd_CMC4:  return (UChar *)(s - 1);

  case S_odd_CM_even_CMC4:  return (UChar *)(s - 1);
  case S_even_CM_even_CMC4: return (UChar *)(s - 3);

  case S_odd_C4CM:          return (UChar *)(s - 0);
  case S_one_CM_odd_C4CM:   return (UChar *)(s - 2);
  case S_even_C4CM:         return (UChar *)(s - 2);
  case S_one_CM_even_C4CM:  return (UChar *)(s - 0);

  case S_even_CM_odd_C4CM:  return (UChar *)(s - 0);
  case S_odd_CM_odd_C4CM:   return (UChar *)(s - 2);
  case S_even_CM_even_C4CM: return (UChar *)(s - 2);
  case S_odd_CM_even_C4CM:  return (UChar *)(s - 0);
  }

  return (UChar* )s;  /* never come here. (escape warning) */
}