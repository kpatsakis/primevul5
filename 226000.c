static void AddNodePixelList(PixelList *pixel_list,const size_t color)
{
  SkipList
    *p;

  ssize_t
    level;

  size_t
    search,
    update[9];

  /*
    Initialize the node.
  */
  p=(&pixel_list->skip_list);
  p->nodes[color].signature=pixel_list->signature;
  p->nodes[color].count=1;
  /*
    Determine where it belongs in the list.
  */
  search=65536UL;
  for (level=p->level; level >= 0; level--)
  {
    while (p->nodes[search].next[level] < color)
      search=p->nodes[search].next[level];
    update[level]=search;
  }
  /*
    Generate a pseudo-random level for this node.
  */
  for (level=0; ; level++)
  {
    pixel_list->seed=(pixel_list->seed*42893621L)+1L;
    if ((pixel_list->seed & 0x300) != 0x300)
      break;
  }
  if (level > 8)
    level=8;
  if (level > (p->level+2))
    level=p->level+2;
  /*
    If we're raising the list's level, link back to the root node.
  */
  while (level > p->level)
  {
    p->level++;
    update[p->level]=65536UL;
  }
  /*
    Link the node into the skip-list.
  */
  do
  {
    p->nodes[color].next[level]=p->nodes[update[level]].next[level];
    p->nodes[update[level]].next[level]=color;
  } while (level-- > 0);
}