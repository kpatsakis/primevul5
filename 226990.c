struct SeqsetIterator *mutt_seqset_iterator_new(const char *seqset)
{
  if (!seqset || (*seqset == '\0'))
    return NULL;

  struct SeqsetIterator *iter = mutt_mem_calloc(1, sizeof(struct SeqsetIterator));
  iter->full_seqset = mutt_str_dup(seqset);
  iter->eostr = strchr(iter->full_seqset, '\0');
  iter->substr_cur = iter->substr_end = iter->full_seqset;

  return iter;
}