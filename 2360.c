static void do_getucd(compiler_common *common)
{
/* Search the UCD record for the character comes in TMP1.
Returns chartype in TMP1 and UCD offset in TMP2. */
DEFINE_COMPILER;
#if PCRE2_CODE_UNIT_WIDTH == 32
struct sljit_jump *jump;
#endif

#if defined SLJIT_DEBUG && SLJIT_DEBUG
/* dummy_ucd_record */
const ucd_record *record = GET_UCD(UNASSIGNED_UTF_CHAR);
SLJIT_ASSERT(record->script == ucp_Unknown && record->chartype == ucp_Cn && record->gbprop == ucp_gbOther);
SLJIT_ASSERT(record->caseset == 0 && record->other_case == 0);
#endif

SLJIT_ASSERT(UCD_BLOCK_SIZE == 128 && sizeof(ucd_record) == 12);

sljit_emit_fast_enter(compiler, RETURN_ADDR, 0);

#if PCRE2_CODE_UNIT_WIDTH == 32
if (!common->utf)
  {
  jump = CMP(SLJIT_LESS, TMP1, 0, SLJIT_IMM, MAX_UTF_CODE_POINT + 1);
  OP1(SLJIT_MOV, TMP1, 0, SLJIT_IMM, UNASSIGNED_UTF_CHAR);
  JUMPHERE(jump);
  }
#endif

OP2(SLJIT_LSHR, TMP2, 0, TMP1, 0, SLJIT_IMM, UCD_BLOCK_SHIFT);
OP2(SLJIT_SHL, TMP2, 0, TMP2, 0, SLJIT_IMM, 1);
OP1(SLJIT_MOV_U16, TMP2, 0, SLJIT_MEM1(TMP2), (sljit_sw)PRIV(ucd_stage1));
OP2(SLJIT_AND, TMP1, 0, TMP1, 0, SLJIT_IMM, UCD_BLOCK_MASK);
OP2(SLJIT_SHL, TMP2, 0, TMP2, 0, SLJIT_IMM, UCD_BLOCK_SHIFT);
OP2(SLJIT_ADD, TMP1, 0, TMP1, 0, TMP2, 0);
OP1(SLJIT_MOV, TMP2, 0, SLJIT_IMM, (sljit_sw)PRIV(ucd_stage2));
OP1(SLJIT_MOV_U16, TMP2, 0, SLJIT_MEM2(TMP2, TMP1), 1);
OP_SRC(SLJIT_FAST_RETURN, RETURN_ADDR, 0);
}