SkipDataBlocks(gif_source_ptr sinfo)
/* Skip a series of data blocks, until a block terminator is found */
{
  U_CHAR buf[256];

  while (GetDataBlock(sinfo, buf) > 0)
    /* skip */;
}