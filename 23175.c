flx_decode_color (GstFlxDec * flxdec, guchar * data, guchar * dest, gint scale)
{
  guint packs, count, indx;

  g_return_if_fail (flxdec != NULL);

  packs = (data[0] + (data[1] << 8));

  data += 2;
  indx = 0;

  GST_LOG ("GstFlxDec: cmap packs: %d", packs);
  while (packs--) {
    /* color map index + skip count */
    indx += *data++;

    /* number of rgb triplets */
    count = *data++ & 0xff;
    if (count == 0)
      count = 256;

    GST_LOG ("GstFlxDec: cmap count: %d (indx: %d)", count, indx);
    flx_set_palette_vector (flxdec->converter, indx, count, data, scale);

    data += (count * 3);
  }
}