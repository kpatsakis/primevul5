load_interlaced_image(j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
{
  gif_source_ptr source = (gif_source_ptr)sinfo;
  register JSAMPROW sptr;
  register JDIMENSION col;
  JDIMENSION row;
  cd_progress_ptr progress = (cd_progress_ptr)cinfo->progress;

  /* Read the interlaced image into the virtual array we've created. */
  for (row = 0; row < cinfo->image_height; row++) {
    if (progress != NULL) {
      progress->pub.pass_counter = (long)row;
      progress->pub.pass_limit = (long)cinfo->image_height;
      (*progress->pub.progress_monitor) ((j_common_ptr)cinfo);
    }
    sptr = *(*cinfo->mem->access_virt_sarray)
      ((j_common_ptr)cinfo, source->interlaced_image, row, (JDIMENSION)1,
       TRUE);
    for (col = cinfo->image_width; col > 0; col--) {
      *sptr++ = (JSAMPLE)LZWReadByte(source);
    }
  }
  if (progress != NULL)
    progress->completed_extra_passes++;

  /* Replace method pointer so subsequent calls don't come here. */
  source->pub.get_pixel_rows = get_interlaced_row;
  /* Initialize for get_interlaced_row, and perform first call on it. */
  source->cur_row_number = 0;
  source->pass2_offset = (cinfo->image_height + 7) / 8;
  source->pass3_offset = source->pass2_offset + (cinfo->image_height + 3) / 8;
  source->pass4_offset = source->pass3_offset + (cinfo->image_height + 1) / 4;

  return get_interlaced_row(cinfo, sinfo);
}