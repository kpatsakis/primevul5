static inline void GetNonpeakPixelList(PixelList *pixel_list,Quantum *pixel)
{
  SkipList
    *p;

  size_t
    color,
    next,
    previous;

  ssize_t
    count;

  /*
    Finds the non peak value for each of the colors.
  */
  p=(&pixel_list->skip_list);
  color=65536L;
  next=p->nodes[color].next[0];
  count=0;
  do
  {
    previous=color;
    color=next;
    next=p->nodes[color].next[0];
    count+=p->nodes[color].count;
  } while (count <= (ssize_t) (pixel_list->length >> 1));
  if ((previous == 65536UL) && (next != 65536UL))
    color=next;
  else
    if ((previous != 65536UL) && (next == 65536UL))
      color=previous;
  *pixel=ScaleShortToQuantum((unsigned short) color);
}