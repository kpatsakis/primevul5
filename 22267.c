static void annotate_state_start(annotate_state_t *state)
{
    /* xxx better way to determine a size for this table? */
    construct_hash_table(&state->entry_table, 100, 1);
    construct_hash_table(&state->server_table, 10, 1);
}