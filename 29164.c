void GfxState::setFont(GfxFont *fontA, double fontSizeA) {
  if (font)
    font->decRefCnt();

  font = fontA;
  fontSize = fontSizeA;
}
