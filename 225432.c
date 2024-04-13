static void gp_query_assign(struct gp_workers *w, struct gp_query *q)
{
    struct gp_thread *t = NULL;

    /* then either find a free thread or queue in the wait list */

    /* ======> POOL LOCK */
    pthread_mutex_lock(&w->lock);
    if (w->free_list) {
        t = w->free_list;
        LIST_DEL(w->free_list, t);
        LIST_ADD(w->busy_list, t);
    }
    /* <====== POOL LOCK */
    pthread_mutex_unlock(&w->lock);

    if (t) {
        /* found free thread, assign work */

        /* ======> COND_MUTEX */
        pthread_mutex_lock(&t->cond_mutex);

        /* hand over the query */
        t->query = q;
        pthread_cond_signal(&t->cond_wakeup);

        /* <====== COND_MUTEX */
        pthread_mutex_unlock(&t->cond_mutex);

    } else {

        /* all threads are busy, store in wait list */

        /* only the dispatcher handles wait_list
        *  so we do not need to lock around it */
        q->next = w->wait_list;
        w->wait_list = q;
    }
}