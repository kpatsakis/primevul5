static void do_revertframes(compiler_common *common)
{
DEFINE_COMPILER;
struct sljit_jump *jump;
struct sljit_label *mainloop;

sljit_emit_fast_enter(compiler, RETURN_ADDR, 0);
GET_LOCAL_BASE(TMP1, 0, 0);

/* Drop frames until we reach STACK_TOP. */
mainloop = LABEL();
OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(STACK_TOP), -sizeof(sljit_sw));
jump = CMP(SLJIT_SIG_LESS_EQUAL, TMP2, 0, SLJIT_IMM, 0);

OP2(SLJIT_ADD, TMP2, 0, TMP2, 0, TMP1, 0);
if (HAS_VIRTUAL_REGISTERS)
  {
  OP1(SLJIT_MOV, SLJIT_MEM1(TMP2), 0, SLJIT_MEM1(STACK_TOP), -(2 * sizeof(sljit_sw)));
  OP1(SLJIT_MOV, SLJIT_MEM1(TMP2), sizeof(sljit_sw), SLJIT_MEM1(STACK_TOP), -(3 * sizeof(sljit_sw)));
  OP2(SLJIT_SUB, STACK_TOP, 0, STACK_TOP, 0, SLJIT_IMM, 3 * sizeof(sljit_sw));
  }
else
  {
  OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(STACK_TOP), -(2 * sizeof(sljit_sw)));
  OP1(SLJIT_MOV, TMP3, 0, SLJIT_MEM1(STACK_TOP), -(3 * sizeof(sljit_sw)));
  OP2(SLJIT_SUB, STACK_TOP, 0, STACK_TOP, 0, SLJIT_IMM, 3 * sizeof(sljit_sw));
  OP1(SLJIT_MOV, SLJIT_MEM1(TMP2), 0, TMP1, 0);
  GET_LOCAL_BASE(TMP1, 0, 0);
  OP1(SLJIT_MOV, SLJIT_MEM1(TMP2), sizeof(sljit_sw), TMP3, 0);
  }
JUMPTO(SLJIT_JUMP, mainloop);

JUMPHERE(jump);
jump = CMP(SLJIT_NOT_ZERO /* SIG_LESS */, TMP2, 0, SLJIT_IMM, 0);
/* End of reverting values. */
OP_SRC(SLJIT_FAST_RETURN, RETURN_ADDR, 0);

JUMPHERE(jump);
OP1(SLJIT_NEG, TMP2, 0, TMP2, 0);
OP2(SLJIT_ADD, TMP2, 0, TMP2, 0, TMP1, 0);
if (HAS_VIRTUAL_REGISTERS)
  {
  OP1(SLJIT_MOV, SLJIT_MEM1(TMP2), 0, SLJIT_MEM1(STACK_TOP), -(2 * sizeof(sljit_sw)));
  OP2(SLJIT_SUB, STACK_TOP, 0, STACK_TOP, 0, SLJIT_IMM, 2 * sizeof(sljit_sw));
  }
else
  {
  OP1(SLJIT_MOV, TMP3, 0, SLJIT_MEM1(STACK_TOP), -(2 * sizeof(sljit_sw)));
  OP2(SLJIT_SUB, STACK_TOP, 0, STACK_TOP, 0, SLJIT_IMM, 2 * sizeof(sljit_sw));
  OP1(SLJIT_MOV, SLJIT_MEM1(TMP2), 0, TMP3, 0);
  }
JUMPTO(SLJIT_JUMP, mainloop);
}