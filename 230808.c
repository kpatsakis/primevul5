static void do_casefulcmp(compiler_common *common)
{
DEFINE_COMPILER;
struct sljit_jump *jump;
struct sljit_label *label;
int char1_reg;
int char2_reg;

if (sljit_get_register_index(TMP3) < 0)
  {
  char1_reg = STR_END;
  char2_reg = STACK_TOP;
  }
else
  {
  char1_reg = TMP3;
  char2_reg = RETURN_ADDR;
  }

sljit_emit_fast_enter(compiler, SLJIT_MEM1(SLJIT_SP), LOCALS0);
OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, TMP2, 0);

if (char1_reg == STR_END)
  {
  OP1(SLJIT_MOV, TMP3, 0, char1_reg, 0);
  OP1(SLJIT_MOV, RETURN_ADDR, 0, char2_reg, 0);
  }

if (sljit_emit_mem(compiler, MOV_UCHAR | SLJIT_MEM_SUPP | SLJIT_MEM_POST, char1_reg, SLJIT_MEM1(TMP1), IN_UCHARS(1)) == SLJIT_SUCCESS)
  {
  label = LABEL();
  sljit_emit_mem(compiler, MOV_UCHAR | SLJIT_MEM_POST, char1_reg, SLJIT_MEM1(TMP1), IN_UCHARS(1));
  sljit_emit_mem(compiler, MOV_UCHAR | SLJIT_MEM_POST, char2_reg, SLJIT_MEM1(STR_PTR), IN_UCHARS(1));
  jump = CMP(SLJIT_NOT_EQUAL, char1_reg, 0, char2_reg, 0);
  OP2(SLJIT_SUB | SLJIT_SET_Z, TMP2, 0, TMP2, 0, SLJIT_IMM, IN_UCHARS(1));
  JUMPTO(SLJIT_NOT_ZERO, label);

  JUMPHERE(jump);
  OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), LOCALS0);
  }
else if (sljit_emit_mem(compiler, MOV_UCHAR | SLJIT_MEM_SUPP | SLJIT_MEM_PRE, char1_reg, SLJIT_MEM1(TMP1), IN_UCHARS(1)) == SLJIT_SUCCESS)
  {
  OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, IN_UCHARS(1));
  OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));

  label = LABEL();
  sljit_emit_mem(compiler, MOV_UCHAR | SLJIT_MEM_PRE, char1_reg, SLJIT_MEM1(TMP1), IN_UCHARS(1));
  sljit_emit_mem(compiler, MOV_UCHAR | SLJIT_MEM_PRE, char2_reg, SLJIT_MEM1(STR_PTR), IN_UCHARS(1));
  jump = CMP(SLJIT_NOT_EQUAL, char1_reg, 0, char2_reg, 0);
  OP2(SLJIT_SUB | SLJIT_SET_Z, TMP2, 0, TMP2, 0, SLJIT_IMM, IN_UCHARS(1));
  JUMPTO(SLJIT_NOT_ZERO, label);

  JUMPHERE(jump);
  OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), LOCALS0);
  OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
  }
else
  {
  label = LABEL();
  OP1(MOV_UCHAR, char1_reg, 0, SLJIT_MEM1(TMP1), 0);
  OP1(MOV_UCHAR, char2_reg, 0, SLJIT_MEM1(STR_PTR), 0);
  OP2(SLJIT_ADD, TMP1, 0, TMP1, 0, SLJIT_IMM, IN_UCHARS(1));
  OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));
  jump = CMP(SLJIT_NOT_EQUAL, char1_reg, 0, char2_reg, 0);
  OP2(SLJIT_SUB | SLJIT_SET_Z, TMP2, 0, TMP2, 0, SLJIT_IMM, IN_UCHARS(1));
  JUMPTO(SLJIT_NOT_ZERO, label);

  JUMPHERE(jump);
  OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), LOCALS0);
  }

if (char1_reg == STR_END)
  {
  OP1(SLJIT_MOV, char1_reg, 0, TMP3, 0);
  OP1(SLJIT_MOV, char2_reg, 0, RETURN_ADDR, 0);
  }

sljit_emit_fast_return(compiler, TMP1, 0);
}