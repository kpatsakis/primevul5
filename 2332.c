static SLJIT_INLINE void reset_early_fail(compiler_common *common)
{
DEFINE_COMPILER;
sljit_u32 size = (sljit_u32)(common->early_fail_end_ptr - common->early_fail_start_ptr);
sljit_u32 uncleared_size;
sljit_s32 src = SLJIT_IMM;
sljit_s32 i;
struct sljit_label *loop;

SLJIT_ASSERT(common->early_fail_start_ptr < common->early_fail_end_ptr);

if (size == sizeof(sljit_sw))
  {
  OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), common->early_fail_start_ptr, SLJIT_IMM, 0);
  return;
  }

if (sljit_get_register_index(TMP3) >= 0 && !sljit_has_cpu_feature(SLJIT_HAS_ZERO_REGISTER))
  {
  OP1(SLJIT_MOV, TMP3, 0, SLJIT_IMM, 0);
  src = TMP3;
  }

if (size <= 6 * sizeof(sljit_sw))
  {
  for (i = common->early_fail_start_ptr; i < common->early_fail_end_ptr; i += sizeof(sljit_sw))
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), i, src, 0);
  return;
  }

GET_LOCAL_BASE(TMP1, 0, common->early_fail_start_ptr);

uncleared_size = ((size / sizeof(sljit_sw)) % 3) * sizeof(sljit_sw);

OP2(SLJIT_ADD, TMP2, 0, TMP1, 0, SLJIT_IMM, size - uncleared_size);

loop = LABEL();
OP1(SLJIT_MOV, SLJIT_MEM1(TMP1), 0, src, 0);
OP2(SLJIT_ADD, TMP1, 0, TMP1, 0, SLJIT_IMM, 3 * sizeof(sljit_sw));
OP1(SLJIT_MOV, SLJIT_MEM1(TMP1), -2 * (sljit_sw)sizeof(sljit_sw), src, 0);
OP1(SLJIT_MOV, SLJIT_MEM1(TMP1), -1 * (sljit_sw)sizeof(sljit_sw), src, 0);
CMPTO(SLJIT_LESS, TMP1, 0, TMP2, 0, loop);

if (uncleared_size >= sizeof(sljit_sw))
  OP1(SLJIT_MOV, SLJIT_MEM1(TMP1), 0, src, 0);

if (uncleared_size >= 2 * sizeof(sljit_sw))
  OP1(SLJIT_MOV, SLJIT_MEM1(TMP1), sizeof(sljit_sw), src, 0);
}