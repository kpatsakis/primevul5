static void gp_handle_reply(verto_ctx *vctx, verto_ev *ev)
{
    struct gp_workers *w;
    struct gp_query *q = NULL;
    char dummy;
    int ret;

    w = verto_get_private(ev);

    /* first read out the dummy so the pipe doesn't get clogged */
    ret = read(w->sig_pipe[0], &dummy, 1);
    if (ret) {
        /* ignore errors */
    }

    /* grab a query reply if any */
    if (w->reply_list) {
        /* ======> POOL LOCK */
        pthread_mutex_lock(&w->lock);

        if (w->reply_list != NULL) {
            q = w->reply_list;
            w->reply_list = q->next;
        }

        /* <====== POOL LOCK */
        pthread_mutex_unlock(&w->lock);
    }

    if (q) {
        switch (q->status) {
        case GP_QUERY_IN:
            /* ?! fallback and kill client conn */
        case GP_QUERY_ERR:
            GPDEBUGN(3, "[status] Handling query error, terminating CID %d.\n",
                     gp_conn_get_cid(q->conn));
            gp_conn_free(q->conn);
            gp_query_free(q, true);
            break;

        case GP_QUERY_OUT:
            GPDEBUGN(3, "[status] Handling query reply: %p (%zu)\n", q->buffer, q->buflen);
            gp_socket_send_data(vctx, q->conn, q->buffer, q->buflen);
            gp_query_free(q, false);
            break;
        }
    }

    /* while we are at it, check if there is anything in the wait list
     * we need to process, as one thread just got free :-) */

    q = NULL;

    if (w->wait_list) {
        /* only the dispatcher handles wait_list
        *  so we do not need to lock around it */
        if (w->wait_list) {
            q = w->wait_list;
            w->wait_list = q->next;
            q->next = NULL;
        }
    }

    if (q) {
        gp_query_assign(w, q);
    }
}