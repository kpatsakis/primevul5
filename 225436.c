int gp_query_new(struct gp_workers *w, struct gp_conn *conn,
                 uint8_t *buffer, size_t buflen)
{
    struct gp_query *q;

    /* create query struct */
    q = calloc(1, sizeof(struct gp_query));
    if (!q) {
        return ENOMEM;
    }

    q->conn = conn;
    q->buffer = buffer;
    q->buflen = buflen;

    gp_query_assign(w, q);

    return 0;
}