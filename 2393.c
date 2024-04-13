static void flush_stubs(compiler_common *common)
{
DEFINE_COMPILER;
stub_list *list_item = common->stubs;

while (list_item)
  {
  JUMPHERE(list_item->start);
  add_jump(compiler, &common->stackalloc, JUMP(SLJIT_FAST_CALL));
  JUMPTO(SLJIT_JUMP, list_item->quit);
  list_item = list_item->next;
  }
common->stubs = NULL;
}