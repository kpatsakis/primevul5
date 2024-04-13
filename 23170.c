flx_decode_chunks (GstFlxDec * flxdec, gulong count, guchar * data,
    guchar * dest)
{
  FlxFrameChunk *hdr;
  gboolean ret = TRUE;

  g_return_val_if_fail (data != NULL, FALSE);

  while (count--) {
    hdr = (FlxFrameChunk *) data;
    FLX_FRAME_CHUNK_FIX_ENDIANNESS (hdr);
    data += FlxFrameChunkSize;

    switch (hdr->id) {
      case FLX_COLOR64:
        flx_decode_color (flxdec, data, dest, 2);
        data += rndalign (hdr->size) - FlxFrameChunkSize;
        break;

      case FLX_COLOR256:
        flx_decode_color (flxdec, data, dest, 0);
        data += rndalign (hdr->size) - FlxFrameChunkSize;
        break;

      case FLX_BRUN:
        ret = flx_decode_brun (flxdec, data, dest);
        data += rndalign (hdr->size) - FlxFrameChunkSize;
        break;

      case FLX_LC:
        ret = flx_decode_delta_fli (flxdec, data, dest);
        data += rndalign (hdr->size) - FlxFrameChunkSize;
        break;

      case FLX_SS2:
        ret = flx_decode_delta_flc (flxdec, data, dest);
        data += rndalign (hdr->size) - FlxFrameChunkSize;
        break;

      case FLX_BLACK:
        memset (dest, 0, flxdec->size);
        break;

      case FLX_MINI:
        data += rndalign (hdr->size) - FlxFrameChunkSize;
        break;

      default:
        GST_WARNING ("Unimplented chunk type: 0x%02x size: %d - skipping",
            hdr->id, hdr->size);
        data += rndalign (hdr->size) - FlxFrameChunkSize;
        break;
    }

    if (!ret)
      break;
  }

  return ret;
}