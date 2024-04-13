GetBufAndLenForValue (
  IN  EFI_HII_VALUE   *Value,
  OUT UINT8           **Buf,
  OUT UINT16          *BufLen
  )
{
  switch (Value->Type) {
  case EFI_IFR_TYPE_BUFFER:
    *Buf    = Value->Buffer;
    *BufLen = Value->BufferLen;
    break;

  case EFI_IFR_TYPE_DATE:
    *Buf    = (UINT8 *) (&Value->Value.date);
    *BufLen = (UINT16) sizeof (EFI_HII_DATE);
    break;

  case EFI_IFR_TYPE_TIME:
    *Buf    = (UINT8 *) (&Value->Value.time);
    *BufLen = (UINT16) sizeof (EFI_HII_TIME);
    break;

  case EFI_IFR_TYPE_REF:
    *Buf    = (UINT8 *) (&Value->Value.ref);
    *BufLen = (UINT16) sizeof (EFI_HII_REF);
    break;

  default:
    *Buf    = NULL;
    *BufLen = 0;
  }
}