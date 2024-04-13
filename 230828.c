static PCRE2_SPTR next_opcode(compiler_common *common, PCRE2_SPTR cc)
{
SLJIT_UNUSED_ARG(common);
switch(*cc)
  {
  case OP_SOD:
  case OP_SOM:
  case OP_SET_SOM:
  case OP_NOT_WORD_BOUNDARY:
  case OP_WORD_BOUNDARY:
  case OP_NOT_DIGIT:
  case OP_DIGIT:
  case OP_NOT_WHITESPACE:
  case OP_WHITESPACE:
  case OP_NOT_WORDCHAR:
  case OP_WORDCHAR:
  case OP_ANY:
  case OP_ALLANY:
  case OP_NOTPROP:
  case OP_PROP:
  case OP_ANYNL:
  case OP_NOT_HSPACE:
  case OP_HSPACE:
  case OP_NOT_VSPACE:
  case OP_VSPACE:
  case OP_EXTUNI:
  case OP_EODN:
  case OP_EOD:
  case OP_CIRC:
  case OP_CIRCM:
  case OP_DOLL:
  case OP_DOLLM:
  case OP_CRSTAR:
  case OP_CRMINSTAR:
  case OP_CRPLUS:
  case OP_CRMINPLUS:
  case OP_CRQUERY:
  case OP_CRMINQUERY:
  case OP_CRRANGE:
  case OP_CRMINRANGE:
  case OP_CRPOSSTAR:
  case OP_CRPOSPLUS:
  case OP_CRPOSQUERY:
  case OP_CRPOSRANGE:
  case OP_CLASS:
  case OP_NCLASS:
  case OP_REF:
  case OP_REFI:
  case OP_DNREF:
  case OP_DNREFI:
  case OP_RECURSE:
  case OP_CALLOUT:
  case OP_ALT:
  case OP_KET:
  case OP_KETRMAX:
  case OP_KETRMIN:
  case OP_KETRPOS:
  case OP_REVERSE:
  case OP_ASSERT:
  case OP_ASSERT_NOT:
  case OP_ASSERTBACK:
  case OP_ASSERTBACK_NOT:
  case OP_ONCE:
  case OP_BRA:
  case OP_BRAPOS:
  case OP_CBRA:
  case OP_CBRAPOS:
  case OP_COND:
  case OP_SBRA:
  case OP_SBRAPOS:
  case OP_SCBRA:
  case OP_SCBRAPOS:
  case OP_SCOND:
  case OP_CREF:
  case OP_DNCREF:
  case OP_RREF:
  case OP_DNRREF:
  case OP_FALSE:
  case OP_TRUE:
  case OP_BRAZERO:
  case OP_BRAMINZERO:
  case OP_BRAPOSZERO:
  case OP_PRUNE:
  case OP_SKIP:
  case OP_THEN:
  case OP_COMMIT:
  case OP_FAIL:
  case OP_ACCEPT:
  case OP_ASSERT_ACCEPT:
  case OP_CLOSE:
  case OP_SKIPZERO:
  return cc + PRIV(OP_lengths)[*cc];

  case OP_CHAR:
  case OP_CHARI:
  case OP_NOT:
  case OP_NOTI:
  case OP_STAR:
  case OP_MINSTAR:
  case OP_PLUS:
  case OP_MINPLUS:
  case OP_QUERY:
  case OP_MINQUERY:
  case OP_UPTO:
  case OP_MINUPTO:
  case OP_EXACT:
  case OP_POSSTAR:
  case OP_POSPLUS:
  case OP_POSQUERY:
  case OP_POSUPTO:
  case OP_STARI:
  case OP_MINSTARI:
  case OP_PLUSI:
  case OP_MINPLUSI:
  case OP_QUERYI:
  case OP_MINQUERYI:
  case OP_UPTOI:
  case OP_MINUPTOI:
  case OP_EXACTI:
  case OP_POSSTARI:
  case OP_POSPLUSI:
  case OP_POSQUERYI:
  case OP_POSUPTOI:
  case OP_NOTSTAR:
  case OP_NOTMINSTAR:
  case OP_NOTPLUS:
  case OP_NOTMINPLUS:
  case OP_NOTQUERY:
  case OP_NOTMINQUERY:
  case OP_NOTUPTO:
  case OP_NOTMINUPTO:
  case OP_NOTEXACT:
  case OP_NOTPOSSTAR:
  case OP_NOTPOSPLUS:
  case OP_NOTPOSQUERY:
  case OP_NOTPOSUPTO:
  case OP_NOTSTARI:
  case OP_NOTMINSTARI:
  case OP_NOTPLUSI:
  case OP_NOTMINPLUSI:
  case OP_NOTQUERYI:
  case OP_NOTMINQUERYI:
  case OP_NOTUPTOI:
  case OP_NOTMINUPTOI:
  case OP_NOTEXACTI:
  case OP_NOTPOSSTARI:
  case OP_NOTPOSPLUSI:
  case OP_NOTPOSQUERYI:
  case OP_NOTPOSUPTOI:
  cc += PRIV(OP_lengths)[*cc];
#ifdef SUPPORT_UNICODE
  if (common->utf && HAS_EXTRALEN(cc[-1])) cc += GET_EXTRALEN(cc[-1]);
#endif
  return cc;

  /* Special cases. */
  case OP_TYPESTAR:
  case OP_TYPEMINSTAR:
  case OP_TYPEPLUS:
  case OP_TYPEMINPLUS:
  case OP_TYPEQUERY:
  case OP_TYPEMINQUERY:
  case OP_TYPEUPTO:
  case OP_TYPEMINUPTO:
  case OP_TYPEEXACT:
  case OP_TYPEPOSSTAR:
  case OP_TYPEPOSPLUS:
  case OP_TYPEPOSQUERY:
  case OP_TYPEPOSUPTO:
  return cc + PRIV(OP_lengths)[*cc] - 1;

  case OP_ANYBYTE:
#ifdef SUPPORT_UNICODE
  if (common->utf) return NULL;
#endif
  return cc + 1;

  case OP_CALLOUT_STR:
  return cc + GET(cc, 1 + 2*LINK_SIZE);

#if defined SUPPORT_UNICODE || PCRE2_CODE_UNIT_WIDTH != 8
  case OP_XCLASS:
  return cc + GET(cc, 1);
#endif

  case OP_MARK:
  case OP_COMMIT_ARG:
  case OP_PRUNE_ARG:
  case OP_SKIP_ARG:
  case OP_THEN_ARG:
  return cc + 1 + 2 + cc[1];

  default:
  /* All opcodes are supported now! */
  SLJIT_UNREACHABLE();
  return NULL;
  }
}