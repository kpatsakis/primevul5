void GfxState::shiftCTM(double tx, double ty) {
  ctm[4] += tx;
  ctm[5] += ty;
  clipXMin += tx;
  clipYMin += ty;
  clipXMax += tx;
  clipYMax += ty;
}
