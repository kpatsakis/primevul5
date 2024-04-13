static void compile_iterator_backtrackingpath(compiler_common *common, struct backtrack_common *current)
{
DEFINE_COMPILER;
PCRE2_SPTR cc = current->cc;
PCRE2_UCHAR opcode;
PCRE2_UCHAR type;
sljit_u32 max = 0, exact;
struct sljit_label *label = NULL;
struct sljit_jump *jump = NULL;
jump_list *jumplist = NULL;
PCRE2_SPTR end;
int private_data_ptr = PRIVATE_DATA(cc);
int base = (private_data_ptr == 0) ? SLJIT_MEM1(STACK_TOP) : SLJIT_MEM1(SLJIT_SP);
int offset0 = (private_data_ptr == 0) ? STACK(0) : private_data_ptr;
int offset1 = (private_data_ptr == 0) ? STACK(1) : private_data_ptr + (int)sizeof(sljit_sw);

cc = get_iterator_parameters(common, cc, &opcode, &type, &max, &exact, &end);

switch(opcode)
  {
  case OP_STAR:
  case OP_UPTO:
  if (type == OP_ANYNL || type == OP_EXTUNI)
    {
    SLJIT_ASSERT(private_data_ptr == 0);
    set_jumps(CURRENT_AS(char_iterator_backtrack)->u.backtracks, LABEL());
    OP1(SLJIT_MOV, STR_PTR, 0, SLJIT_MEM1(STACK_TOP), STACK(0));
    free_stack(common, 1);
    CMPTO(SLJIT_NOT_EQUAL, STR_PTR, 0, SLJIT_IMM, 0, CURRENT_AS(char_iterator_backtrack)->matchingpath);
    }
  else
    {
    if (CURRENT_AS(char_iterator_backtrack)->u.charpos.enabled)
      {
      OP1(SLJIT_MOV, STR_PTR, 0, base, offset0);
      OP1(SLJIT_MOV, TMP2, 0, base, offset1);
      OP2(SLJIT_SUB, STR_PTR, 0, STR_PTR, 0, SLJIT_IMM, IN_UCHARS(1));

      jump = CMP(SLJIT_LESS_EQUAL, STR_PTR, 0, TMP2, 0);
      label = LABEL();
      OP1(MOV_UCHAR, TMP1, 0, SLJIT_MEM1(STR_PTR), IN_UCHARS(-1));
      OP1(SLJIT_MOV, base, offset0, STR_PTR, 0);
      if (CURRENT_AS(char_iterator_backtrack)->u.charpos.othercasebit != 0)
        OP2(SLJIT_OR, TMP1, 0, TMP1, 0, SLJIT_IMM, CURRENT_AS(char_iterator_backtrack)->u.charpos.othercasebit);
      CMPTO(SLJIT_EQUAL, TMP1, 0, SLJIT_IMM, CURRENT_AS(char_iterator_backtrack)->u.charpos.chr, CURRENT_AS(char_iterator_backtrack)->matchingpath);
      move_back(common, NULL, TRUE);
      CMPTO(SLJIT_GREATER, STR_PTR, 0, TMP2, 0, label);
      }
    else
      {
      OP1(SLJIT_MOV, STR_PTR, 0, base, offset0);
      jump = CMP(SLJIT_LESS_EQUAL, STR_PTR, 0, base, offset1);
      move_back(common, NULL, TRUE);
      OP1(SLJIT_MOV, base, offset0, STR_PTR, 0);
      JUMPTO(SLJIT_JUMP, CURRENT_AS(char_iterator_backtrack)->matchingpath);
      }
    JUMPHERE(jump);
    if (private_data_ptr == 0)
      free_stack(common, 2);
    }
  break;

  case OP_MINSTAR:
  OP1(SLJIT_MOV, STR_PTR, 0, base, offset0);
  compile_char1_matchingpath(common, type, cc, &jumplist, TRUE);
  OP1(SLJIT_MOV, base, offset0, STR_PTR, 0);
  JUMPTO(SLJIT_JUMP, CURRENT_AS(char_iterator_backtrack)->matchingpath);
  set_jumps(jumplist, LABEL());
  if (private_data_ptr == 0)
    free_stack(common, 1);
  break;

  case OP_MINUPTO:
  OP1(SLJIT_MOV, TMP1, 0, base, offset1);
  OP1(SLJIT_MOV, STR_PTR, 0, base, offset0);
  OP2(SLJIT_SUB | SLJIT_SET_Z, TMP1, 0, TMP1, 0, SLJIT_IMM, 1);
  add_jump(compiler, &jumplist, JUMP(SLJIT_ZERO));

  OP1(SLJIT_MOV, base, offset1, TMP1, 0);
  compile_char1_matchingpath(common, type, cc, &jumplist, TRUE);
  OP1(SLJIT_MOV, base, offset0, STR_PTR, 0);
  JUMPTO(SLJIT_JUMP, CURRENT_AS(char_iterator_backtrack)->matchingpath);

  set_jumps(jumplist, LABEL());
  if (private_data_ptr == 0)
    free_stack(common, 2);
  break;

  case OP_QUERY:
  OP1(SLJIT_MOV, STR_PTR, 0, base, offset0);
  OP1(SLJIT_MOV, base, offset0, SLJIT_IMM, 0);
  CMPTO(SLJIT_NOT_EQUAL, STR_PTR, 0, SLJIT_IMM, 0, CURRENT_AS(char_iterator_backtrack)->matchingpath);
  jump = JUMP(SLJIT_JUMP);
  set_jumps(CURRENT_AS(char_iterator_backtrack)->u.backtracks, LABEL());
  OP1(SLJIT_MOV, STR_PTR, 0, base, offset0);
  OP1(SLJIT_MOV, base, offset0, SLJIT_IMM, 0);
  JUMPTO(SLJIT_JUMP, CURRENT_AS(char_iterator_backtrack)->matchingpath);
  JUMPHERE(jump);
  if (private_data_ptr == 0)
    free_stack(common, 1);
  break;

  case OP_MINQUERY:
  OP1(SLJIT_MOV, STR_PTR, 0, base, offset0);
  OP1(SLJIT_MOV, base, offset0, SLJIT_IMM, 0);
  jump = CMP(SLJIT_EQUAL, STR_PTR, 0, SLJIT_IMM, 0);
  compile_char1_matchingpath(common, type, cc, &jumplist, TRUE);
  JUMPTO(SLJIT_JUMP, CURRENT_AS(char_iterator_backtrack)->matchingpath);
  set_jumps(jumplist, LABEL());
  JUMPHERE(jump);
  if (private_data_ptr == 0)
    free_stack(common, 1);
  break;

  case OP_EXACT:
  case OP_POSSTAR:
  case OP_POSQUERY:
  case OP_POSUPTO:
  break;

  default:
  SLJIT_UNREACHABLE();
  break;
  }

set_jumps(current->topbacktracks, LABEL());
}