new_size(st_index_t size)
{
    int i;

#if 0
    for (i=3; i<31; i++) {
	if ((1<<i) > size) return 1<<i;
    }
    return -1;
#else
    st_index_t newsize;

    for (i = 0, newsize = MINSIZE; i < numberof(primes); i++, newsize <<= 1) {
	if (newsize > size) return primes[i];
    }
    /* Ran out of polynomials */
#ifndef NOT_RUBY
    rb_raise(rb_eRuntimeError, "st_table too big");
#endif
    return -1;			/* should raise exception */
#endif
}