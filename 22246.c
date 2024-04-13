static void annotate_begin(annotate_db_t *d)
{
    if (d)
        d->in_txn = 1;
}