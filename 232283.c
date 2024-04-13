PrintFormattedNumber (
  IN FORM_DISPLAY_ENGINE_STATEMENT   *Question,
  IN OUT CHAR16               *FormattedNumber,
  IN UINTN                    BufferSize
  )
{
  INT64          Value;
  CHAR16         *Format;
  EFI_HII_VALUE  *QuestionValue;
  EFI_IFR_NUMERIC *NumericOp;

  if (BufferSize < (21 * sizeof (CHAR16))) {
    return EFI_BUFFER_TOO_SMALL;
  }

  QuestionValue = &Question->CurrentValue;
  NumericOp     = (EFI_IFR_NUMERIC *) Question->OpCode;

  Value = (INT64) QuestionValue->Value.u64;
  switch (NumericOp->Flags & EFI_IFR_DISPLAY) {
  case EFI_IFR_DISPLAY_INT_DEC:
    switch (QuestionValue->Type) {
    case EFI_IFR_NUMERIC_SIZE_1:
      Value = (INT64) ((INT8) QuestionValue->Value.u8);
      break;

    case EFI_IFR_NUMERIC_SIZE_2:
      Value = (INT64) ((INT16) QuestionValue->Value.u16);
      break;

    case EFI_IFR_NUMERIC_SIZE_4:
      Value = (INT64) ((INT32) QuestionValue->Value.u32);
      break;

    case EFI_IFR_NUMERIC_SIZE_8:
    default:
      break;
    }

    if (Value < 0) {
      Value = -Value;
      Format = L"-%ld";
    } else {
      Format = L"%ld";
    }
    break;

  case EFI_IFR_DISPLAY_UINT_DEC:
    Format = L"%ld";
    break;

  case EFI_IFR_DISPLAY_UINT_HEX:
    Format = L"%lx";
    break;

  default:
    return EFI_UNSUPPORTED;
  }

  UnicodeSPrint (FormattedNumber, BufferSize, Format, Value);

  return EFI_SUCCESS;
}