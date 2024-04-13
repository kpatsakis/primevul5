static SLJIT_INLINE struct sljit_jump *search_requested_char(compiler_common *common, PCRE2_UCHAR req_char, BOOL caseless, BOOL has_firstchar)
{
DEFINE_COMPILER;
struct sljit_label *loop;
struct sljit_jump *toolong;
struct sljit_jump *alreadyfound;
struct sljit_jump *found;
struct sljit_jump *foundoc = NULL;
struct sljit_jump *notfound;
sljit_u32 oc, bit;

SLJIT_ASSERT(common->req_char_ptr != 0);
OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(SLJIT_SP), common->req_char_ptr);
OP2(SLJIT_ADD, TMP1, 0, STR_PTR, 0, SLJIT_IMM, REQ_CU_MAX);
toolong = CMP(SLJIT_LESS, TMP1, 0, STR_END, 0);
alreadyfound = CMP(SLJIT_LESS, STR_PTR, 0, TMP2, 0);

if (has_firstchar)
  OP2(SLJIT_ADD, TMP1, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
else
  OP1(SLJIT_MOV, TMP1, 0, STR_PTR, 0);

loop = LABEL();
notfound = CMP(SLJIT_GREATER_EQUAL, TMP1, 0, STR_END, 0);

OP1(MOV_UCHAR, TMP2, 0, SLJIT_MEM1(TMP1), 0);
oc = req_char;
if (caseless)
  {
  oc = TABLE_GET(req_char, common->fcc, req_char);
#if defined SUPPORT_UNICODE && PCRE2_CODE_UNIT_WIDTH != 8
  if (req_char > 127 && common->utf)
    oc = UCD_OTHERCASE(req_char);
#endif
  }
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
    foundoc = CMP(SLJIT_EQUAL, TMP2, 0, SLJIT_IMM, oc);
    }
  }
OP2(SLJIT_ADD, TMP1, 0, TMP1, 0, SLJIT_IMM, IN_UCHARS(1));
JUMPTO(SLJIT_JUMP, loop);

JUMPHERE(found);
if (foundoc)
  JUMPHERE(foundoc);
OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->req_char_ptr, TMP1, 0);
JUMPHERE(alreadyfound);
JUMPHERE(toolong);
return notfound;
}