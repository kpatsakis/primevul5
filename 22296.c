static void detach_db(annotate_db_t *prev, annotate_db_t *d)
{
    if (prev)
        prev->next = d->next;
    else
        all_dbs_head = d->next;
    if (all_dbs_tail == d)
        all_dbs_tail = prev;
}