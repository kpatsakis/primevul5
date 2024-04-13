increment_simple_rowgroup_ctr(j_decompress_ptr cinfo, JDIMENSION rows)
{
  JDIMENSION rows_left;
  my_main_ptr main_ptr = (my_main_ptr)cinfo->main;
  my_master_ptr master = (my_master_ptr)cinfo->master;

  if (master->using_merged_upsample && cinfo->max_v_samp_factor == 2) {
    read_and_discard_scanlines(cinfo, rows);
    return;
  }

  /* Increment the counter to the next row group after the skipped rows. */
  main_ptr->rowgroup_ctr += rows / cinfo->max_v_samp_factor;

  /* Partially skipping a row group would involve modifying the internal state
   * of the upsampler, so read the remaining rows into a dummy buffer instead.
   */
  rows_left = rows % cinfo->max_v_samp_factor;
  cinfo->output_scanline += rows - rows_left;

  read_and_discard_scanlines(cinfo, rows_left);
}