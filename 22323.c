static void append_db(annotate_db_t *d)
{
    if (all_dbs_tail)
        all_dbs_tail->next = d;
    else
        all_dbs_head = d;
    all_dbs_tail = d;
    d->next = NULL;
}