static void annotation_get_freespace_percent_most(annotate_state_t *state,
                     struct annotate_entry_list *entry)
{
    uint64_t avail = 0;
    uint64_t total = 0;
    struct buf value = BUF_INITIALIZER;

    (void) partlist_local_find_freespace_most(1, &avail, &total, NULL, NULL);
    buf_printf(&value, "%" PRIuMAX ";%" PRIuMAX, (uintmax_t)avail, (uintmax_t)total);
    output_entryatt(state, entry->name, "", &value);
    buf_free(&value);
}