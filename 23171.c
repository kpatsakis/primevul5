flx_decode_brun (GstFlxDec * flxdec, guchar * data, guchar * dest)
{
  gulong count, lines, row;
  guchar x;

  g_return_val_if_fail (flxdec != NULL, FALSE);

  lines = flxdec->hdr.height;
  while (lines--) {
    /* packet count.  
     * should not be used anymore, since the flc format can
     * contain more then 255 RLE packets. we use the frame 
     * width instead. 
     */
    data++;

    row = flxdec->hdr.width;
    while (row) {
      count = *data++;

      if (count > 0x7f) {
        /* literal run */
        count = 0x100 - count;
        if ((glong) row - (glong) count < 0) {
          GST_ERROR_OBJECT (flxdec, "Invalid BRUN packet detected.");
          return FALSE;
        }
        row -= count;

        while (count--)
          *dest++ = *data++;

      } else {
        if ((glong) row - (glong) count < 0) {
          GST_ERROR_OBJECT (flxdec, "Invalid BRUN packet detected.");
          return FALSE;
        }

        /* replicate run */
        row -= count;
        x = *data++;

        while (count--)
          *dest++ = x;
      }
    }
  }

  return TRUE;
}