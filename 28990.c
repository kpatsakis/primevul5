void CairoOutputDev::startDoc(XRef *xrefA, Catalog *catalogA,
			      CairoFontEngine *parentFontEngine) {
  xref = xrefA;
  catalog = catalogA;
  if (parentFontEngine) {
    fontEngine = parentFontEngine;
  } else {
    if (fontEngine) {
      delete fontEngine;
    }
    fontEngine = new CairoFontEngine(ft_lib);
    fontEngine_owner = gTrue;
  }
}
