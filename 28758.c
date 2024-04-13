is_invalid_entry (struct fileinfo *f)
{
  struct fileinfo *cur = f;
  char *f_name = f->name;

  /* If the node we're currently checking has a duplicate later, we eliminate
   * the current node and leave the next one intact. */
  while (cur->next)
    {
      cur = cur->next;
      if (strcmp(f_name, cur->name) == 0)
          return true;
    }
  return false;
}
