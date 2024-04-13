static void compile_dnref_search(compiler_common *common, PCRE2_SPTR cc, jump_list **backtracks)
{
/* The OVECTOR offset goes to TMP2. */
DEFINE_COMPILER;
int count = GET2(cc, 1 + IMM2_SIZE);
PCRE2_SPTR slot = common->name_table + GET2(cc, 1) * common->name_entry_size;
unsigned int offset;
jump_list *found = NULL;

SLJIT_ASSERT(*cc == OP_DNREF || *cc == OP_DNREFI);

OP1(SLJIT_MOV, TMP1, 0, SLJIT_MEM1(SLJIT_SP), OVECTOR(1));

count--;
while (count-- > 0)
  {
  offset = GET2(slot, 0) << 1;
  GET_LOCAL_BASE(TMP2, 0, OVECTOR(offset));
  add_jump(compiler, &found, CMP(SLJIT_NOT_EQUAL, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset), TMP1, 0));
  slot += common->name_entry_size;
  }

offset = GET2(slot, 0) << 1;
GET_LOCAL_BASE(TMP2, 0, OVECTOR(offset));
if (backtracks != NULL && !common->unset_backref)
  add_jump(compiler, backtracks, CMP(SLJIT_EQUAL, SLJIT_MEM1(SLJIT_SP), OVECTOR(offset), TMP1, 0));

set_jumps(found, LABEL());
}