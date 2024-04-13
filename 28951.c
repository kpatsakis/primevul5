void ArthurOutputDev::updateLineWidth(GfxState *state)
{
  m_currentPen.setWidthF(state->getTransformedLineWidth());
  m_painter->setPen(m_currentPen);
}
