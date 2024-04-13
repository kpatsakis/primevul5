static SLJIT_INLINE void reset_fast_fail(compiler_common *common)
{
DEFINE_COMPILER;
sljit_s32 i;

SLJIT_ASSERT(common->fast_fail_start_ptr < common->fast_fail_end_ptr);

OP2(SLJIT_SUB, TMP1, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
for (i = common->fast_fail_start_ptr; i < common->fast_fail_end_ptr; i += sizeof(sljit_sw))
  OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), i, TMP1, 0);
}