void CairoOutputDev::type3D1(GfxState *state, double wx, double wy,
			     double llx, double lly, double urx, double ury) {
  t3_glyph_wx = wx;
  t3_glyph_wy = wy;
  t3_glyph_bbox[0] = llx;
  t3_glyph_bbox[1] = lly;
  t3_glyph_bbox[2] = urx;
  t3_glyph_bbox[3] = ury;
  t3_glyph_has_bbox = gTrue;
}
