void CairoOutputDev::endPage() {
  if (text) {
    text->endPage();
    text->coalesce(gTrue, gFalse);
  }
}
