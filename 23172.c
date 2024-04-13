flx_decode_delta_fli (GstFlxDec * flxdec, guchar * data, guchar * dest)
{
  gulong count, packets, lines, start_line;
  guchar *start_p, x;

  g_return_val_if_fail (flxdec != NULL, FALSE);
  g_return_val_if_fail (flxdec->delta_data != NULL, FALSE);

  /* use last frame for delta */
  memcpy (dest, flxdec->delta_data, flxdec->size);

  start_line = (data[0] + (data[1] << 8));
  lines = (data[2] + (data[3] << 8));
  if (start_line + lines > flxdec->hdr.height) {
    GST_ERROR_OBJECT (flxdec, "Invalid FLI packet detected. too many lines.");
    return FALSE;
  }
  data += 4;

  /* start position of delta */
  dest += (flxdec->hdr.width * start_line);
  start_p = dest;

  while (lines--) {
    /* packet count */
    packets = *data++;

    while (packets--) {
      /* skip count */
      guchar skip = *data++;
      dest += skip;

      /* RLE count */
      count = *data++;

      if (count > 0x7f) {
        /* literal run */
        count = 0x100 - count;

        if (skip + count > flxdec->hdr.width) {
          GST_ERROR_OBJECT (flxdec, "Invalid FLI packet detected. "
              "line too long.");
          return FALSE;
        }

        x = *data++;
        while (count--)
          *dest++ = x;

      } else {
        if (skip + count > flxdec->hdr.width) {
          GST_ERROR_OBJECT (flxdec, "Invalid FLI packet detected. "
              "line too long.");
          return FALSE;
        }

        /* replicate run */
        while (count--)
          *dest++ = *data++;
      }
    }
    start_p += flxdec->hdr.width;
    dest = start_p;
  }

  return TRUE;
}