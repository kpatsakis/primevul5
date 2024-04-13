void gp_workers_free(struct gp_workers *w)
{
    struct gp_thread *t;
    void *retval;

    /* ======> POOL LOCK */
    pthread_mutex_lock(&w->lock);

    w->shutdown = true;

    /* <====== POOL LOCK */
    pthread_mutex_unlock(&w->lock);

    /* we do not run the following operations within
     * the lock, or deadlocks may arise for threads
     * that are just finishing doing some work */

    /* we guarantee nobody is touching these lists by
     * preventing workers from touching the free/busy
     * lists when a 'shutdown' is in progress */

    while (w->free_list) {
        /* pick threads one by one */
        t = w->free_list;
        LIST_DEL(w->free_list, t);

        /* wake up threads, then join them */
        /* ======> COND_MUTEX */
        pthread_mutex_lock(&t->cond_mutex);
        pthread_cond_signal(&t->cond_wakeup);
        /* <====== COND_MUTEX */
        pthread_mutex_unlock(&t->cond_mutex);

        pthread_join(t->tid, &retval);

        pthread_mutex_destroy(&t->cond_mutex);
        pthread_cond_destroy(&t->cond_wakeup);
        free(t);
    }

    /* do the same with the busy list */
    while (w->busy_list) {
        /* pick threads one by one */
        t = w->busy_list;
        LIST_DEL(w->free_list, t);

        /* wake up threads, then join them */
        /* ======> COND_MUTEX */
        pthread_mutex_lock(&t->cond_mutex);
        pthread_cond_signal(&t->cond_wakeup);
        /* <====== COND_MUTEX */
        pthread_mutex_unlock(&t->cond_mutex);

        pthread_join(t->tid, &retval);

        pthread_mutex_destroy(&t->cond_mutex);
        pthread_cond_destroy(&t->cond_wakeup);
        free(t);
    }

    close(w->sig_pipe[0]);
    close(w->sig_pipe[1]);

    pthread_mutex_destroy(&w->lock);

    free(w);
}