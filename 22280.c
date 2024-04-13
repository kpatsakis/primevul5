EXPORTED int annotate_state_commit(annotate_state_t **statep)
{
    int r = 0;
    if (*statep)
        r = annotate_commit((*statep)->d);

    annotate_state_free(statep);
    return r;
}