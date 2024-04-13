static void add_stub(compiler_common *common, struct sljit_jump *start)
{
DEFINE_COMPILER;
stub_list *list_item = sljit_alloc_memory(compiler, sizeof(stub_list));

if (list_item)
  {
  list_item->start = start;
  list_item->quit = LABEL();
  list_item->next = common->stubs;
  common->stubs = list_item;
  }
}