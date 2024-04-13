STATIC word GC_descr_obj_size(complex_descriptor *d)
{
    switch(d -> TAG) {
      case LEAF_TAG:
        return(d -> ld.ld_nelements * d -> ld.ld_size);
      case ARRAY_TAG:
        return(d -> ad.ad_nelements
               * GC_descr_obj_size(d -> ad.ad_element_descr));
      case SEQUENCE_TAG:
        return(GC_descr_obj_size(d -> sd.sd_first)
               + GC_descr_obj_size(d -> sd.sd_second));
      default:
        ABORT_RET("Bad complex descriptor");
        return 0;
    }
}