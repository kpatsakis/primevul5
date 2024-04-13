STATIC mse * GC_array_mark_proc(word * addr, mse * mark_stack_ptr,
                                mse * mark_stack_limit,
                                word env GC_ATTR_UNUSED)
{
    hdr * hhdr = HDR(addr);
    size_t sz = hhdr -> hb_sz;
    size_t nwords = BYTES_TO_WORDS(sz);
    complex_descriptor * descr = (complex_descriptor *)(addr[nwords-1]);
    mse * orig_mark_stack_ptr = mark_stack_ptr;
    mse * new_mark_stack_ptr;

    if (descr == 0) {
        /* Found a reference to a free list entry.  Ignore it. */
        return(orig_mark_stack_ptr);
    }
    /* In use counts were already updated when array descriptor was     */
    /* pushed.  Here we only replace it by subobject descriptors, so    */
    /* no update is necessary.                                          */
    new_mark_stack_ptr = GC_push_complex_descriptor(addr, descr,
                                                    mark_stack_ptr,
                                                    mark_stack_limit-1);
    if (new_mark_stack_ptr == 0) {
        /* Doesn't fit.  Conservatively push the whole array as a unit  */
        /* and request a mark stack expansion.                          */
        /* This cannot cause a mark stack overflow, since it replaces   */
        /* the original array entry.                                    */
        GC_ASSERT(mark_stack_ptr != NULL);
        GC_mark_stack_too_small = TRUE;
        new_mark_stack_ptr = orig_mark_stack_ptr + 1;
        new_mark_stack_ptr -> mse_start = (ptr_t)addr;
        new_mark_stack_ptr -> mse_descr.w = sz | GC_DS_LENGTH;
    } else {
        /* Push descriptor itself */
        new_mark_stack_ptr++;
        new_mark_stack_ptr -> mse_start = (ptr_t)(addr + nwords - 1);
        new_mark_stack_ptr -> mse_descr.w = sizeof(word) | GC_DS_LENGTH;
    }
    return new_mark_stack_ptr;
}