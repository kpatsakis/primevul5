  Copy_query_with_rewrite(THD *t, const char *s, size_t l, String *d)
    :thd(t), src(s), src_len(l), from(0), dst(d) { }