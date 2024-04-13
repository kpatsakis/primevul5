STATIC void GC_init_explicit_typing(void)
{
    unsigned i;

    GC_STATIC_ASSERT(sizeof(struct LeafDescriptor) % sizeof(word) == 0);
    /* Set up object kind with simple indirect descriptor. */
      GC_eobjfreelist = (ptr_t *)GC_new_free_list_inner();
      GC_explicit_kind = GC_new_kind_inner(
                            (void **)GC_eobjfreelist,
                            (WORDS_TO_BYTES((word)-1) | GC_DS_PER_OBJECT),
                            TRUE, TRUE);
                /* Descriptors are in the last word of the object. */
      GC_typed_mark_proc_index = GC_new_proc_inner(GC_typed_mark_proc);
    /* Set up object kind with array descriptor. */
      GC_array_mark_proc_index = GC_new_proc_inner(GC_array_mark_proc);
      GC_array_kind = GC_new_kind_inner(GC_new_free_list_inner(),
                            GC_MAKE_PROC(GC_array_mark_proc_index, 0),
                            FALSE, TRUE);
      GC_bm_table[0] = GC_DS_BITMAP;
      for (i = 1; i < WORDSZ/2; i++) {
          GC_bm_table[i] = (((word)-1) << (WORDSZ - i)) | GC_DS_BITMAP;
      }
}