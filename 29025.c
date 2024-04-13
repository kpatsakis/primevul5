GfxIndexedColorSpace::GfxIndexedColorSpace(GfxColorSpace *baseA,
					   int indexHighA) {
  base = baseA;
  indexHigh = indexHighA;
  lookup = (Guchar *)gmallocn((indexHigh + 1) * base->getNComps(),
			      sizeof(Guchar));
}
