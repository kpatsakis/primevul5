HiiValueToUINT64 (
  IN EFI_HII_VALUE      *Value
  )
{
  UINT64  RetVal;

  RetVal = 0;

  switch (Value->Type) {
  case EFI_IFR_TYPE_NUM_SIZE_8:
    RetVal = Value->Value.u8;
    break;

  case EFI_IFR_TYPE_NUM_SIZE_16:
    RetVal = Value->Value.u16;
    break;

  case EFI_IFR_TYPE_NUM_SIZE_32:
    RetVal = Value->Value.u32;
    break;

  case EFI_IFR_TYPE_BOOLEAN:
    RetVal = Value->Value.b;
    break;

  case EFI_IFR_TYPE_DATE:
    RetVal = *(UINT64*) &Value->Value.date;
    break;

  case EFI_IFR_TYPE_TIME:
    RetVal = (*(UINT64*) &Value->Value.time) & 0xffffff;
    break;

  default:
    RetVal = Value->Value.u64;
    break;
  }

  return RetVal;
}