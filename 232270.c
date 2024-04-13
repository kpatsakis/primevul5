IsTypeInBuffer (
  IN  EFI_HII_VALUE   *Value
  )
{
  switch (Value->Type) {
  case EFI_IFR_TYPE_BUFFER:
  case EFI_IFR_TYPE_DATE:
  case EFI_IFR_TYPE_TIME:
  case EFI_IFR_TYPE_REF:
    return TRUE;

  default:
    return FALSE;
  }
}