GfxPattern *GfxTilingPattern::copy() {
  return new GfxTilingPattern(paintType, tilingType, bbox, xStep, yStep,
			      &resDict, matrix, &contentStream);
}
