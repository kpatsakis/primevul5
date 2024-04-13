static void add_label_addr(compiler_common *common, sljit_uw *update_addr)
{
DEFINE_COMPILER;
label_addr_list *label_addr;

label_addr = sljit_alloc_memory(compiler, sizeof(label_addr_list));
if (label_addr == NULL)
  return;
label_addr->label = LABEL();
label_addr->update_addr = update_addr;
label_addr->next = common->label_addrs;
common->label_addrs = label_addr;
}