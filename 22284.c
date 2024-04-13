_annotate_state_add_entry(annotate_state_t *state,
                          const annotate_entrydesc_t *desc,
                          const char *name)
{
    struct annotate_entry_list *ee;

    ee = xzmalloc(sizeof(*ee));
    ee->name = xstrdup(name);
    ee->desc = desc;

    ee->next = state->entry_list;
    state->entry_list = ee;

    return ee;
}