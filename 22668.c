onig_is_prelude(void)
{
    return !rb_const_defined(rb_cThread, rb_intern_const("MUTEX_FOR_THREAD_EXCLUSIVE"));
}