static void annotation_get_freespace(annotate_state_t *state,
                                     struct annotate_entry_list *entry)
{
    uint64_t tavail = 0;
    struct buf value = BUF_INITIALIZER;

    (void) partlist_local_find_freespace_most(0, NULL, NULL, &tavail, NULL);
    buf_printf(&value, "%" PRIuMAX, (uintmax_t)tavail);
    output_entryatt(state, entry->name, "", &value);
    buf_free(&value);
}