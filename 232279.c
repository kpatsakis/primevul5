IsTypeInUINT64 (
  IN  EFI_HII_VALUE   *Value
  )
{
  switch (Value->Type) {
  case EFI_IFR_TYPE_NUM_SIZE_8:
  case EFI_IFR_TYPE_NUM_SIZE_16:
  case EFI_IFR_TYPE_NUM_SIZE_32:
  case EFI_IFR_TYPE_NUM_SIZE_64:
  case EFI_IFR_TYPE_BOOLEAN:
    return TRUE;

  default:
    return FALSE;
  }
}