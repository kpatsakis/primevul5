readDelayed(void *context, CommRead const &)
{
    HttpStateData *state = static_cast<HttpStateData*>(context);
    state->flags.do_next_read = true;
    state->maybeReadVirginBody();
}