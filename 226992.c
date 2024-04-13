void mutt_seqset_iterator_free(struct SeqsetIterator **ptr)
{
  if (!ptr || !*ptr)
    return;

  struct SeqsetIterator *iter = *ptr;
  FREE(&iter->full_seqset);
  FREE(ptr);
}