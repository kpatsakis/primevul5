FindArrayData (
  IN VOID                     *Array,
  IN UINT8                    Type,
  IN UINT64                   Value,
  OUT UINTN                   *Index OPTIONAL
  )
{
  UINTN  Count;
  UINT64 TmpValue;
  UINT64 ValueComp;

  ASSERT (Array != NULL);

  Count    = 0;
  TmpValue = 0;

  switch (Type) {
  case EFI_IFR_TYPE_NUM_SIZE_8:
    ValueComp = (UINT8) Value;
    break;

  case EFI_IFR_TYPE_NUM_SIZE_16:
    ValueComp = (UINT16) Value;
    break;

  case EFI_IFR_TYPE_NUM_SIZE_32:
    ValueComp = (UINT32) Value;
    break;

  case EFI_IFR_TYPE_NUM_SIZE_64:
    ValueComp = (UINT64) Value;
    break;

  default:
    ValueComp = 0;
    break;
  }

  while ((TmpValue = GetArrayData (Array, Type, Count)) != 0) {
    if (ValueComp == TmpValue) {
      if (Index != NULL) {
        *Index = Count;
      }
      return TRUE;
    }

    Count ++;
  }

  return FALSE;
}