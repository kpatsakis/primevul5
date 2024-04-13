int msg_new(message_data_t **m)
{
    message_data_t *ret = (message_data_t *) xmalloc(sizeof(message_data_t));

    ret->data = NULL;
    ret->f = NULL;
    ret->id = NULL;
    ret->path = NULL;
    ret->control = NULL;
    ret->size = 0;
    strarray_init(&ret->rcpt);
    ret->date = NULL;

    ret->hdrcache = spool_new_hdrcache();

    *m = ret;
    return 0;
}