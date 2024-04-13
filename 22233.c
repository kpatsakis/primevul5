EXPORTED void annotate_state_abort(annotate_state_t **statep)
{
    if (*statep)
        annotate_abort((*statep)->d);

    annotate_state_free(statep);
}