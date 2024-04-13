set_max_block_time(self, time)
    VALUE self;
    VALUE time;
{
    struct Tcl_Time tcl_time;
    VALUE divmod;

    switch(TYPE(time)) {
    case T_FIXNUM:
    case T_BIGNUM:
        /* time is micro-second value */
        divmod = rb_funcall(time, rb_intern("divmod"), 1, LONG2NUM(1000000));
        tcl_time.sec  = NUM2LONG(RARRAY_PTR(divmod)[0]);
        tcl_time.usec = NUM2LONG(RARRAY_PTR(divmod)[1]);
        break;

    case T_FLOAT:
        /* time is second value */
        divmod = rb_funcall(time, rb_intern("divmod"), 1, INT2FIX(1));
        tcl_time.sec  = NUM2LONG(RARRAY_PTR(divmod)[0]);
        tcl_time.usec = (long)(NUM2DBL(RARRAY_PTR(divmod)[1]) * 1000000);

    default:
        {
	    VALUE tmp = rb_funcallv(time, ID_inspect, 0, 0);
	    rb_raise(rb_eArgError, "invalid value for time: '%s'",
		     StringValuePtr(tmp));
	}
    }

    Tcl_SetMaxBlockTime(&tcl_time);

    return Qnil;
}