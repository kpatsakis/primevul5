static SLJIT_INLINE PCRE2_SPTR compile_callout_matchingpath(compiler_common *common, PCRE2_SPTR cc, backtrack_common *parent)
{
DEFINE_COMPILER;
backtrack_common *backtrack;
sljit_s32 mov_opcode;
unsigned int callout_length = (*cc == OP_CALLOUT)
    ? PRIV(OP_lengths)[OP_CALLOUT] : GET(cc, 1 + 2 * LINK_SIZE);
sljit_sw value1;
sljit_sw value2;
sljit_sw value3;
sljit_uw callout_arg_size = (common->re->top_bracket + 1) * 2 * sizeof(sljit_sw);

PUSH_BACKTRACK(sizeof(backtrack_common), cc, NULL);

callout_arg_size = (sizeof(pcre2_callout_block) + callout_arg_size + sizeof(sljit_sw) - 1) / sizeof(sljit_sw);

allocate_stack(common, callout_arg_size);

SLJIT_ASSERT(common->capture_last_ptr != 0);
OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(SLJIT_SP), common->capture_last_ptr);
OP1(SLJIT_MOV, TMP1, 0, ARGUMENTS, 0);
value1 = (*cc == OP_CALLOUT) ? cc[1 + 2 * LINK_SIZE] : 0;
OP1(SLJIT_MOV_U32, SLJIT_MEM1(STACK_TOP), CALLOUT_ARG_OFFSET(callout_number), SLJIT_IMM, value1);
OP1(SLJIT_MOV_U32, SLJIT_MEM1(STACK_TOP), CALLOUT_ARG_OFFSET(capture_last), TMP2, 0);
OP1(SLJIT_MOV_U32, SLJIT_MEM1(STACK_TOP), CALLOUT_ARG_OFFSET(capture_top), SLJIT_IMM, common->re->top_bracket + 1);

/* These pointer sized fields temporarly stores internal variables. */
OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), CALLOUT_ARG_OFFSET(offset_vector), STR_PTR, 0);

if (common->mark_ptr != 0)
  OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(TMP1), SLJIT_OFFSETOF(jit_arguments, mark_ptr));
mov_opcode = (sizeof(PCRE2_SIZE) == 4) ? SLJIT_MOV_U32 : SLJIT_MOV;
OP1(mov_opcode, SLJIT_MEM1(STACK_TOP), CALLOUT_ARG_OFFSET(pattern_position), SLJIT_IMM, GET(cc, 1));
OP1(mov_opcode, SLJIT_MEM1(STACK_TOP), CALLOUT_ARG_OFFSET(next_item_length), SLJIT_IMM, GET(cc, 1 + LINK_SIZE));

if (*cc == OP_CALLOUT)
  {
  value1 = 0;
  value2 = 0;
  value3 = 0;
  }
else
  {
  value1 = (sljit_sw) (cc + (1 + 4*LINK_SIZE) + 1);
  value2 = (callout_length - (1 + 4*LINK_SIZE + 2));
  value3 = (sljit_sw) (GET(cc, 1 + 3*LINK_SIZE));
  }

OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), CALLOUT_ARG_OFFSET(callout_string), SLJIT_IMM, value1);
OP1(mov_opcode, SLJIT_MEM1(STACK_TOP), CALLOUT_ARG_OFFSET(callout_string_length), SLJIT_IMM, value2);
OP1(mov_opcode, SLJIT_MEM1(STACK_TOP), CALLOUT_ARG_OFFSET(callout_string_offset), SLJIT_IMM, value3);
OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), CALLOUT_ARG_OFFSET(mark), (common->mark_ptr != 0) ? TMP2 : SLJIT_IMM, 0);

SLJIT_ASSERT(TMP1 == SLJIT_R0 && STR_PTR == SLJIT_R1);

/* Needed to save important temporary registers. */
OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), LOCALS0, STR_PTR, 0);
/* SLJIT_R0 = arguments */
OP1(SLJIT_MOV, SLJIT_R1, 0, STACK_TOP, 0);
GET_LOCAL_BASE(SLJIT_R2, 0, OVECTOR_START);
sljit_emit_icall(compiler, SLJIT_CALL, SLJIT_ARGS3(32, W, W, W), SLJIT_IMM, SLJIT_FUNC_ADDR(do_callout));
OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_MEM1(SLJIT_SP), LOCALS0);
free_stack(common, callout_arg_size);

/* Check return value. */
OP2U(SLJIT_SUB32 | SLJIT_SET_Z | SLJIT_SET_SIG_GREATER, SLJIT_RETURN_REG, 0, SLJIT_IMM, 0);
add_jump(compiler, &backtrack->topbacktracks, JUMP(SLJIT_SIG_GREATER));
if (common->abort_label == NULL)
  add_jump(compiler, &common->abort, JUMP(SLJIT_NOT_EQUAL) /* SIG_LESS */);
else
  JUMPTO(SLJIT_NOT_EQUAL /* SIG_LESS */, common->abort_label);
return cc + callout_length;
}