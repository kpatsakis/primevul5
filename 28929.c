void ArthurOutputDev::clip(GfxState *state)
{
  m_painter->setClipPath(convertPath( state, state->getPath(), Qt::WindingFill ) );
}
