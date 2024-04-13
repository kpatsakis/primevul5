ProcessHelpString (
  IN  CHAR16  *StringPtr,
  OUT CHAR16  **FormattedString,
  OUT UINT16  *EachLineWidth,
  IN  UINTN   RowCount
  )
{
  UINTN   Index;
  CHAR16  *OutputString;
  UINTN   TotalRowNum;
  UINTN   CheckedNum;
  UINT16  GlyphWidth;
  UINT16  LineWidth;
  UINT16  MaxStringLen;
  UINT16  StringLen;

  TotalRowNum    = 0;
  CheckedNum     = 0;
  GlyphWidth     = 1;
  Index          = 0;
  MaxStringLen   = 0;
  StringLen      = 0;

  //
  // Set default help string width.
  //
  LineWidth      = (UINT16) (gHelpBlockWidth - 1);

  //
  // Get row number of the String.
  //
  while ((StringLen = GetLineByWidth (StringPtr, LineWidth, &GlyphWidth, &Index, &OutputString)) != 0) {
    if (StringLen > MaxStringLen) {
      MaxStringLen = StringLen;
    }

    TotalRowNum ++;
    FreePool (OutputString);
  }
  *EachLineWidth = MaxStringLen;

  *FormattedString = AllocateZeroPool (TotalRowNum * MaxStringLen * sizeof (CHAR16));
  ASSERT (*FormattedString != NULL);

  //
  // Generate formatted help string array.
  //
  GlyphWidth  = 1;
  Index       = 0;
  while((StringLen = GetLineByWidth (StringPtr, LineWidth, &GlyphWidth, &Index, &OutputString)) != 0) {
    CopyMem (*FormattedString + CheckedNum * MaxStringLen, OutputString, StringLen * sizeof (CHAR16));
    CheckedNum ++;
    FreePool (OutputString);
  }

  return TotalRowNum;
}