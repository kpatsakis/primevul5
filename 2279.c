static SLJIT_INLINE void add_jump(struct sljit_compiler *compiler, jump_list **list, struct sljit_jump *jump)
{
jump_list *list_item = sljit_alloc_memory(compiler, sizeof(jump_list));
if (list_item)
  {
  list_item->next = *list;
  list_item->jump = jump;
  *list = list_item;
  }
}