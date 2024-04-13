GetArrayData (
  IN VOID                     *Array,
  IN UINT8                    Type,
  IN UINTN                    Index
  )
{
  UINT64 Data;

  ASSERT (Array != NULL);

  Data = 0;
  switch (Type) {
  case EFI_IFR_TYPE_NUM_SIZE_8:
    Data = (UINT64) *(((UINT8 *) Array) + Index);
    break;

  case EFI_IFR_TYPE_NUM_SIZE_16:
    Data = (UINT64) *(((UINT16 *) Array) + Index);
    break;

  case EFI_IFR_TYPE_NUM_SIZE_32:
    Data = (UINT64) *(((UINT32 *) Array) + Index);
    break;

  case EFI_IFR_TYPE_NUM_SIZE_64:
    Data = (UINT64) *(((UINT64 *) Array) + Index);
    break;

  default:
    break;
  }

  return Data;
}