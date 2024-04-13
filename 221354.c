sym_to_proc(VALUE sym)
{
    static VALUE sym_proc_cache = Qfalse;
    enum {SYM_PROC_CACHE_SIZE = 67};
    VALUE proc;
    long id, index;
    VALUE *aryp;

    if (!sym_proc_cache) {
	sym_proc_cache = rb_ary_tmp_new(SYM_PROC_CACHE_SIZE * 2);
	rb_gc_register_mark_object(sym_proc_cache);
	rb_ary_store(sym_proc_cache, SYM_PROC_CACHE_SIZE*2 - 1, Qnil);
    }

    id = SYM2ID(sym);
    index = (id % SYM_PROC_CACHE_SIZE) << 1;

    aryp = RARRAY_PTR(sym_proc_cache);
    if (aryp[index] == sym) {
	return aryp[index + 1];
    }
    else {
	proc = rb_proc_new(sym_call, (VALUE)id);
	aryp[index] = sym;
	aryp[index + 1] = proc;
	return proc;
    }
}