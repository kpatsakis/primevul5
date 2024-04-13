static SLJIT_INLINE void set_jumps(jump_list *list, struct sljit_label *label)
{
while (list)
  {
  /* sljit_set_label is clever enough to do nothing
  if either the jump or the label is NULL. */
  SET_LABEL(list->jump, label);
  list = list->next;
  }
}