GfxSeparationColorSpace::GfxSeparationColorSpace(GooString *nameA,
						 GfxColorSpace *altA,
						 Function *funcA) {
  name = nameA;
  alt = altA;
  func = funcA;
  nonMarking = !name->cmp("None");
}
