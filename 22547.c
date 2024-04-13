parse_posix_bracket(CClassNode* cc, UChar** src, UChar* end, ScanEnv* env)
{
#define POSIX_BRACKET_CHECK_LIMIT_LENGTH  20
#define POSIX_BRACKET_NAME_MIN_LEN         4

  static const PosixBracketEntryType PBS[] = {
    { (UChar* )"alnum",  ONIGENC_CTYPE_ALNUM,  5 },
    { (UChar* )"alpha",  ONIGENC_CTYPE_ALPHA,  5 },
    { (UChar* )"blank",  ONIGENC_CTYPE_BLANK,  5 },
    { (UChar* )"cntrl",  ONIGENC_CTYPE_CNTRL,  5 },
    { (UChar* )"digit",  ONIGENC_CTYPE_DIGIT,  5 },
    { (UChar* )"graph",  ONIGENC_CTYPE_GRAPH,  5 },
    { (UChar* )"lower",  ONIGENC_CTYPE_LOWER,  5 },
    { (UChar* )"print",  ONIGENC_CTYPE_PRINT,  5 },
    { (UChar* )"punct",  ONIGENC_CTYPE_PUNCT,  5 },
    { (UChar* )"space",  ONIGENC_CTYPE_SPACE,  5 },
    { (UChar* )"upper",  ONIGENC_CTYPE_UPPER,  5 },
    { (UChar* )"xdigit", ONIGENC_CTYPE_XDIGIT, 6 },
    { (UChar* )"ascii",  ONIGENC_CTYPE_ASCII,  5 },
    { (UChar* )"word",   ONIGENC_CTYPE_WORD,   4 },
    { (UChar* )NULL,     -1, 0 }
  };

  const PosixBracketEntryType *pb;
  int not, i, r;
  OnigCodePoint c;
  OnigEncoding enc = env->enc;
  UChar *p = *src;
  PFETCH_READY;

  if (PPEEK_IS('^')) {
    PINC;
    not = 1;
  }
  else
    not = 0;

  if (onigenc_strlen(enc, p, end) < POSIX_BRACKET_NAME_MIN_LEN + 3)
    goto not_posix_bracket;

  for (pb = PBS; IS_NOT_NULL(pb->name); pb++) {
    if (onigenc_with_ascii_strncmp(enc, p, end, pb->name, pb->len) == 0) {
      p = (UChar* )onigenc_step(enc, p, end, pb->len);
      if (onigenc_with_ascii_strncmp(enc, p, end, (UChar* )":]", 2) != 0)
	return ONIGERR_INVALID_POSIX_BRACKET_TYPE;

      r = add_ctype_to_cc(cc, pb->ctype, not, env);
      if (r != 0) return r;

      PINC; PINC;
      *src = p;
      return 0;
    }
  }

 not_posix_bracket:
  c = 0;
  i = 0;
  while (!PEND && ((c = PPEEK) != ':') && c != ']') {
    PINC;
    if (++i > POSIX_BRACKET_CHECK_LIMIT_LENGTH) break;
  }
  if (c == ':' && ! PEND) {
    PINC;
    if (! PEND) {
      PFETCH(c);
      if (c == ']')
	return ONIGERR_INVALID_POSIX_BRACKET_TYPE;
    }
  }

  return 1;  /* 1: is not POSIX bracket, but no error. */
}