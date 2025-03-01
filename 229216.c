jpeg_skip_scanlines(j_decompress_ptr cinfo, JDIMENSION num_lines)
{
  my_main_ptr main_ptr = (my_main_ptr)cinfo->main;
  my_coef_ptr coef = (my_coef_ptr)cinfo->coef;
  my_master_ptr master = (my_master_ptr)cinfo->master;
  my_upsample_ptr upsample = (my_upsample_ptr)cinfo->upsample;
  JDIMENSION i, x;
  int y;
  JDIMENSION lines_per_iMCU_row, lines_left_in_iMCU_row, lines_after_iMCU_row;
  JDIMENSION lines_to_skip, lines_to_read;

  /* Two-pass color quantization is not supported. */
  if (cinfo->quantize_colors && cinfo->two_pass_quantize)
    ERREXIT(cinfo, JERR_NOTIMPL);

  if (cinfo->global_state != DSTATE_SCANNING)
    ERREXIT1(cinfo, JERR_BAD_STATE, cinfo->global_state);

  /* Do not skip past the bottom of the image. */
  if (cinfo->output_scanline + num_lines >= cinfo->output_height) {
    num_lines = cinfo->output_height - cinfo->output_scanline;
    cinfo->output_scanline = cinfo->output_height;
    (*cinfo->inputctl->finish_input_pass) (cinfo);
    cinfo->inputctl->eoi_reached = TRUE;
    return num_lines;
  }

  if (num_lines == 0)
    return 0;

  lines_per_iMCU_row = cinfo->_min_DCT_scaled_size * cinfo->max_v_samp_factor;
  lines_left_in_iMCU_row =
    (lines_per_iMCU_row - (cinfo->output_scanline % lines_per_iMCU_row)) %
    lines_per_iMCU_row;
  lines_after_iMCU_row = num_lines - lines_left_in_iMCU_row;

  /* Skip the lines remaining in the current iMCU row.  When upsampling
   * requires context rows, we need the previous and next rows in order to read
   * the current row.  This adds some complexity.
   */
  if (cinfo->upsample->need_context_rows) {
    /* If the skipped lines would not move us past the current iMCU row, we
     * read the lines and ignore them.  There might be a faster way of doing
     * this, but we are facing increasing complexity for diminishing returns.
     * The increasing complexity would be a by-product of meddling with the
     * state machine used to skip context rows.  Near the end of an iMCU row,
     * the next iMCU row may have already been entropy-decoded.  In this unique
     * case, we will read the next iMCU row if we cannot skip past it as well.
     */
    if ((num_lines < lines_left_in_iMCU_row + 1) ||
        (lines_left_in_iMCU_row <= 1 && main_ptr->buffer_full &&
         lines_after_iMCU_row < lines_per_iMCU_row + 1)) {
      read_and_discard_scanlines(cinfo, num_lines);
      return num_lines;
    }

    /* If the next iMCU row has already been entropy-decoded, make sure that
     * we do not skip too far.
     */
    if (lines_left_in_iMCU_row <= 1 && main_ptr->buffer_full) {
      cinfo->output_scanline += lines_left_in_iMCU_row + lines_per_iMCU_row;
      lines_after_iMCU_row -= lines_per_iMCU_row;
    } else {
      cinfo->output_scanline += lines_left_in_iMCU_row;
    }

    /* If we have just completed the first block, adjust the buffer pointers */
    if (main_ptr->iMCU_row_ctr == 0 ||
        (main_ptr->iMCU_row_ctr == 1 && lines_left_in_iMCU_row > 2))
      set_wraparound_pointers(cinfo);
    main_ptr->buffer_full = FALSE;
    main_ptr->rowgroup_ctr = 0;
    main_ptr->context_state = CTX_PREPARE_FOR_IMCU;
    if (!master->using_merged_upsample) {
      upsample->next_row_out = cinfo->max_v_samp_factor;
      upsample->rows_to_go = cinfo->output_height - cinfo->output_scanline;
    }
  }

  /* Skipping is much simpler when context rows are not required. */
  else {
    if (num_lines < lines_left_in_iMCU_row) {
      increment_simple_rowgroup_ctr(cinfo, num_lines);
      return num_lines;
    } else {
      cinfo->output_scanline += lines_left_in_iMCU_row;
      main_ptr->buffer_full = FALSE;
      main_ptr->rowgroup_ctr = 0;
      if (!master->using_merged_upsample) {
        upsample->next_row_out = cinfo->max_v_samp_factor;
        upsample->rows_to_go = cinfo->output_height - cinfo->output_scanline;
      }
    }
  }

  /* Calculate how many full iMCU rows we can skip. */
  if (cinfo->upsample->need_context_rows)
    lines_to_skip = ((lines_after_iMCU_row - 1) / lines_per_iMCU_row) *
                    lines_per_iMCU_row;
  else
    lines_to_skip = (lines_after_iMCU_row / lines_per_iMCU_row) *
                    lines_per_iMCU_row;
  /* Calculate the number of lines that remain to be skipped after skipping all
   * of the full iMCU rows that we can.  We will not read these lines unless we
   * have to.
   */
  lines_to_read = lines_after_iMCU_row - lines_to_skip;

  /* For images requiring multiple scans (progressive, non-interleaved, etc.),
   * all of the entropy decoding occurs in jpeg_start_decompress(), assuming
   * that the input data source is non-suspending.  This makes skipping easy.
   */
  if (cinfo->inputctl->has_multiple_scans) {
    if (cinfo->upsample->need_context_rows) {
      cinfo->output_scanline += lines_to_skip;
      cinfo->output_iMCU_row += lines_to_skip / lines_per_iMCU_row;
      main_ptr->iMCU_row_ctr += lines_to_skip / lines_per_iMCU_row;
      /* It is complex to properly move to the middle of a context block, so
       * read the remaining lines instead of skipping them.
       */
      read_and_discard_scanlines(cinfo, lines_to_read);
    } else {
      cinfo->output_scanline += lines_to_skip;
      cinfo->output_iMCU_row += lines_to_skip / lines_per_iMCU_row;
      increment_simple_rowgroup_ctr(cinfo, lines_to_read);
    }
    if (!master->using_merged_upsample)
      upsample->rows_to_go = cinfo->output_height - cinfo->output_scanline;
    return num_lines;
  }

  /* Skip the iMCU rows that we can safely skip. */
  for (i = 0; i < lines_to_skip; i += lines_per_iMCU_row) {
    for (y = 0; y < coef->MCU_rows_per_iMCU_row; y++) {
      for (x = 0; x < cinfo->MCUs_per_row; x++) {
        /* Calling decode_mcu() with a NULL pointer causes it to discard the
         * decoded coefficients.  This is ~5% faster for large subsets, but
         * it's tough to tell a difference for smaller images.
         */
        (*cinfo->entropy->decode_mcu) (cinfo, NULL);
      }
    }
    cinfo->input_iMCU_row++;
    cinfo->output_iMCU_row++;
    if (cinfo->input_iMCU_row < cinfo->total_iMCU_rows)
      start_iMCU_row(cinfo);
    else
      (*cinfo->inputctl->finish_input_pass) (cinfo);
  }
  cinfo->output_scanline += lines_to_skip;

  if (cinfo->upsample->need_context_rows) {
    /* Context-based upsampling keeps track of iMCU rows. */
    main_ptr->iMCU_row_ctr += lines_to_skip / lines_per_iMCU_row;

    /* It is complex to properly move to the middle of a context block, so
     * read the remaining lines instead of skipping them.
     */
    read_and_discard_scanlines(cinfo, lines_to_read);
  } else {
    increment_simple_rowgroup_ctr(cinfo, lines_to_read);
  }

  /* Since skipping lines involves skipping the upsampling step, the value of
   * "rows_to_go" will become invalid unless we set it here.  NOTE: This is a
   * bit odd, since "rows_to_go" seems to be redundantly keeping track of
   * output_scanline.
   */
  if (!master->using_merged_upsample)
    upsample->rows_to_go = cinfo->output_height - cinfo->output_scanline;

  /* Always skip the requested number of lines. */
  return num_lines;
}