static void gp_handle_query(struct gp_workers *w, struct gp_query *q)
{
    struct gp_call_ctx gpcall = { 0 };
    uint8_t *buffer;
    size_t buflen;
    int ret;

    /* find service */
    gpcall.gpctx = w->gpctx;
    gpcall.service = gp_creds_match_conn(w->gpctx, q->conn);
    if (!gpcall.service) {
        q->status = GP_QUERY_ERR;
        return;
    }
    gpcall.connection = q->conn;

    ret = gp_rpc_process_call(&gpcall,
                              q->buffer, q->buflen,
                              &buffer, &buflen);
    if (ret) {
        q->status = GP_QUERY_ERR;
    } else {
        q->status = GP_QUERY_OUT;
        free(q->buffer);
        q->buffer = buffer;
        q->buflen = buflen;
    }

    if (gpcall.destroy_callback) {
        gpcall.destroy_callback(gpcall.destroy_callback_data);
    }
}