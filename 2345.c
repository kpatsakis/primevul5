static SLJIT_INLINE PCRE2_SPTR compile_ref_iterator_matchingpath(compiler_common *common, PCRE2_SPTR cc, backtrack_common *parent)
{
DEFINE_COMPILER;
BOOL ref = (*cc == OP_REF || *cc == OP_REFI);
backtrack_common *backtrack;
PCRE2_UCHAR type;
int offset = 0;
struct sljit_label *label;
struct sljit_jump *zerolength;
struct sljit_jump *jump = NULL;
PCRE2_SPTR ccbegin = cc;
int min = 0, max = 0;
BOOL minimize;

PUSH_BACKTRACK(sizeof(ref_iterator_backtrack), cc, NULL);

if (ref)
  offset = GET2(cc, 1) << 1;
else
  cc += IMM2_SIZE;
type = cc[1 + IMM2_SIZE];

SLJIT_COMPILE_ASSERT((OP_CRSTAR & 0x1) == 0, crstar_opcode_must_be_even);
minimize = (type & 0x1) != 0;
switch(type)
  {
  case OP_CRSTAR:
  case OP_CRMINSTAR:
  min = 0;
  max = 0;
  cc += 1 + IMM2_SIZE + 1;
  break;
  case OP_CRPLUS:
  case OP_CRMINPLUS:
  min = 1;
  max = 0;
  cc += 1 + IMM2_SIZE + 1;
  break;
  case OP_CRQUERY:
  case OP_CRMINQUERY:
  min = 0;
  max = 1;
  cc += 1 + IMM2_SIZE + 1;
  break;
  case OP_CRRANGE:
  case OP_CRMINRANGE:
  min = GET2(cc, 1 + IMM2_SIZE + 1);
  max = GET2(cc, 1 + IMM2_SIZE + 1 + IMM2_SIZE);
  cc += 1 + IMM2_SIZE + 1 + 2 * IMM2_SIZE;
  break;
  default:
  SLJIT_UNREACHABLE();
  break;
  }

if (!minimize)
  {
  if (min == 0)
    {
    allocate_stack(common, 2);
    if (ref)
      OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset));
    OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), STR_PTR, 0);
    OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(1), SLJIT_IMM, 0);
    /* Temporary release of STR_PTR. */
    OP2(SLJIT_ADD, STACK_TOP, 0, STACK_TOP, 0, SLJIT_IMM, sizeof(sljit_sw));
    /* Handles both invalid and empty cases. Since the minimum repeat,
    is zero the invalid case is basically the same as an empty case. */
    if (ref)
      zerolength = CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset + 1));
    else
      {
      compile_dnref_search(common, ccbegin, NULL);
      OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(TMP2), 0);
      OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), POSSESSIVE1, TMP2, 0);
      zerolength = CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_MEM1(TMP2), sizeof(sljit_sw));
      }
    /* Restore if not zero length. */
    OP2(SLJIT_SUB, STACK_TOP, 0, STACK_TOP, 0, SLJIT_IMM, sizeof(sljit_sw));
    }
  else
    {
    allocate_stack(common, 1);
    if (ref)
      OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset));
    OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), SLJIT_IMM, 0);
    if (ref)
      {
      add_jump(compiler, &backtrack->topbacktracks, CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_MEM1(SLJIT_SP), OVECTOR(1)));
      zerolength = CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset + 1));
      }
    else
      {
      compile_dnref_search(common, ccbegin, &backtrack->topbacktracks);
      OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(TMP2), 0);
      OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), POSSESSIVE1, TMP2, 0);
      zerolength = CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_MEM1(TMP2), sizeof(sljit_sw));
      }
    }

  if (min > 1 || max > 1)
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), POSSESSIVE0, SLJIT_IMM, 0);

  label = LABEL();
  if (!ref)
    OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(SLJIT_SP), POSSESSIVE1);
  compile_ref_matchingpath(common, ccbegin, &backtrack->topbacktracks, FALSE, FALSE);

  if (min > 1 || max > 1)
    {
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), POSSESSIVE0);
    OP2(SLJIT_ADD, TMP1, 0, TMP1, 0, SLJIT_IMM, 1);
    OP1(SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), POSSESSIVE0, TMP1, 0);
    if (min > 1)
      CMPTO(SLJIT_LESS, TMP1, 0, SLJIT_IMM, min, label);
    if (max > 1)
      {
      jump = CMP(SLJIT_GREATER_EQUAL, TMP1, 0, SLJIT_IMM, max);
      allocate_stack(common, 1);
      OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), STR_PTR, 0);
      JUMPTO(SLJIT_JUMP, label);
      JUMPHERE(jump);
      }
    }

  if (max == 0)
    {
    /* Includes min > 1 case as well. */
    allocate_stack(common, 1);
    OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), STR_PTR, 0);
    JUMPTO(SLJIT_JUMP, label);
    }

  JUMPHERE(zerolength);
  BACKTRACK_AS(ref_iterator_backtrack)->matchingpath = LABEL();

  count_match(common);
  return cc;
  }

allocate_stack(common, ref ? 2 : 3);
if (ref)
  OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset));
OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), SLJIT_IMM, 0);
if (type != OP_CRMINSTAR)
  OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(1), SLJIT_IMM, 0);

if (min == 0)
  {
  /* Handles both invalid and empty cases. Since the minimum repeat,
  is zero the invalid case is basically the same as an empty case. */
  if (ref)
    zerolength = CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset + 1));
  else
    {
    compile_dnref_search(common, ccbegin, NULL);
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(TMP2), 0);
    OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(2), TMP2, 0);
    zerolength = CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_MEM1(TMP2), sizeof(sljit_sw));
    }
  /* Length is non-zero, we can match real repeats. */
  OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), STR_PTR, 0);
  jump = JUMP(SLJIT_JUMP);
  }
else
  {
  if (ref)
    {
    add_jump(compiler, &backtrack->topbacktracks, CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_MEM1(SLJIT_SP), OVECTOR(1)));
    zerolength = CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset + 1));
    }
  else
    {
    compile_dnref_search(common, ccbegin, &backtrack->topbacktracks);
    OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(TMP2), 0);
    OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(2), TMP2, 0);
    zerolength = CMP(SLJIT_EQUAL, TMP1, 0, SLJIT_MEM1(TMP2), sizeof(sljit_sw));
    }
  }

BACKTRACK_AS(ref_iterator_backtrack)->matchingpath = LABEL();
if (max > 0)
  add_jump(compiler, &backtrack->topbacktracks, CMP(SLJIT_GREATER_EQUAL, SLJIT_MEM1(STACK_TOP), STACK(1), SLJIT_IMM, max));

if (!ref)
  OP1(SLJIT_MOV, TMP2, 0, SLJIT_MEM1(STACK_TOP), STACK(2));
compile_ref_matchingpath(common, ccbegin, &backtrack->topbacktracks, TRUE, TRUE);
OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(0), STR_PTR, 0);

if (min > 1)
  {
  OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(STACK_TOP), STACK(1));
  OP2(SLJIT_ADD, TMP1, 0, TMP1, 0, SLJIT_IMM, 1);
  OP1(SLJIT_MOV, SLJIT_MEM1(STACK_TOP), STACK(1), TMP1, 0);
  CMPTO(SLJIT_LESS, TMP1, 0, SLJIT_IMM, min, BACKTRACK_AS(ref_iterator_backtrack)->matchingpath);
  }
else if (max > 0)
  OP2(SLJIT_ADD, SLJIT_MEM1(STACK_TOP), STACK(1), SLJIT_MEM1(STACK_TOP), STACK(1), SLJIT_IMM, 1);

if (jump != NULL)
  JUMPHERE(jump);
JUMPHERE(zerolength);

count_match(common);
return cc;
}