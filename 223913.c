STATIC int GC_make_array_descriptor(size_t nelements, size_t size,
                                    GC_descr descriptor, GC_descr *simple_d,
                                    complex_descriptor **complex_d,
                                    struct LeafDescriptor * leaf)
{
#   define OPT_THRESHOLD 50
        /* For larger arrays, we try to combine descriptors of adjacent */
        /* descriptors to speed up marking, and to reduce the amount    */
        /* of space needed on the mark stack.                           */
    if ((descriptor & GC_DS_TAGS) == GC_DS_LENGTH) {
      if (descriptor == (GC_descr)size) {
        *simple_d = nelements * descriptor;
        return(SIMPLE);
      } else if ((word)descriptor == 0) {
        *simple_d = (GC_descr)0;
        return(SIMPLE);
      }
    }
    if (nelements <= OPT_THRESHOLD) {
      if (nelements <= 1) {
        if (nelements == 1) {
            *simple_d = descriptor;
            return(SIMPLE);
        } else {
            *simple_d = (GC_descr)0;
            return(SIMPLE);
        }
      }
    } else if (size <= BITMAP_BITS/2
               && (descriptor & GC_DS_TAGS) != GC_DS_PROC
               && (size & (sizeof(word)-1)) == 0) {
      int result =
          GC_make_array_descriptor(nelements/2, 2*size,
                                   GC_double_descr(descriptor,
                                                   BYTES_TO_WORDS(size)),
                                   simple_d, complex_d, leaf);
      if ((nelements & 1) == 0) {
          return(result);
      } else {
          struct LeafDescriptor * one_element =
              (struct LeafDescriptor *)
                GC_malloc_atomic(sizeof(struct LeafDescriptor));

          if (result == NO_MEM || one_element == 0) return(NO_MEM);
          one_element -> ld_tag = LEAF_TAG;
          one_element -> ld_size = size;
          one_element -> ld_nelements = 1;
          one_element -> ld_descriptor = descriptor;
          switch(result) {
            case SIMPLE:
            {
              struct LeafDescriptor * beginning =
                (struct LeafDescriptor *)
                  GC_malloc_atomic(sizeof(struct LeafDescriptor));
              if (beginning == 0) return(NO_MEM);
              beginning -> ld_tag = LEAF_TAG;
              beginning -> ld_size = size;
              beginning -> ld_nelements = 1;
              beginning -> ld_descriptor = *simple_d;
              *complex_d = GC_make_sequence_descriptor(
                                (complex_descriptor *)beginning,
                                (complex_descriptor *)one_element);
              break;
            }
            case LEAF:
            {
              struct LeafDescriptor * beginning =
                (struct LeafDescriptor *)
                  GC_malloc_atomic(sizeof(struct LeafDescriptor));
              if (beginning == 0) return(NO_MEM);
              beginning -> ld_tag = LEAF_TAG;
              beginning -> ld_size = leaf -> ld_size;
              beginning -> ld_nelements = leaf -> ld_nelements;
              beginning -> ld_descriptor = leaf -> ld_descriptor;
              *complex_d = GC_make_sequence_descriptor(
                                (complex_descriptor *)beginning,
                                (complex_descriptor *)one_element);
              break;
            }
            case COMPLEX:
              *complex_d = GC_make_sequence_descriptor(
                                *complex_d,
                                (complex_descriptor *)one_element);
              break;
          }
          return(COMPLEX);
      }
    }

    leaf -> ld_size = size;
    leaf -> ld_nelements = nelements;
    leaf -> ld_descriptor = descriptor;
    return(LEAF);
}