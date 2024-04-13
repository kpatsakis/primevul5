static void init_frame(compiler_common *common, PCRE2_SPTR cc, PCRE2_SPTR ccend, int stackpos, int stacktop)
{
DEFINE_COMPILER;
BOOL setsom_found = FALSE;
BOOL setmark_found = FALSE;
/* The last capture is a local variable even for recursions. */
BOOL capture_last_found = FALSE;
int offset;

/* >= 1 + shortest item size (2) */
SLJIT_UNUSED_ARG(stacktop);
SLJIT_ASSERT(stackpos >= stacktop + 2);

stackpos = STACK(stackpos);
if (ccend == NULL)
  {
  ccend = bracketend(cc) - (1 + LINK_SIZE);
  if (*cc != OP_CBRAPOS && *cc != OP_SCBRAPOS)
    cc = next_opcode(common, cc);
  }

SLJIT_ASSERT(cc != NULL);
while (cc < ccend)
  switch(*cc)
    {
    case OP_SET_SOM:
    SLJIT_ASSERT(common->has_set_som);
    if (!setsom_found)
      {
      OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), OVECTOR(0));
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), stackpos, SLJIT_IMM, -OVECTOR(0));
      stackpos -= (int)sizeof(sljit_sw);
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), stackpos, TMP1, 0);
      stackpos -= (int)sizeof(sljit_sw);
      setsom_found = TRUE;
      }
    cc += 1;
    break;

    case OP_MARK:
    case OP_COMMIT_ARG:
    case OP_PRUNE_ARG:
    case OP_THEN_ARG:
    SLJIT_ASSERT(common->mark_ptr != 0);
    if (!setmark_found)
      {
      OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), common->mark_ptr);
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), stackpos, SLJIT_IMM, -common->mark_ptr);
      stackpos -= (int)sizeof(sljit_sw);
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), stackpos, TMP1, 0);
      stackpos -= (int)sizeof(sljit_sw);
      setmark_found = TRUE;
      }
    cc += 1 + 2 + cc[1];
    break;

    case OP_RECURSE:
    if (common->has_set_som && !setsom_found)
      {
      OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), OVECTOR(0));
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), stackpos, SLJIT_IMM, -OVECTOR(0));
      stackpos -= (int)sizeof(sljit_sw);
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), stackpos, TMP1, 0);
      stackpos -= (int)sizeof(sljit_sw);
      setsom_found = TRUE;
      }
    if (common->mark_ptr != 0 && !setmark_found)
      {
      OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), common->mark_ptr);
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), stackpos, SLJIT_IMM, -common->mark_ptr);
      stackpos -= (int)sizeof(sljit_sw);
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), stackpos, TMP1, 0);
      stackpos -= (int)sizeof(sljit_sw);
      setmark_found = TRUE;
      }
    if (common->capture_last_ptr != 0 && !capture_last_found)
      {
      OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), common->capture_last_ptr);
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), stackpos, SLJIT_IMM, -common->capture_last_ptr);
      stackpos -= (int)sizeof(sljit_sw);
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), stackpos, TMP1, 0);
      stackpos -= (int)sizeof(sljit_sw);
      capture_last_found = TRUE;
      }
    cc += 1 + LINK_SIZE;
    break;

    case OP_CBRA:
    case OP_CBRAPOS:
    case OP_SCBRA:
    case OP_SCBRAPOS:
    if (common->capture_last_ptr != 0 && !capture_last_found)
      {
      OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), common->capture_last_ptr);
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), stackpos, SLJIT_IMM, -common->capture_last_ptr);
      stackpos -= (int)sizeof(sljit_sw);
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), stackpos, TMP1, 0);
      stackpos -= (int)sizeof(sljit_sw);
      capture_last_found = TRUE;
      }
    offset = (GET2(cc, 1 + LINK_SIZE)) << 1;
    OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), stackpos, SLJIT_IMM, OVECTOR(offset));
    stackpos -= (int)sizeof(sljit_sw);
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset));
    OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset + 1));
    OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), stackpos, TMP1, 0);
    stackpos -= (int)sizeof(sljit_sw);
    OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), stackpos, TMP2, 0);
    stackpos -= (int)sizeof(sljit_sw);

    cc += 1 + LINK_SIZE + IMM2_SIZE;
    break;

    default:
    cc = next_opcode(common, cc);
    SLJIT_ASSERT(cc != NULL);
    break;
    }

OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), stackpos, SLJIT_IMM, 0);
SLJIT_ASSERT(stackpos == STACK(stacktop));
}