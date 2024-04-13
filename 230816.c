static BOOL is_accelerated_repeat(PCRE2_SPTR cc)
{
switch(*cc)
  {
  case OP_TYPESTAR:
  case OP_TYPEMINSTAR:
  case OP_TYPEPLUS:
  case OP_TYPEMINPLUS:
  case OP_TYPEPOSSTAR:
  case OP_TYPEPOSPLUS:
  return (cc[1] != OP_ANYNL && cc[1] != OP_EXTUNI);

  case OP_STAR:
  case OP_MINSTAR:
  case OP_PLUS:
  case OP_MINPLUS:
  case OP_POSSTAR:
  case OP_POSPLUS:

  case OP_STARI:
  case OP_MINSTARI:
  case OP_PLUSI:
  case OP_MINPLUSI:
  case OP_POSSTARI:
  case OP_POSPLUSI:

  case OP_NOTSTAR:
  case OP_NOTMINSTAR:
  case OP_NOTPLUS:
  case OP_NOTMINPLUS:
  case OP_NOTPOSSTAR:
  case OP_NOTPOSPLUS:

  case OP_NOTSTARI:
  case OP_NOTMINSTARI:
  case OP_NOTPLUSI:
  case OP_NOTMINPLUSI:
  case OP_NOTPOSSTARI:
  case OP_NOTPOSPLUSI:
  return TRUE;

  case OP_CLASS:
  case OP_NCLASS:
#if defined SUPPORT_UNICODE || PCRE2_CODE_UNIT_WIDTH != 8
  case OP_XCLASS:
  cc += (*cc == OP_XCLASS) ? GET(cc, 1) : (int)(1 + (32 / sizeof(PCRE2_UCHAR)));
#else
  cc += (1 + (32 / sizeof(PCRE2_UCHAR)));
#endif

  switch(*cc)
    {
    case OP_CRSTAR:
    case OP_CRMINSTAR:
    case OP_CRPLUS:
    case OP_CRMINPLUS:
    case OP_CRPOSSTAR:
    case OP_CRPOSPLUS:
    return TRUE;
    }
  break;
  }
return FALSE;
}