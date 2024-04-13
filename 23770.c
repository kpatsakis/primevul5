cwexec (kwset_t kwset, char const *text, size_t len, struct kwsmatch *kwsmatch)
{
  struct trie * const *next;
  struct trie const *trie;
  struct trie const *accept;
  char const *beg, *lim, *mch, *lmch;
  unsigned char c;
  unsigned char const *delta;
  int d;
  char const *end, *qlim;
  struct tree const *tree;
  char const *trans;

#ifdef lint
  accept = NULL;
#endif

  /* Initialize register copies and look for easy ways out. */
  if (len < kwset->mind)
    return -1;
  next = kwset->next;
  delta = kwset->delta;
  trans = kwset->trans;
  lim = text + len;
  end = text;
  if ((d = kwset->mind) != 0)
    mch = NULL;
  else
    {
      mch = text, accept = kwset->trie;
      goto match;
    }

  if (len >= 4 * kwset->mind)
    qlim = lim - 4 * kwset->mind;
  else
    qlim = NULL;

  while (lim - end >= d)
    {
      if (qlim && end <= qlim)
        {
          end += d - 1;
          while ((d = delta[c = *end]) && end < qlim)
            {
              end += d;
              end += delta[U(*end)];
              end += delta[U(*end)];
            }
          ++end;
        }
      else
        d = delta[c = (end += d)[-1]];
      if (d)
        continue;
      beg = end - 1;
      trie = next[c];
      if (trie->accepting)
        {
          mch = beg;
          accept = trie;
        }
      d = trie->shift;
      while (beg > text)
        {
          unsigned char uc = *--beg;
          c = trans ? trans[uc] : uc;
          tree = trie->links;
          while (tree && c != tree->label)
            if (c < tree->label)
              tree = tree->llink;
            else
              tree = tree->rlink;
          if (tree)
            {
              trie = tree->trie;
              if (trie->accepting)
                {
                  mch = beg;
                  accept = trie;
                }
            }
          else
            break;
          d = trie->shift;
        }
      if (mch)
        goto match;
    }
  return -1;

 match:
  /* Given a known match, find the longest possible match anchored
     at or before its starting point.  This is nearly a verbatim
     copy of the preceding main search loops. */
  if (lim - mch > kwset->maxd)
    lim = mch + kwset->maxd;
  lmch = 0;
  d = 1;
  while (lim - end >= d)
    {
      if ((d = delta[c = (end += d)[-1]]) != 0)
        continue;
      beg = end - 1;
      if (!(trie = next[c]))
        {
          d = 1;
          continue;
        }
      if (trie->accepting && beg <= mch)
        {
          lmch = beg;
          accept = trie;
        }
      d = trie->shift;
      while (beg > text)
        {
          unsigned char uc = *--beg;
          c = trans ? trans[uc] : uc;
          tree = trie->links;
          while (tree && c != tree->label)
            if (c < tree->label)
              tree = tree->llink;
            else
              tree = tree->rlink;
          if (tree)
            {
              trie = tree->trie;
              if (trie->accepting && beg <= mch)
                {
                  lmch = beg;
                  accept = trie;
                }
            }
          else
            break;
          d = trie->shift;
        }
      if (lmch)
        {
          mch = lmch;
          goto match;
        }
      if (!d)
        d = 1;
    }

  kwsmatch->index = accept->accepting / 2;
  kwsmatch->offset[0] = mch - text;
  kwsmatch->size[0] = accept->depth;

  return mch - text;
}