static void do_caselesscmp(compiler_common *common)
{
DEFINE_COMPILER;
struct sljit_jump *jump;
struct sljit_label *label;
int char1_reg = STR_END;
int char2_reg;
int lcc_table;
int opt_type = 0;

if (sljit_get_register_index(TMP3) < 0)
  {
  char2_reg = STACK_TOP;
  lcc_table = STACK_LIMIT;
  }
else
  {
  char2_reg = RETURN_ADDR;
  lcc_table = TMP3;
  }

if (sljit_emit_mem(compiler, MOV_UCHAR | SLJIT_MEM_SUPP | SLJIT_MEM_POST, char1_reg, SLJIT_MEM1(TMP1), IN_UCHARS(1)) == SLJIT_SUCCESS)
  opt_type = 1;
else if (sljit_emit_mem(compiler, MOV_UCHAR | SLJIT_MEM_SUPP | SLJIT_MEM_PRE, char1_reg, SLJIT_MEM1(TMP1), IN_UCHARS(1)) == SLJIT_SUCCESS)
  opt_type = 2;

sljit_emit_fast_enter(compiler, SLJIT_MEM1(SLJIT_SP), LOCALS0);
OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, TMP2, 0);

OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), LOCALS1, char1_reg, 0);

if (char2_reg == STACK_TOP)
  {
  OP1(SLJIT_MOV, TMP3, 0, char2_reg, 0);
  OP1(SLJIT_MOV, RETURN_ADDR, 0, lcc_table, 0);
  }

OP1(SLJIT_MOV, lcc_table, 0, SLJIT_IMM, common->lcc);

if (opt_type == 1)
  {
  label = LABEL();
  sljit_emit_mem(compiler, MOV_UCHAR | SLJIT_MEM_POST, char1_reg, SLJIT_MEM1(TMP1), IN_UCHARS(1));
  sljit_emit_mem(compiler, MOV_UCHAR | SLJIT_MEM_POST, char2_reg, SLJIT_MEM1(STR_PTR), IN_UCHARS(1));
  }
else if (opt_type == 2)
  {
  OP2(SLJIT_SUB, TMP1, 0, TMP1, 0, SLJIT_IMM, IN_UCHARS(1));
  OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));

  label = LABEL();
  sljit_emit_mem(compiler, MOV_UCHAR | SLJIT_MEM_PRE, char1_reg, SLJIT_MEM1(TMP1), IN_UCHARS(1));
  sljit_emit_mem(compiler, MOV_UCHAR | SLJIT_MEM_PRE, char2_reg, SLJIT_MEM1(STR_PTR), IN_UCHARS(1));
  }
else
  {
  label = LABEL();
  OP1(MOV_UCHAR, char1_reg, 0, SLJIT_MEM1(TMP1), 0);
  OP1(MOV_UCHAR, char2_reg, 0, SLJIT_MEM1(STR_PTR), 0);
  OP2(SLJIT_ADD, TMP1, 0, TMP1, 0, SLJIT_IMM, IN_UCHARS(1));
  }

#if PCRE2_CODE_UNIT_WIDTH != 8
jump = CMP(SLJIT_GREATER, char1_reg, 0, SLJIT_IMM, 255);
#endif
OP1(SLJIT_MOV_U8, char1_reg, 0, SLJIT_MEM2(lcc_table, char1_reg), 0);
#if PCRE2_CODE_UNIT_WIDTH != 8
JUMPHERE(jump);
jump = CMP(SLJIT_GREATER, char2_reg, 0, SLJIT_IMM, 255);
#endif
OP1(SLJIT_MOV_U8, char2_reg, 0, SLJIT_MEM2(lcc_table, char2_reg), 0);
#if PCRE2_CODE_UNIT_WIDTH != 8
JUMPHERE(jump);
#endif

if (opt_type == 0)
  OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));

jump = CMP(SLJIT_NOT_EQUAL, char1_reg, 0, char2_reg, 0);
OP2(SLJIT_SUB | SLJIT_SET_Z, TMP2, 0, TMP2, 0, SLJIT_IMM, IN_UCHARS(1));
JUMPTO(SLJIT_NOT_ZERO, label);

JUMPHERE(jump);
OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), LOCALS0);

if (opt_type == 2)
  OP2(SLJIT_ADD, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));

if (char2_reg == STACK_TOP)
  {
  OP1(SLJIT_MOV, char2_reg, 0, TMP3, 0);
  OP1(SLJIT_MOV, lcc_table, 0, RETURN_ADDR, 0);
  }

OP1(SLJIT_MOV, char1_reg, 0, SLJIT_MEM1(SLJIT_SP), LOCALS1);
sljit_emit_fast_return(compiler, TMP1, 0);
}