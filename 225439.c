static void gp_query_free(struct gp_query *q, bool free_buffer)
{
    if (!q) {
        return;
    }

    if (free_buffer) {
        free(q->buffer);
    }

    free(q);
}