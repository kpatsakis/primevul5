static SLJIT_INLINE jump_list *search_requested_char(compiler_common *common, PCRE2_UCHAR req_char, BOOL caseless, BOOL has_firstchar)
{
DEFINE_COMPILER;
struct sljit_label *loop;
struct sljit_jump *toolong;
struct sljit_jump *already_found;
struct sljit_jump *found;
struct sljit_jump *found_oc = NULL;
jump_list *not_found = NULL;
sljit_u32 oc, bit;

SLJIT_ASSERT(common->req_char_ptr != 0);
OP2(SLJIT_ADD, TMP2, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(REQ_CU_MAX) * 100);
OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), common->req_char_ptr);
toolong = CMP(SLJIT_LESS, TMP2, 0, STR_END, 0);
already_found = CMP(SLJIT_LESS, STR_PTR, 0, TMP1, 0);

if (has_firstchar)
  OP2(SLJIT_ADD, TMP1, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
else
  OP1(SLJIT_MOV, TMP1, 0, STR_PTR, 0);

oc = req_char;
if (caseless)
  {
  oc = TABLE_GET(req_char, common->fcc, req_char);
#if defined SUPPORT_UNICODE
  if (req_char > 127 && (common->utf || common->ucp))
    oc = UCD_OTHERCASE(req_char);
#endif
  }

#ifdef JIT_HAS_FAST_REQUESTED_CHAR_SIMD
if (JIT_HAS_FAST_REQUESTED_CHAR_SIMD)
  {
  not_found = fast_requested_char_simd(common, req_char, oc);
  }
else
#endif
  {
  loop = LABEL();
  add_jump(compiler, &not_found, CMP(SLJIT_GREATER_EQUAL, TMP1, 0, STR_END, 0));

  OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(TMP1), 0);

  if (req_char == oc)
    found = CMP(SLJIT_EQUAL, TMP2, 0, SLJIT_IMM, req_char);
  else
    {
    bit = req_char ^ oc;
    if (is_powerof2(bit))
      {
       OP2(SLJIT_OR, TMP2, 0, TMP2, 0, SLJIT_IMM, bit);
      found = CMP(SLJIT_EQUAL, TMP2, 0, SLJIT_IMM, req_char | bit);
      }
    else
      {
      found = CMP(SLJIT_EQUAL, TMP2, 0, SLJIT_IMM, req_char);
      found_oc = CMP(SLJIT_EQUAL, TMP2, 0, SLJIT_IMM, oc);
      }
    }
  OP2(SLJIT_ADD, TMP1, 0, TMP1, 0, SLJIT_IMM, IN_UCHARS(1));
  JUMPTO(SLJIT_JUMP, loop);

  JUMPHERE(found);
  if (found_oc)
    JUMPHERE(found_oc);
  }

OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->req_char_ptr, TMP1, 0);

JUMPHERE(already_found);
JUMPHERE(toolong);
return not_found;
}