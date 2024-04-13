void CairoOutputDev::setSoftMask(GfxState * state, double * bbox, GBool alpha,
                                 Function * transferFunc, GfxColor * backdropColor) {
  cairo_pattern_destroy(mask);

  if (alpha == false) {
    /* We need to mask according to the luminocity of the group.
     * So we paint the group to an image surface convert it to a luminocity map
     * and then use that as the mask. */

    double x1, y1, x2, y2, tmp;
    cairo_clip_extents(cairo, &x1, &y1, &x2, &y2);
    cairo_user_to_device(cairo, &x1, &y1);
    cairo_user_to_device(cairo, &x2, &y2);
    if (x1 > x2) {
      tmp = x1;
      x1 = x2;
      x2 = tmp;
    }

    if (y1 > y2) {
      tmp = y1;
      y1 = y2;
      y2 = tmp;
    }

    int width = (int)(ceil(x2) - floor(x1));
    int height = (int)(ceil(y2) - floor(y1));

    cairo_surface_t *source = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    cairo_t *maskCtx = cairo_create(source);

    GfxRGB backdropColorRGB;
    groupColorSpaceStack->cs->getRGB(backdropColor, &backdropColorRGB);
    /* paint the backdrop */
    cairo_set_source_rgb(maskCtx, backdropColorRGB.r / 65535.0,
			 backdropColorRGB.g / 65535.0,
			 backdropColorRGB.b / 65535.0);


    cairo_matrix_t mat;
    cairo_get_matrix(cairo, &mat);
    cairo_set_matrix(maskCtx, &mat);

    /* make the device offset of the new mask match that of the group */
    double x_offset, y_offset;
    cairo_surface_t *pats;
    cairo_pattern_get_surface(group, &pats);
    cairo_surface_get_device_offset(pats, &x_offset, &y_offset);
    cairo_surface_set_device_offset(source, x_offset, y_offset);

    /* paint the group */
    cairo_set_source(maskCtx, group);
    cairo_paint(maskCtx);

    /* XXX status = cairo_status(maskCtx); */
    cairo_destroy(maskCtx);

    /* convert to a luminocity map */
    uint32_t *source_data = (uint32_t*)cairo_image_surface_get_data(source);
    /* get stride in units of 32 bits */
    int stride = cairo_image_surface_get_stride(source)/4;
    for (int y=0; y<height; y++) {
      for (int x=0; x<width; x++) {
	source_data[y*stride + x] = luminocity(source_data[y*stride + x]);

#if 0
	here is how splash deals with the transferfunction we should deal with this
	  at some point
	if (transferFunc) {
	  transferFunc->transform(&lum, &lum2);
	} else {
	  lum2 = lum;
	}
	p[x] = (int)(lum2 * 255.0 + 0.5);
#endif

      }
    }

    /* setup the new mask pattern */
    mask = cairo_pattern_create_for_surface(source);
    cairo_matrix_t patMatrix;
    cairo_pattern_get_matrix(group, &patMatrix);
    cairo_pattern_set_matrix(mask, &patMatrix);

    cairo_surface_destroy(source);
  } else {
    mask = cairo_pattern_reference(group);
  }

  popTransparencyGroup();
}
