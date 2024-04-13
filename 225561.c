ReadColorMap(gif_source_ptr sinfo, int cmaplen, JSAMPARRAY cmap)
/* Read a GIF colormap */
{
  int i;

  for (i = 0; i < cmaplen; i++) {
#if BITS_IN_JSAMPLE == 8
#define UPSCALE(x)  (x)
#else
#define UPSCALE(x)  ((x) << (BITS_IN_JSAMPLE - 8))
#endif
    cmap[CM_RED][i]   = (JSAMPLE)UPSCALE(ReadByte(sinfo));
    cmap[CM_GREEN][i] = (JSAMPLE)UPSCALE(ReadByte(sinfo));
    cmap[CM_BLUE][i]  = (JSAMPLE)UPSCALE(ReadByte(sinfo));
  }
}