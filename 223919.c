STATIC mse * GC_typed_mark_proc(word * addr, mse * mark_stack_ptr,
                                mse * mark_stack_limit, word env)
{
    word bm = GC_ext_descriptors[env].ed_bitmap;
    word * current_p = addr;
    word current;
    ptr_t greatest_ha = GC_greatest_plausible_heap_addr;
    ptr_t least_ha = GC_least_plausible_heap_addr;
    DECLARE_HDR_CACHE;

    INIT_HDR_CACHE;
    for (; bm != 0; bm >>= 1, current_p++) {
        if (bm & 1) {
            current = *current_p;
            FIXUP_POINTER(current);
            if (current >= (word)least_ha && current <= (word)greatest_ha) {
                PUSH_CONTENTS((ptr_t)current, mark_stack_ptr,
                              mark_stack_limit, (ptr_t)current_p, exit1);
            }
        }
    }
    if (GC_ext_descriptors[env].ed_continued) {
        /* Push an entry with the rest of the descriptor back onto the  */
        /* stack.  Thus we never do too much work at once.  Note that   */
        /* we also can't overflow the mark stack unless we actually     */
        /* mark something.                                              */
        mark_stack_ptr++;
        if ((word)mark_stack_ptr >= (word)mark_stack_limit) {
            mark_stack_ptr = GC_signal_mark_stack_overflow(mark_stack_ptr);
        }
        mark_stack_ptr -> mse_start = (ptr_t)(addr + WORDSZ);
        mark_stack_ptr -> mse_descr.w =
                        GC_MAKE_PROC(GC_typed_mark_proc_index, env + 1);
    }
    return(mark_stack_ptr);
}