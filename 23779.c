kwsprep (kwset_t kwset)
{
  char const *trans = kwset->trans;
  int i;
  unsigned char deltabuf[NCHAR];
  unsigned char *delta = trans ? deltabuf : kwset->delta;

  /* Initial values for the delta table; will be changed later.  The
     delta entry for a given character is the smallest depth of any
     node at which an outgoing edge is labeled by that character. */
  memset (delta, MIN (kwset->mind, UCHAR_MAX), sizeof deltabuf);

  /* Traverse the nodes of the trie in level order, simultaneously
     computing the delta table, failure function, and shift function.  */
  struct trie *curr, *last;
  for (curr = last = kwset->trie; curr; curr = curr->next)
    {
      /* Enqueue the immediate descendants in the level order queue.  */
      enqueue (curr->links, &last);

      curr->shift = kwset->mind;
      curr->maxshift = kwset->mind;

      /* Update the delta table for the descendants of this node.  */
      treedelta (curr->links, curr->depth, delta);

      /* Compute the failure function for the descendants of this node.  */
      treefails (curr->links, curr->fail, kwset->trie);

      /* Update the shifts at each node in the current node's chain
         of fails back to the root.  */
      struct trie *fail;
      for (fail = curr->fail; fail; fail = fail->fail)
        {
          /* If the current node has some outgoing edge that the fail
             doesn't, then the shift at the fail should be no larger
             than the difference of their depths.  */
          if (!hasevery (fail->links, curr->links))
            if (curr->depth - fail->depth < fail->shift)
              fail->shift = curr->depth - fail->depth;

          /* If the current node is accepting then the shift at the
             fail and its descendants should be no larger than the
             difference of their depths.  */
          if (curr->accepting && fail->maxshift > curr->depth - fail->depth)
            fail->maxshift = curr->depth - fail->depth;
        }
    }

  /* Traverse the trie in level order again, fixing up all nodes whose
     shift exceeds their inherited maxshift.  */
  for (curr = kwset->trie->next; curr; curr = curr->next)
    {
      if (curr->maxshift > curr->parent->maxshift)
        curr->maxshift = curr->parent->maxshift;
      if (curr->shift > curr->maxshift)
        curr->shift = curr->maxshift;
    }

  /* Create a vector, indexed by character code, of the outgoing links
     from the root node.  */
  struct trie *nextbuf[NCHAR];
  struct trie **next = trans ? nextbuf : kwset->next;
  memset (next, 0, sizeof nextbuf);
  treenext (kwset->trie->links, next);
  if (trans)
    for (i = 0; i < NCHAR; ++i)
      kwset->next[i] = next[U(trans[i])];

  /* Check if we can use the simple boyer-moore algorithm, instead
     of the hairy commentz-walter algorithm. */
  if (kwset->words == 1)
    {
      /* Looking for just one string.  Extract it from the trie. */
      kwset->target = obstack_alloc (&kwset->obstack, kwset->mind);
      for (i = kwset->mind - 1, curr = kwset->trie; i >= 0; --i)
        {
          kwset->target[i] = curr->links->label;
          curr = curr->next;
        }
      /* Looking for the delta2 shift that we might make after a
         backwards match has failed.  Extract it from the trie.  */
      if (kwset->mind > 1)
        {
          kwset->shift
            = obstack_alloc (&kwset->obstack,
                             sizeof *kwset->shift * (kwset->mind - 1));
          for (i = 0, curr = kwset->trie->next; i < kwset->mind - 1; ++i)
            {
              kwset->shift[i] = curr->shift;
              curr = curr->next;
            }
        }

      char gc1 = tr (trans, kwset->target[kwset->mind - 1]);

      /* Set GC1HELP according to whether exactly one, exactly two, or
         three-or-more characters match GC1.  */
      int gc1help = -1;
      if (trans)
        {
          char const *equiv1 = memchr (trans, gc1, NCHAR);
          char const *equiv2 = memchr (equiv1 + 1, gc1,
                                       trans + NCHAR - (equiv1 + 1));
          if (equiv2)
            gc1help = (memchr (equiv2 + 1, gc1, trans + NCHAR - (equiv2 + 1))
                       ? NCHAR
                       : U(gc1) ^ (equiv1 - trans) ^ (equiv2 - trans));
        }

      kwset->gc1 = gc1;
      kwset->gc1help = gc1help;
      if (kwset->mind > 1)
        kwset->gc2 = tr (trans, kwset->target[kwset->mind - 2]);
    }

  /* Fix things up for any translation table. */
  if (trans)
    for (i = 0; i < NCHAR; ++i)
      kwset->delta[i] = delta[U(trans[i])];
}