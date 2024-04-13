STATIC signed_word GC_add_ext_descriptor(const word * bm, word nbits)
{
    size_t nwords = divWORDSZ(nbits + WORDSZ-1);
    signed_word result;
    size_t i;
    word last_part;
    size_t extra_bits;
    DCL_LOCK_STATE;

    LOCK();
    while (GC_avail_descr + nwords >= GC_ed_size) {
        ext_descr * new;
        size_t new_size;
        word ed_size = GC_ed_size;

        if (ed_size == 0) {
            GC_ASSERT((word)&GC_ext_descriptors % sizeof(word) == 0);
            GC_push_typed_structures = GC_push_typed_structures_proc;
            UNLOCK();
            new_size = ED_INITIAL_SIZE;
        } else {
            UNLOCK();
            new_size = 2 * ed_size;
            if (new_size > MAX_ENV) return(-1);
        }
        new = (ext_descr *) GC_malloc_atomic(new_size * sizeof(ext_descr));
        if (new == 0) return(-1);
        LOCK();
        if (ed_size == GC_ed_size) {
            if (GC_avail_descr != 0) {
                BCOPY(GC_ext_descriptors, new,
                      GC_avail_descr * sizeof(ext_descr));
            }
            GC_ed_size = new_size;
            GC_ext_descriptors = new;
        }  /* else another thread already resized it in the meantime */
    }
    result = GC_avail_descr;
    for (i = 0; i < nwords-1; i++) {
        GC_ext_descriptors[result + i].ed_bitmap = bm[i];
        GC_ext_descriptors[result + i].ed_continued = TRUE;
    }
    last_part = bm[i];
    /* Clear irrelevant bits. */
    extra_bits = nwords * WORDSZ - nbits;
    last_part <<= extra_bits;
    last_part >>= extra_bits;
    GC_ext_descriptors[result + i].ed_bitmap = last_part;
    GC_ext_descriptors[result + i].ed_continued = FALSE;
    GC_avail_descr += nwords;
    UNLOCK();
    return(result);
}