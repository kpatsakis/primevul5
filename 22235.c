static void annotation_get_freespace_total(annotate_state_t *state,
                     struct annotate_entry_list *entry)
{
    uint64_t tavail = 0;
    uint64_t ttotal = 0;
    struct buf value = BUF_INITIALIZER;

    (void) partlist_local_find_freespace_most(0, NULL, NULL, &tavail, &ttotal);
    buf_printf(&value, "%" PRIuMAX ";%" PRIuMAX, (uintmax_t)tavail, (uintmax_t)ttotal);
    output_entryatt(state, entry->name, "", &value);
    buf_free(&value);
}