void CairoOutputDev::endMarkedContent(GfxState *state)
{
  if (text)
    actualText->endMC(state);
}
