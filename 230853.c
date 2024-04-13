static PCRE2_SPTR byte_sequence_compare(compiler_common *common, BOOL caseless, PCRE2_SPTR cc,
    compare_context *context, jump_list **backtracks)
{
DEFINE_COMPILER;
unsigned int othercasebit = 0;
PCRE2_SPTR othercasechar = NULL;
#ifdef SUPPORT_UNICODE
int utflength;
#endif

if (caseless && char_has_othercase(common, cc))
  {
  othercasebit = char_get_othercase_bit(common, cc);
  SLJIT_ASSERT(othercasebit);
  /* Extracting bit difference info. */
#if PCRE2_CODE_UNIT_WIDTH == 8
  othercasechar = cc + (othercasebit >> 8);
  othercasebit &= 0xff;
#elif PCRE2_CODE_UNIT_WIDTH == 16 || PCRE2_CODE_UNIT_WIDTH == 32
  /* Note that this code only handles characters in the BMP. If there
  ever are characters outside the BMP whose othercase differs in only one
  bit from itself (there currently are none), this code will need to be
  revised for PCRE2_CODE_UNIT_WIDTH == 32. */
  othercasechar = cc + (othercasebit >> 9);
  if ((othercasebit & 0x100) != 0)
    othercasebit = (othercasebit & 0xff) << 8;
  else
    othercasebit &= 0xff;
#endif /* PCRE2_CODE_UNIT_WIDTH == [8|16|32] */
  }

if (context->sourcereg == -1)
  {
#if PCRE2_CODE_UNIT_WIDTH == 8
#if defined SLJIT_UNALIGNED && SLJIT_UNALIGNED
  if (context->length >= 4)
    OP1(SLJIT_MOV_S32, TMP1, 0, SLJIT_MEM1(STR_PTR), -context->length);
  else if (context->length >= 2)
    OP1(SLJIT_MOV_U16, TMP1, 0, SLJIT_MEM1(STR_PTR), -context->length);
  else
#endif
    OP1(SLJIT_MOV_U8, TMP1, 0, SLJIT_MEM1(STR_PTR), -context->length);
#elif PCRE2_CODE_UNIT_WIDTH == 16
#if defined SLJIT_UNALIGNED && SLJIT_UNALIGNED
  if (context->length >= 4)
    OP1(SLJIT_MOV_S32, TMP1, 0, SLJIT_MEM1(STR_PTR), -context->length);
  else
#endif
    OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), -context->length);
#elif PCRE2_CODE_UNIT_WIDTH == 32
  OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), -context->length);
#endif /* PCRE2_CODE_UNIT_WIDTH == [8|16|32] */
  context->sourcereg = TMP2;
  }

#ifdef SUPPORT_UNICODE
utflength = 1;
if (common->utf && HAS_EXTRALEN(*cc))
  utflength += GET_EXTRALEN(*cc);

do
  {
#endif

  context->length -= IN_UCHARS(1);
#if (defined SLJIT_UNALIGNED && SLJIT_UNALIGNED) && (PCRE2_CODE_UNIT_WIDTH == 8 || PCRE2_CODE_UNIT_WIDTH == 16)

  /* Unaligned read is supported. */
  if (othercasebit != 0 && othercasechar == cc)
    {
    context->c.asuchars[context->ucharptr] = *cc | othercasebit;
    context->oc.asuchars[context->ucharptr] = othercasebit;
    }
  else
    {
    context->c.asuchars[context->ucharptr] = *cc;
    context->oc.asuchars[context->ucharptr] = 0;
    }
  context->ucharptr++;

#if PCRE2_CODE_UNIT_WIDTH == 8
  if (context->ucharptr >= 4 || context->length == 0 || (context->ucharptr == 2 && context->length == 1))
#else
  if (context->ucharptr >= 2 || context->length == 0)
#endif
    {
    if (context->length >= 4)
      OP1(SLJIT_MOV_S32, context->sourcereg, 0, SLJIT_MEM1(STR_PTR), -context->length);
    else if (context->length >= 2)
      OP1(SLJIT_MOV_U16, context->sourcereg, 0, SLJIT_MEM1(STR_PTR), -context->length);
#if PCRE2_CODE_UNIT_WIDTH == 8
    else if (context->length >= 1)
      OP1(SLJIT_MOV_U8, context->sourcereg, 0, SLJIT_MEM1(STR_PTR), -context->length);
#endif /* PCRE2_CODE_UNIT_WIDTH == 8 */
    context->sourcereg = context->sourcereg == TMP1 ? TMP2 : TMP1;

    switch(context->ucharptr)
      {
      case 4 / sizeof(PCRE2_UCHAR):
      if (context->oc.asint != 0)
        OP2(SLJIT_OR, context->sourcereg, 0, context->sourcereg, 0, SLJIT_IMM, context->oc.asint);
      add_jump(compiler, backtracks, CMP(SLJIT_NOT_EQUAL, context->sourcereg, 0, SLJIT_IMM, context->c.asint | context->oc.asint));
      break;

      case 2 / sizeof(PCRE2_UCHAR):
      if (context->oc.asushort != 0)
        OP2(SLJIT_OR, context->sourcereg, 0, context->sourcereg, 0, SLJIT_IMM, context->oc.asushort);
      add_jump(compiler, backtracks, CMP(SLJIT_NOT_EQUAL, context->sourcereg, 0, SLJIT_IMM, context->c.asushort | context->oc.asushort));
      break;

#if PCRE2_CODE_UNIT_WIDTH == 8
      case 1:
      if (context->oc.asbyte != 0)
        OP2(SLJIT_OR, context->sourcereg, 0, context->sourcereg, 0, SLJIT_IMM, context->oc.asbyte);
      add_jump(compiler, backtracks, CMP(SLJIT_NOT_EQUAL, context->sourcereg, 0, SLJIT_IMM, context->c.asbyte | context->oc.asbyte));
      break;
#endif

      default:
      SLJIT_UNREACHABLE();
      break;
      }
    context->ucharptr = 0;
    }

#else

  /* Unaligned read is unsupported or in 32 bit mode. */
  if (context->length >= 1)
    OP1(MOV_UCHAR, context->sourcereg, 0, SLJIT_MEM1(STR_PTR), -context->length);

  context->sourcereg = context->sourcereg == TMP1 ? TMP2 : TMP1;

  if (othercasebit != 0 && othercasechar == cc)
    {
    OP2(SLJIT_OR, context->sourcereg, 0, context->sourcereg, 0, SLJIT_IMM, othercasebit);
    add_jump(compiler, backtracks, CMP(SLJIT_NOT_EQUAL, context->sourcereg, 0, SLJIT_IMM, *cc | othercasebit));
    }
  else
    add_jump(compiler, backtracks, CMP(SLJIT_NOT_EQUAL, context->sourcereg, 0, SLJIT_IMM, *cc));

#endif

  cc++;
#ifdef SUPPORT_UNICODE
  utflength--;
  }
while (utflength > 0);
#endif

return cc;
}