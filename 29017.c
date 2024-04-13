GfxDeviceNColorSpace::GfxDeviceNColorSpace(int nCompsA,
					   GfxColorSpace *altA,
					   Function *funcA) {
  nComps = nCompsA;
  alt = altA;
  func = funcA;
  nonMarking = gFalse;
}
