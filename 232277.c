ValueToOption (
  IN FORM_DISPLAY_ENGINE_STATEMENT   *Question,
  IN EFI_HII_VALUE                   *OptionValue
  )
{
  LIST_ENTRY               *Link;
  DISPLAY_QUESTION_OPTION  *Option;
  INTN                     Result;
  EFI_HII_VALUE            Value;

  Link = GetFirstNode (&Question->OptionListHead);
  while (!IsNull (&Question->OptionListHead, Link)) {
    Option = DISPLAY_QUESTION_OPTION_FROM_LINK (Link);

    ZeroMem (&Value, sizeof (EFI_HII_VALUE));
    Value.Type = Option->OptionOpCode->Type;
    CopyMem (&Value.Value, &Option->OptionOpCode->Value, Option->OptionOpCode->Header.Length - OFFSET_OF (EFI_IFR_ONE_OF_OPTION, Value));

    if ((CompareHiiValue (&Value, OptionValue, &Result, NULL) == EFI_SUCCESS) && (Result == 0)) {
      return Option;
    }

    Link = GetNextNode (&Question->OptionListHead, Link);
  }

  return NULL;
}