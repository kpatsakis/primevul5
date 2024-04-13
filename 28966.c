void CairoOutputDev::drawImageMaskPrescaled(GfxState *state, Object *ref, Stream *str,
				    int width, int height, GBool invert,
				    GBool inlineImg) {
  unsigned char *buffer;
  cairo_surface_t *image;
  cairo_pattern_t *pattern;
  ImageStream *imgStr;
  Guchar *pix;
  cairo_matrix_t matrix;
  int invert_bit;
  int row_stride;

  /* cairo does a very poor job of scaling down images so we scale them ourselves */

  /* this scaling code is adopted from the splash image scaling code */
  cairo_get_matrix(cairo, &matrix);
#if 0
  printf("[%f %f], [%f %f], %f %f\n", matrix.xx, matrix.xy, matrix.yx, matrix.yy, matrix.x0, matrix.y0);
#endif
  /* this whole computation should be factored out */
  double xScale = matrix.xx;
  double yScale = matrix.yy;
  int tx, tx2, ty, ty2; /* the integer co-oridinates of the resulting image */
  int scaledHeight;
  int scaledWidth;
  if (xScale >= 0) {
    tx = splashRound(matrix.x0 - 0.01);
    tx2 = splashRound(matrix.x0 + xScale + 0.01) - 1;
  } else {
    tx = splashRound(matrix.x0 + 0.01) - 1;
    tx2 = splashRound(matrix.x0 + xScale - 0.01);
  }
  scaledWidth = abs(tx2 - tx) + 1;
  if (scaledWidth == 0) {
    scaledWidth = 1;
  }
  if (yScale >= 0) {
    ty = splashFloor(matrix.y0 + 0.01);
    ty2 = splashCeil(matrix.y0 + yScale - 0.01);
  } else {
    ty = splashCeil(matrix.y0 - 0.01);
    ty2 = splashFloor(matrix.y0 + yScale + 0.01);
  }
  scaledHeight = abs(ty2 - ty);
  if (scaledHeight == 0) {
    scaledHeight = 1;
  }
#if 0
  printf("xscale: %g, yscale: %g\n", xScale, yScale);
  printf("width: %d, height: %d\n", width, height);
  printf("scaledWidth: %d, scaledHeight: %d\n", scaledWidth, scaledHeight);
#endif

  /* compute the required padding */
  /* Padding is used to preserve the aspect ratio.
     We compute total_pad to make (height+total_pad)/scaledHeight as close to height/yScale as possible */
  int head_pad = 0;
  int tail_pad = 0;
  int total_pad = splashRound(height*(scaledHeight/fabs(yScale)) - height);

  /* compute the two pieces of padding */
  if (total_pad > 0) {
    float tail_error = fabs(matrix.y0 - ty);
    float head_error = fabs(ty2 - (matrix.y0 + yScale));
    float tail_fraction = tail_error/(tail_error + head_error);
    tail_pad = splashRound(total_pad*tail_fraction);
    head_pad = total_pad - tail_pad;
  } else {
    tail_pad = 0;
    head_pad = 0;
  }
  int origHeight = height;
  height += tail_pad;
  height += head_pad;
#if 0
  printf("head_pad: %d tail_pad: %d\n", head_pad, tail_pad);
  printf("origHeight: %d height: %d\n", origHeight, height);
  printf("ty: %d, ty2: %d\n", ty, ty2);
#endif

  /* TODO: Do we want to cache these? */
  imgStr = new ImageStream(str, width, 1, 1);
  imgStr->reset();

  invert_bit = invert ? 1 : 0;

  row_stride = (scaledWidth + 3) & ~3;
  buffer = (unsigned char *) malloc (scaledHeight * row_stride);
  if (buffer == NULL) {
    error(-1, "Unable to allocate memory for image.");
    return;
  }

  int yp = height / scaledHeight;
  int yq = height % scaledHeight;
  int xp = width / scaledWidth;
  int xq = width % scaledWidth;
  int yt = 0;
  int origHeight_c = origHeight;
  /* use MIN() because yp might be > origHeight because of padding */
  unsigned char *pixBuf = (unsigned char *)malloc(MIN(yp+1, origHeight)*width);
  int lastYStep = 1;
  int total = 0;
  for (int y = 0; y < scaledHeight; y++) {
    int yStep = yp;
    yt += yq;

    if (yt >= scaledHeight) {
      yt -= scaledHeight;
      ++yStep;
    }

    {
      int n = (yp > 0) ? yStep : lastYStep;
      total += n;
      if (n > 0) {
	unsigned char *p = pixBuf;
	int head_pad_count = head_pad;
	int origHeight_count = origHeight;
	int tail_pad_count = tail_pad;
	for (int i=0; i<n; i++) {
	  if (head_pad_count) {
	    head_pad_count--;
	  } else if (origHeight_count) {
	    pix = imgStr->getLine();
	    for (int j=0; j<width; j++) {
	      if (pix[j] ^ invert_bit)
		p[j] = 0;
	      else
		p[j] = 255;
	    }
	    origHeight_count--;
	    p += width;
	  } else if (tail_pad_count) {
	    tail_pad_count--;
	  } else {
	    printf("%d %d\n", n, total);
	    assert(0 && "over run\n");
	  }
	}
      }
    }

    lastYStep = yStep;
    int k1 = y;

    int xt = 0;
    int xSrc = 0;
    int x1 = k1;
    int n = yStep > 0 ? yStep : 1;
    int origN = n;

    /* compute the size of padding and pixels that will be used for this row */
    int head_pad_size = MIN(n, head_pad);
    n -= head_pad_size;
    head_pad -= MIN(head_pad_size, yStep);

    int pix_size = MIN(n, origHeight);
    n -= pix_size;
    origHeight -= MIN(pix_size, yStep);

    int tail_pad_size = MIN(n, tail_pad);
    n -= tail_pad_size;
    tail_pad -= MIN(tail_pad_size, yStep);
    if (n != 0) {
      printf("n = %d (%d %d %d)\n", n, head_pad_size, pix_size, tail_pad_size);
      assert(n == 0);
    }

    for (int x = 0; x < scaledWidth; ++x) {
      int xStep = xp;
      xt += xq;
      if (xt >= scaledWidth) {
	xt -= scaledWidth;
	++xStep;
      }
      int m = xStep > 0 ? xStep : 1;
      float pixAcc0 = 0;
      /* could m * head_pad_size * tail_pad_size  overflow? */
      if (invert_bit) {
	pixAcc0 += m * head_pad_size * tail_pad_size * 255;
      } else {
	pixAcc0 += m * head_pad_size * tail_pad_size * 0;
      }
      /* Accumulate all of the source pixels for the destination pixel */
      for (int i = 0; i < pix_size; ++i) {
	for (int j = 0; j< m; ++j) {
	  if (xSrc + i*width + j > MIN(yp + 1, origHeight_c)*width) {
	    printf("%d > %d (%d %d %d %d) (%d %d %d)\n", xSrc + i*width + j, MIN(yp + 1, origHeight_c)*width, xSrc, i , width, j, yp, origHeight_c, width);
	    printf("%d %d %d\n", head_pad_size, pix_size, tail_pad_size);
	    assert(0 && "bad access\n");
	  }
	  pixAcc0 += pixBuf[xSrc + i*width + j];
	}
      }
      buffer[y * row_stride + x] = splashFloor(pixAcc0 / (origN*m));
      xSrc += xStep;
      x1 += 1;
    }

  }
  free(pixBuf);

  image = cairo_image_surface_create_for_data (buffer, CAIRO_FORMAT_A8,
      scaledWidth, scaledHeight, row_stride);
  if (image == NULL) {
    delete imgStr;
    return;
  }
  pattern = cairo_pattern_create_for_surface (image);
  if (pattern == NULL) {
    delete imgStr;
    return;
  }

  /* we should actually be using CAIRO_FILTER_NEAREST here. However,
   * cairo doesn't yet do minifaction filtering causing scaled down
   * images with CAIRO_FILTER_NEAREST to look really bad */
  cairo_pattern_set_filter (pattern, CAIRO_FILTER_BEST);

  cairo_save (cairo);

  /* modify our current transformation so that the prescaled image
   * goes where it is supposed to */
  cairo_get_matrix(cairo, &matrix);
  cairo_scale(cairo, 1.0/matrix.xx, 1.0/matrix.yy);
  cairo_translate (cairo, tx - matrix.x0, ty2 - matrix.y0);
  if (yScale > 0)
    cairo_scale(cairo, 1, -1);

  cairo_mask (cairo, pattern);

  cairo_restore(cairo);

  if (cairo_shape) {
    cairo_save (cairo_shape);

    /* modify our current transformation so that the prescaled image
     * goes where it is supposed to */
    cairo_get_matrix(cairo_shape, &matrix);
    cairo_scale(cairo_shape, 1.0/matrix.xx, 1.0/matrix.yy);
    cairo_translate (cairo_shape, tx - matrix.x0, ty2 - matrix.y0);
    if (yScale > 0)
      cairo_scale(cairo_shape, 1, -1);

    cairo_mask (cairo_shape, pattern);

    cairo_restore(cairo_shape);
  }


  cairo_pattern_destroy (pattern);
  cairo_surface_destroy (image);
  free (buffer);
  delete imgStr;
}
