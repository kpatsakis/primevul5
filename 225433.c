int gp_workers_init(struct gssproxy_ctx *gpctx)
{
    struct gp_workers *w;
    struct gp_thread *t;
    pthread_attr_t attr;
    verto_ev *ev;
    int vflags;
    int ret;
    int i;

    w = calloc(1, sizeof(struct gp_workers));
    if (!w) {
        return ENOMEM;
    }
    w->gpctx = gpctx;

    /* init global queue mutex */
    ret = pthread_mutex_init(&w->lock, NULL);
    if (ret) {
        free(w);
        return ENOMEM;
    }

    if (gpctx->config->num_workers > 0) {
        w->num_threads = gpctx->config->num_workers;
    } else {
        w->num_threads = DEFAULT_WORKER_THREADS_NUM;
    }

    /* make thread joinable (portability) */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    /* init all workers */
    for (i = 0; i < w->num_threads; i++) {
        t = calloc(1, sizeof(struct gp_thread));
        if (!t) {
            ret = -1;
            goto done;
        }
        t->pool = w;
        ret = pthread_cond_init(&t->cond_wakeup, NULL);
        if (ret) {
            free(t);
            goto done;
        }
        ret = pthread_mutex_init(&t->cond_mutex, NULL);
        if (ret) {
            free(t);
            goto done;
        }
        ret = pthread_create(&t->tid, &attr, gp_worker_main, t);
        if (ret) {
            free(t);
            goto done;
        }
        LIST_ADD(w->free_list, t);
    }

    /* add wakeup pipe, so that threads can hand back replies to the
     * dispatcher */
    ret = pipe2(w->sig_pipe, O_NONBLOCK | O_CLOEXEC);
    if (ret == -1) {
        goto done;
    }

    vflags = VERTO_EV_FLAG_PERSIST | VERTO_EV_FLAG_IO_READ;
    ev = verto_add_io(gpctx->vctx, vflags, gp_handle_reply, w->sig_pipe[0]);
    if (!ev) {
        ret = -1;
        goto done;
    }
    verto_set_private(ev, w, NULL);

    gpctx->workers = w;
    ret = 0;

done:
    if (ret) {
        gp_workers_free(w);
    }
    return ret;
}