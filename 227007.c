static void imap_msn_index_to_uid_seqset(struct Buffer *buf, struct ImapMboxData *mdata)
{
  int first = 1, state = 0;
  unsigned int cur_uid = 0, last_uid = 0;
  unsigned int range_begin = 0, range_end = 0;
  const size_t max_msn = imap_msn_highest(&mdata->msn);

  for (unsigned int msn = 1; msn <= max_msn + 1; msn++)
  {
    bool match = false;
    if (msn <= max_msn)
    {
      struct Email *e_cur = imap_msn_get(&mdata->msn, msn - 1);
      cur_uid = e_cur ? imap_edata_get(e_cur)->uid : 0;
      if (!state || (cur_uid && ((cur_uid - 1) == last_uid)))
        match = true;
      last_uid = cur_uid;
    }

    if (match)
    {
      switch (state)
      {
        case 1: /* single: convert to a range */
          state = 2;
          /* fall through */
        case 2: /* extend range ending */
          range_end = cur_uid;
          break;
        default:
          state = 1;
          range_begin = cur_uid;
          break;
      }
    }
    else if (state)
    {
      if (first)
        first = 0;
      else
        mutt_buffer_addch(buf, ',');

      if (state == 1)
        mutt_buffer_add_printf(buf, "%u", range_begin);
      else if (state == 2)
        mutt_buffer_add_printf(buf, "%u:%u", range_begin, range_end);

      state = 1;
      range_begin = cur_uid;
    }
  }
}