static void annotate_state_finish(annotate_state_t *state)
{
    /* Free the entry list */
    while (state->entry_list) {
        struct annotate_entry_list *ee = state->entry_list;
        state->entry_list = ee->next;
        buf_free(&ee->shared);
        buf_free(&ee->priv);
        free(ee->name);
        free(ee);
    }

    free_hash_table(&state->entry_table, NULL);
    free_hash_table(&state->server_table, NULL);
}