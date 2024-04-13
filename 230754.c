eval_queue_handler(evPtr, flags)
    Tcl_Event *evPtr;
    int flags;
{
    struct eval_queue *q = (struct eval_queue *)evPtr;
    volatile VALUE ret;
    volatile VALUE q_dat;
    volatile VALUE thread = q->thread;
    struct tcltkip *ptr;

    DUMP2("do_eval_queue_handler : evPtr = %p", evPtr);
    DUMP2("eval_queue_thread : %"PRIxVALUE, rb_thread_current());
    DUMP2("added by thread : %"PRIxVALUE, thread);

    if (*(q->done)) {
        DUMP1("processed by another event-loop");
        return 0;
    } else {
        DUMP1("process it on current event-loop");
    }

    if (RTEST(rb_thread_alive_p(thread))
	&& ! RTEST(rb_funcall(thread, ID_stop_p, 0))) {
      DUMP1("caller is not yet ready to receive the result -> pending");
      return 0;
    }

    /* process it */
    *(q->done) = 1;

    /* deleted ipterp ? */
    ptr = get_ip(q->interp);
    if (deleted_ip(ptr)) {
        /* deleted IP --> ignore */
        return 1;
    }

    /* incr internal handler mark */
    rbtk_internal_eventloop_handler++;

    /* check safe-level */
    if (rb_safe_level() != q->safe_level) {
#ifdef HAVE_NATIVETHREAD
#ifndef RUBY_USE_NATIVE_THREAD
    if (!ruby_native_thread_p()) {
      rb_bug("cross-thread violation on eval_queue_handler()");
    }
#endif
#endif
        /* q_dat = Data_Wrap_Struct(rb_cData,0,-1,q); */
        q_dat = Data_Wrap_Struct(0,eval_queue_mark,-1,q);
        ret = rb_funcall(rb_proc_new(evq_safelevel_handler, q_dat),
                         ID_call, 0);
        rb_gc_force_recycle(q_dat);
	q_dat = (VALUE)NULL;
    } else {
        ret = ip_eval_real(q->interp, q->str, q->len);
    }

    /* set result */
    RARRAY_PTR(q->result)[0] = ret;
    ret = (VALUE)NULL;

    /* decr internal handler mark */
    rbtk_internal_eventloop_handler--;

    /* complete */
    *(q->done) = -1;

    /* unlink ruby objects */
    q->interp = (VALUE)NULL;
    q->result = (VALUE)NULL;
    q->thread = (VALUE)NULL;

    /* back to caller */
    if (RTEST(rb_thread_alive_p(thread))) {
      DUMP2("back to caller (caller thread:%"PRIxVALUE")", thread);
      DUMP2("               (current thread:%"PRIxVALUE")", rb_thread_current());
#if CONTROL_BY_STATUS_OF_RB_THREAD_WAITING_FOR_VALUE
      have_rb_thread_waiting_for_value = 1;
      rb_thread_wakeup(thread);
#else
      rb_thread_run(thread);
#endif
      DUMP1("finish back to caller");
#if DO_THREAD_SCHEDULE_AT_CALLBACK_DONE
      rb_thread_schedule();
#endif
    } else {
      DUMP2("caller is dead (caller thread:%"PRIxVALUE")", thread);
      DUMP2("               (current thread:%"PRIxVALUE")", rb_thread_current());
    }

    /* end of handler : remove it */
    return 1;
}