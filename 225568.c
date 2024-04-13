get_pixel_rows(j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
{
  gif_source_ptr source = (gif_source_ptr)sinfo;
  register int c;
  register JSAMPROW ptr;
  register JDIMENSION col;
  register JSAMPARRAY colormap = source->colormap;

  ptr = source->pub.buffer[0];
  for (col = cinfo->image_width; col > 0; col--) {
    c = LZWReadByte(source);
    *ptr++ = colormap[CM_RED][c];
    *ptr++ = colormap[CM_GREEN][c];
    *ptr++ = colormap[CM_BLUE][c];
  }
  return 1;
}