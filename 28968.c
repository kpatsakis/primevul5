void CairoImageOutputDev::drawMaskedImage(GfxState *state, Object *ref, Stream *str,
					  int width, int height,
					  GfxImageColorMap *colorMap,
					  Stream *maskStr,
					  int maskWidth, int maskHeight,
					  GBool maskInvert)
{
  cairo_t *cr;
  cairo_surface_t *surface;
  double x1, y1, x2, y2;
  double *ctm;
  double mat[6];
  CairoImage *image;

  ctm = state->getCTM();
  
  mat[0] = ctm[0];
  mat[1] = ctm[1];
  mat[2] = -ctm[2];
  mat[3] = -ctm[3];
  mat[4] = ctm[2] + ctm[4];
  mat[5] = ctm[3] + ctm[5];
  x1 = mat[4];
  y1 = mat[5];
  x2 = x1 + width;
  y2 = y1 + height;

  image = new CairoImage (x1, y1, x2, y2);
  saveImage (image);

  if (imgDrawCbk && imgDrawCbk (numImages - 1, imgDrawCbkData)) {
    surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
    cr = cairo_create (surface);
    setCairo (cr);
    cairo_translate (cr, 0, height);
    cairo_scale (cr, width, -height);
    
    CairoOutputDev::drawMaskedImage(state, ref, str, width, height, colorMap,
				    maskStr, maskWidth, maskHeight, maskInvert);
    image->setImage (surface);
    
    setCairo (NULL);
    cairo_surface_destroy (surface);
    cairo_destroy (cr);
  }
}
