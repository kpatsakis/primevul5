treenext (struct tree const *tree, struct trie *next[])
{
  if (!tree)
    return;
  treenext(tree->llink, next);
  treenext(tree->rlink, next);
  next[tree->label] = tree->trie;
}