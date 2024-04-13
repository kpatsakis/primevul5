kwsalloc (char const *trans)
{
  struct kwset *kwset = xmalloc (sizeof *kwset);

  obstack_init (&kwset->obstack);
  kwset->words = 0;
  kwset->trie = obstack_alloc (&kwset->obstack, sizeof *kwset->trie);
  kwset->trie->accepting = 0;
  kwset->trie->links = NULL;
  kwset->trie->parent = NULL;
  kwset->trie->next = NULL;
  kwset->trie->fail = NULL;
  kwset->trie->depth = 0;
  kwset->trie->shift = 0;
  kwset->mind = INT_MAX;
  kwset->maxd = -1;
  kwset->target = NULL;
  kwset->trans = trans;

  return kwset;
}