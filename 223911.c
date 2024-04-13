GC_API GC_descr GC_CALL GC_make_descriptor(const GC_word * bm, size_t len)
{
    signed_word last_set_bit = len - 1;
    GC_descr result;
    signed_word i;
#   define HIGH_BIT (((word)1) << (WORDSZ - 1))
    DCL_LOCK_STATE;

#   if defined(THREADS) && defined(AO_HAVE_load_acquire)
      if (!EXPECT(AO_load_acquire(
                        (volatile AO_t *)&GC_explicit_typing_initialized),
                  TRUE))
#   endif
    {
      LOCK();
#     if defined(THREADS) && defined(AO_HAVE_load_acquire)
        if (!GC_explicit_typing_initialized)
#     else
        if (!EXPECT(GC_explicit_typing_initialized, TRUE))
#     endif
      {
        GC_init_explicit_typing();
        GC_explicit_typing_initialized = TRUE;
      }
      UNLOCK();
    }

    while (last_set_bit >= 0 && !GC_get_bit(bm, last_set_bit))
      last_set_bit--;
    if (last_set_bit < 0) return(0 /* no pointers */);
#   if ALIGNMENT == CPP_WORDSZ/8
    {
      for (i = 0; i < last_set_bit; i++) {
        if (!GC_get_bit(bm, i)) {
          break;
        }
      }
      if (i == last_set_bit) {
        /* An initial section contains all pointers.  Use length descriptor. */
        return (WORDS_TO_BYTES(last_set_bit+1) | GC_DS_LENGTH);
      }
    }
#   endif
    if ((word)last_set_bit < BITMAP_BITS) {
        /* Hopefully the common case.                   */
        /* Build bitmap descriptor (with bits reversed) */
        result = HIGH_BIT;
        for (i = last_set_bit - 1; i >= 0; i--) {
            result >>= 1;
            if (GC_get_bit(bm, i)) result |= HIGH_BIT;
        }
        result |= GC_DS_BITMAP;
    } else {
        signed_word index = GC_add_ext_descriptor(bm, (word)last_set_bit + 1);
        if (index == -1) return(WORDS_TO_BYTES(last_set_bit+1) | GC_DS_LENGTH);
                                /* Out of memory: use conservative      */
                                /* approximation.                       */
        result = GC_MAKE_PROC(GC_typed_mark_proc_index, (word)index);
    }
    return result;
}