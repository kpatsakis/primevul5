FoFiType1C::~FoFiType1C() {
  int i;

  if (name) {
    delete name;
  }
  if (encoding &&
      encoding != fofiType1StandardEncoding &&
      encoding != fofiType1ExpertEncoding) {
    for (i = 0; i < 256; ++i) {
      gfree(encoding[i]);
    }
    gfree(encoding);
  }
  if (privateDicts) {
    gfree(privateDicts);
  }
  if (fdSelect) {
    gfree(fdSelect);
  }
  if (charset &&
      charset != fofiType1CISOAdobeCharset &&
      charset != fofiType1CExpertCharset &&
      charset != fofiType1CExpertSubsetCharset) {
    gfree(charset);
  }
}
