GetDataBlock(gif_source_ptr sinfo, U_CHAR *buf)
/* Read a GIF data block, which has a leading count byte */
/* A zero-length block marks the end of a data block sequence */
{
  int count;

  count = ReadByte(sinfo);
  if (count > 0) {
    if (!ReadOK(sinfo->pub.input_file, buf, count))
      ERREXIT(sinfo->cinfo, JERR_INPUT_EOF);
  }
  return count;
}