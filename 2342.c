static SLJIT_INLINE PCRE2_SPTR get_iterator_parameters(compiler_common *common, PCRE2_SPTR cc, PCRE2_UCHAR *opcode, PCRE2_UCHAR *type, sljit_u32 *max, sljit_u32 *exact, PCRE2_SPTR *end)
{
int class_len;

*opcode = *cc;
*exact = 0;

if (*opcode >= OP_STAR && *opcode <= OP_POSUPTO)
  {
  cc++;
  *type = OP_CHAR;
  }
else if (*opcode >= OP_STARI && *opcode <= OP_POSUPTOI)
  {
  cc++;
  *type = OP_CHARI;
  *opcode -= OP_STARI - OP_STAR;
  }
else if (*opcode >= OP_NOTSTAR && *opcode <= OP_NOTPOSUPTO)
  {
  cc++;
  *type = OP_NOT;
  *opcode -= OP_NOTSTAR - OP_STAR;
  }
else if (*opcode >= OP_NOTSTARI && *opcode <= OP_NOTPOSUPTOI)
  {
  cc++;
  *type = OP_NOTI;
  *opcode -= OP_NOTSTARI - OP_STAR;
  }
else if (*opcode >= OP_TYPESTAR && *opcode <= OP_TYPEPOSUPTO)
  {
  cc++;
  *opcode -= OP_TYPESTAR - OP_STAR;
  *type = OP_END;
  }
else
  {
  SLJIT_ASSERT(*opcode == OP_CLASS || *opcode == OP_NCLASS || *opcode == OP_XCLASS);
  *type = *opcode;
  cc++;
  class_len = (*type < OP_XCLASS) ? (int)(1 + (32 / sizeof(PCRE2_UCHAR))) : GET(cc, 0);
  *opcode = cc[class_len - 1];

  if (*opcode >= OP_CRSTAR && *opcode <= OP_CRMINQUERY)
    {
    *opcode -= OP_CRSTAR - OP_STAR;
    *end = cc + class_len;

    if (*opcode == OP_PLUS || *opcode == OP_MINPLUS)
      {
      *exact = 1;
      *opcode -= OP_PLUS - OP_STAR;
      }
    }
  else if (*opcode >= OP_CRPOSSTAR && *opcode <= OP_CRPOSQUERY)
    {
    *opcode -= OP_CRPOSSTAR - OP_POSSTAR;
    *end = cc + class_len;

    if (*opcode == OP_POSPLUS)
      {
      *exact = 1;
      *opcode = OP_POSSTAR;
      }
    }
  else
    {
    SLJIT_ASSERT(*opcode == OP_CRRANGE || *opcode == OP_CRMINRANGE || *opcode == OP_CRPOSRANGE);
    *max = GET2(cc, (class_len + IMM2_SIZE));
    *exact = GET2(cc, class_len);

    if (*max == 0)
      {
      if (*opcode == OP_CRPOSRANGE)
        *opcode = OP_POSSTAR;
      else
        *opcode -= OP_CRRANGE - OP_STAR;
      }
    else
      {
      *max -= *exact;
      if (*max == 0)
        *opcode = OP_EXACT;
      else if (*max == 1)
        {
        if (*opcode == OP_CRPOSRANGE)
          *opcode = OP_POSQUERY;
        else
          *opcode -= OP_CRRANGE - OP_QUERY;
        }
      else
        {
        if (*opcode == OP_CRPOSRANGE)
          *opcode = OP_POSUPTO;
        else
          *opcode -= OP_CRRANGE - OP_UPTO;
        }
      }
    *end = cc + class_len + 2 * IMM2_SIZE;
    }
  return cc;
  }

switch(*opcode)
  {
  case OP_EXACT:
  *exact = GET2(cc, 0);
  cc += IMM2_SIZE;
  break;

  case OP_PLUS:
  case OP_MINPLUS:
  *exact = 1;
  *opcode -= OP_PLUS - OP_STAR;
  break;

  case OP_POSPLUS:
  *exact = 1;
  *opcode = OP_POSSTAR;
  break;

  case OP_UPTO:
  case OP_MINUPTO:
  case OP_POSUPTO:
  *max = GET2(cc, 0);
  cc += IMM2_SIZE;
  break;
  }

if (*type == OP_END)
  {
  *type = *cc;
  *end = next_opcode(common, cc);
  cc++;
  return cc;
  }

*end = cc + 1;
#ifdef SUPPORT_UNICODE
if (common->utf && HAS_EXTRALEN(*cc)) *end += GET_EXTRALEN(*cc);
#endif
return cc;
}