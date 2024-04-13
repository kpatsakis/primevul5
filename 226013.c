static void ResetPixelList(PixelList *pixel_list)
{
  int
    level;

  SkipNode
    *root;

  SkipList
    *p;

  /*
    Reset the skip-list.
  */
  p=(&pixel_list->skip_list);
  root=p->nodes+65536UL;
  p->level=0;
  for (level=0; level < 9; level++)
    root->next[level]=65536UL;
  pixel_list->seed=pixel_list->signature++;
}