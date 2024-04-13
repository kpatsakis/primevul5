void ArthurOutputDev::eoFill(GfxState *state)
{
  m_painter->fillPath( convertPath( state, state->getPath(), Qt::OddEvenFill ), m_currentBrush );
}
