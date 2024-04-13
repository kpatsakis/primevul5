ArthurOutputDev::ArthurOutputDev(QPainter *painter):
  m_painter(painter)
{
  m_currentBrush = QBrush(Qt::SolidPattern);
  m_fontEngine = 0;
  m_font = 0;
  m_image = 0;
}
