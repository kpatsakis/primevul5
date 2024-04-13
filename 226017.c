static inline void GetModePixelList(PixelList *pixel_list,Quantum *pixel)
{
  SkipList
    *p;

  size_t
    color,
    max_count,
    mode;

  ssize_t
    count;

  /*
    Make each pixel the 'predominant color' of the specified neighborhood.
  */
  p=(&pixel_list->skip_list);
  color=65536L;
  mode=color;
  max_count=p->nodes[mode].count;
  count=0;
  do
  {
    color=p->nodes[color].next[0];
    if (p->nodes[color].count > max_count)
      {
        mode=color;
        max_count=p->nodes[mode].count;
      }
    count+=p->nodes[color].count;
  } while (count < (ssize_t) pixel_list->length);
  *pixel=ScaleShortToQuantum((unsigned short) mode);
}