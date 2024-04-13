void CairoOutputDev::setTextPage(TextPage *text)
{
  if (this->text) 
    this->text->decRefCnt();
  if (actualText)
    delete actualText;
  if (text) {
    this->text = text;
    this->text->incRefCnt();
    actualText = new ActualText(text);
  } else {
    this->text = NULL;
    actualText = NULL;
  }
}
