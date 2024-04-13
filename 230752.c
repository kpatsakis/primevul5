ip_eval(self, str)
    VALUE self;
    VALUE str;
{
    struct eval_queue *evq;
#ifdef RUBY_USE_NATIVE_THREAD
    struct tcltkip *ptr;
#endif
    char *eval_str;
    int  *alloc_done;
    int  thr_crit_bup;
    volatile VALUE current = rb_thread_current();
    volatile VALUE ip_obj = self;
    volatile VALUE result;
    volatile VALUE ret;
    Tcl_QueuePosition position;
    struct timeval t;

    thr_crit_bup = rb_thread_critical;
    rb_thread_critical = Qtrue;
    StringValue(str);
    rb_thread_critical = thr_crit_bup;

#ifdef RUBY_USE_NATIVE_THREAD
    ptr = get_ip(ip_obj);
    DUMP2("eval status: ptr->tk_thread_id %p", ptr->tk_thread_id);
    DUMP2("eval status: Tcl_GetCurrentThread %p", Tcl_GetCurrentThread());
#else
    DUMP2("status: Tcl_GetCurrentThread %p", Tcl_GetCurrentThread());
#endif
    DUMP2("status: eventloopt_thread %"PRIxVALUE, eventloop_thread);

    if (
#ifdef RUBY_USE_NATIVE_THREAD
	(ptr->tk_thread_id == 0 || ptr->tk_thread_id == Tcl_GetCurrentThread())
	&&
#endif
	(NIL_P(eventloop_thread) || current == eventloop_thread)
	) {
        if (NIL_P(eventloop_thread)) {
            DUMP2("eval from thread:%"PRIxVALUE" but no eventloop", current);
        } else {
            DUMP2("eval from current eventloop %"PRIxVALUE, current);
        }
        result = ip_eval_real(self, RSTRING_PTR(str), RSTRING_LENINT(str));
        if (rb_obj_is_kind_of(result, rb_eException)) {
            rb_exc_raise(result);
        }
        return result;
    }

    DUMP2("eval from thread %"PRIxVALUE" (NOT current eventloop)", current);

    thr_crit_bup = rb_thread_critical;
    rb_thread_critical = Qtrue;

    /* allocate memory (keep result) */
    /* alloc_done = (int*)ALLOC(int); */
    alloc_done = RbTk_ALLOC_N(int, 1);
#if 0 /* use Tcl_Preserve/Release */
    Tcl_Preserve((ClientData)alloc_done); /* XXXXXXXX */
#endif
    *alloc_done = 0;

    /* eval_str = ALLOC_N(char, RSTRING_LEN(str) + 1); */
    eval_str = ckalloc(RSTRING_LENINT(str) + 1);
#if 0 /* use Tcl_Preserve/Release */
    Tcl_Preserve((ClientData)eval_str); /* XXXXXXXX */
#endif
    memcpy(eval_str, RSTRING_PTR(str), RSTRING_LEN(str));
    eval_str[RSTRING_LEN(str)] = 0;

    /* allocate memory (freed by Tcl_ServiceEvent) */
    /* evq = (struct eval_queue *)Tcl_Alloc(sizeof(struct eval_queue)); */
    evq = RbTk_ALLOC_N(struct eval_queue, 1);
#if 0 /* use Tcl_Preserve/Release */
    Tcl_Preserve(evq);
#endif

    /* allocate result obj */
    result = rb_ary_new3(1, Qnil);

    /* construct event data */
    evq->done = alloc_done;
    evq->str = eval_str;
    evq->len = RSTRING_LENINT(str);
    evq->interp = ip_obj;
    evq->result = result;
    evq->thread = current;
    evq->safe_level = rb_safe_level();
    evq->ev.proc = eval_queue_handler;

    position = TCL_QUEUE_TAIL;

    /* add the handler to Tcl event queue */
    DUMP1("add handler");
#ifdef RUBY_USE_NATIVE_THREAD
    if (ptr->tk_thread_id) {
      /* Tcl_ThreadQueueEvent(ptr->tk_thread_id, &(evq->ev), position); */
      Tcl_ThreadQueueEvent(ptr->tk_thread_id, (Tcl_Event*)evq, position);
      Tcl_ThreadAlert(ptr->tk_thread_id);
    } else if (tk_eventloop_thread_id) {
      Tcl_ThreadQueueEvent(tk_eventloop_thread_id, (Tcl_Event*)evq, position);
      /* Tcl_ThreadQueueEvent(tk_eventloop_thread_id,
			   &(evq->ev), position); */
      Tcl_ThreadAlert(tk_eventloop_thread_id);
    } else {
      /* Tcl_QueueEvent(&(evq->ev), position); */
      Tcl_QueueEvent((Tcl_Event*)evq, position);
    }
#else
    /* Tcl_QueueEvent(&(evq->ev), position); */
    Tcl_QueueEvent((Tcl_Event*)evq, position);
#endif

    rb_thread_critical = thr_crit_bup;

    /* wait for the handler to be processed */
    t.tv_sec  = 0;
    t.tv_usec = (long)((EVENT_HANDLER_TIMEOUT)*1000.0);

    DUMP2("evq wait for handler (current thread:%"PRIxVALUE")", current);
    while(*alloc_done >= 0) {
      DUMP2("*** evq wait for handler (current thread:%"PRIxVALUE")", current);
      /* rb_thread_stop(); */
      /* rb_thread_sleep_forever(); */
      rb_thread_wait_for(t);
      DUMP2("*** evq wakeup (current thread:%"PRIxVALUE")", current);
      DUMP2("***          (eventloop thread:%"PRIxVALUE")", eventloop_thread);
      if (NIL_P(eventloop_thread)) {
	DUMP1("*** evq lost eventloop thread");
	break;
      }
    }
    DUMP2("back from handler (current thread:%"PRIxVALUE")", current);

    /* get result & free allocated memory */
    ret = RARRAY_PTR(result)[0];

#if 0 /* use Tcl_EventuallyFree */
    Tcl_EventuallyFree((ClientData)alloc_done, TCL_DYNAMIC); /* XXXXXXXX */
#else
#if 0 /* use Tcl_Preserve/Release */
    Tcl_Release((ClientData)alloc_done); /* XXXXXXXX */
#else
    /* free(alloc_done); */
    ckfree((char*)alloc_done);
#endif
#endif
#if 0 /* use Tcl_EventuallyFree */
    Tcl_EventuallyFree((ClientData)eval_str, TCL_DYNAMIC); /* XXXXXXXX */
#else
#if 0 /* use Tcl_Preserve/Release */
    Tcl_Release((ClientData)eval_str); /* XXXXXXXX */
#else
    /* free(eval_str); */
    ckfree(eval_str);
#endif
#endif
#if 0 /* evq is freed by Tcl_ServiceEvent */
#if 0 /* use Tcl_Preserve/Release */
    Tcl_Release(evq);
#else
    ckfree((char*)evq);
#endif
#endif

    if (rb_obj_is_kind_of(ret, rb_eException)) {
        DUMP1("raise exception");
        /* rb_exc_raise(ret); */
	rb_exc_raise(rb_exc_new3(rb_obj_class(ret),
				 rb_funcallv(ret, ID_to_s, 0, 0)));
    }

    return ret;
}