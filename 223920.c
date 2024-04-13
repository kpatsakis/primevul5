GC_make_sequence_descriptor(complex_descriptor *first,
                            complex_descriptor *second)
{
    struct SequenceDescriptor * result =
        (struct SequenceDescriptor *)
                GC_malloc(sizeof(struct SequenceDescriptor));
    /* Can't result in overly conservative marking, since tags are      */
    /* very small integers. Probably faster than maintaining type       */
    /* info.                                                            */
    if (result != 0) {
        result -> sd_tag = SEQUENCE_TAG;
        result -> sd_first = first;
        result -> sd_second = second;
    }
    return((complex_descriptor *)result);
}