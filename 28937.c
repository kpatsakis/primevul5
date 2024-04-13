void ArthurOutputDev::fill(GfxState *state)
{
  m_painter->fillPath( convertPath( state, state->getPath(), Qt::WindingFill ), m_currentBrush );
}
