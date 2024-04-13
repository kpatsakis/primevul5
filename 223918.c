STATIC mse * GC_push_complex_descriptor(word *addr, complex_descriptor *d,
                                        mse *msp, mse *msl)
{
    register ptr_t current = (ptr_t) addr;
    register word nelements;
    register word sz;
    register word i;

    switch(d -> TAG) {
      case LEAF_TAG:
        {
          register GC_descr descr = d -> ld.ld_descriptor;

          nelements = d -> ld.ld_nelements;
          if (msl - msp <= (ptrdiff_t)nelements) return(0);
          sz = d -> ld.ld_size;
          for (i = 0; i < nelements; i++) {
              msp++;
              msp -> mse_start = current;
              msp -> mse_descr.w = descr;
              current += sz;
          }
          return(msp);
        }
      case ARRAY_TAG:
        {
          register complex_descriptor *descr = d -> ad.ad_element_descr;

          nelements = d -> ad.ad_nelements;
          sz = GC_descr_obj_size(descr);
          for (i = 0; i < nelements; i++) {
              msp = GC_push_complex_descriptor((word *)current, descr,
                                                msp, msl);
              if (msp == 0) return(0);
              current += sz;
          }
          return(msp);
        }
      case SEQUENCE_TAG:
        {
          sz = GC_descr_obj_size(d -> sd.sd_first);
          msp = GC_push_complex_descriptor((word *)current, d -> sd.sd_first,
                                           msp, msl);
          if (msp == 0) return(0);
          current += sz;
          msp = GC_push_complex_descriptor((word *)current, d -> sd.sd_second,
                                           msp, msl);
          return(msp);
        }
      default:
        ABORT_RET("Bad complex descriptor");
        return 0;
   }
}