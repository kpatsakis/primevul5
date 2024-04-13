DoExtension(gif_source_ptr sinfo)
/* Process an extension block */
/* Currently we ignore 'em all */
{
  int extlabel;

  /* Read extension label byte */
  extlabel = ReadByte(sinfo);
  TRACEMS1(sinfo->cinfo, 1, JTRC_GIF_EXTENSION, extlabel);
  /* Skip the data block(s) associated with the extension */
  SkipDataBlocks(sinfo);
}