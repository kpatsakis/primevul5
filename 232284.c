CompareHiiValue (
  IN  EFI_HII_VALUE   *Value1,
  IN  EFI_HII_VALUE   *Value2,
  OUT INTN            *Result,
  IN  EFI_HII_HANDLE  HiiHandle OPTIONAL
  )
{
  INT64   Temp64;
  CHAR16  *Str1;
  CHAR16  *Str2;
  UINTN   Len;
  UINT8   *Buf1;
  UINT16  Buf1Len;
  UINT8   *Buf2;
  UINT16  Buf2Len;

  if (Value1->Type == EFI_IFR_TYPE_STRING && Value2->Type == EFI_IFR_TYPE_STRING) {
    if (Value1->Value.string == 0 || Value2->Value.string == 0) {
      //
      // StringId 0 is reserved
      //
      return EFI_INVALID_PARAMETER;
    }

    if (Value1->Value.string == Value2->Value.string) {
      *Result = 0;
      return EFI_SUCCESS;
    }

    Str1 = GetToken (Value1->Value.string, HiiHandle);
    if (Str1 == NULL) {
      //
      // String not found
      //
      return EFI_NOT_FOUND;
    }

    Str2 = GetToken (Value2->Value.string, HiiHandle);
    if (Str2 == NULL) {
      FreePool (Str1);
      return EFI_NOT_FOUND;
    }

    *Result = StrCmp (Str1, Str2);

    FreePool (Str1);
    FreePool (Str2);

    return EFI_SUCCESS;
  }

  //
  // Take types(date, time, ref, buffer) as buffer
  //
  if (IsTypeInBuffer(Value1) && IsTypeInBuffer(Value2)) {
    GetBufAndLenForValue(Value1, &Buf1, &Buf1Len);
    GetBufAndLenForValue(Value2, &Buf2, &Buf2Len);

    Len = Buf1Len > Buf2Len ? Buf2Len : Buf1Len;
    *Result = CompareMem (Buf1, Buf2, Len);
    if ((*Result == 0) && (Buf1Len != Buf2Len)) {
      //
      // In this case, means base on samll number buffer, the data is same
      // So which value has more data, which value is bigger.
      //
      *Result = Buf1Len > Buf2Len ? 1 : -1;
    }
    return EFI_SUCCESS;
  }

  //
  // Take remain types(integer, boolean, date/time) as integer
  //
  if (IsTypeInUINT64(Value1) && IsTypeInUINT64(Value2)) {
    Temp64 = HiiValueToUINT64(Value1) - HiiValueToUINT64(Value2);
    if (Temp64 > 0) {
      *Result = 1;
    } else if (Temp64 < 0) {
      *Result = -1;
    } else {
      *Result = 0;
    }
    return EFI_SUCCESS;
  }

  return EFI_UNSUPPORTED;
}