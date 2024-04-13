  bool append(Rewritable_query_parameter *p)
  {
    if (copy_up_to(p->pos_in_query) || p->append_for_log(thd, dst))
      return true;
    from= p->pos_in_query + p->len_in_query;
    return false;
  }