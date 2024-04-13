ProcessOptions (
  IN  UI_MENU_OPTION              *MenuOption,
  IN  BOOLEAN                     Selected,
  OUT CHAR16                      **OptionString,
  IN  BOOLEAN                     SkipErrorValue
  )
{
  EFI_STATUS                      Status;
  CHAR16                          *StringPtr;
  UINTN                           Index;
  FORM_DISPLAY_ENGINE_STATEMENT   *Question;
  CHAR16                          FormattedNumber[21];
  UINT16                          Number;
  CHAR16                          Character[2];
  EFI_INPUT_KEY                   Key;
  UINTN                           BufferSize;
  DISPLAY_QUESTION_OPTION         *OneOfOption;
  LIST_ENTRY                      *Link;
  EFI_HII_VALUE                   HiiValue;
  EFI_HII_VALUE                   *QuestionValue;
  DISPLAY_QUESTION_OPTION         *Option;
  UINTN                           Index2;
  UINT8                           *ValueArray;
  UINT8                           ValueType;
  EFI_IFR_ORDERED_LIST            *OrderList;
  BOOLEAN                         ValueInvalid;
  UINTN                           MaxLen;

  Status        = EFI_SUCCESS;

  StringPtr     = NULL;
  Character[1]  = L'\0';
  *OptionString = NULL;
  ValueInvalid  = FALSE;

  ZeroMem (FormattedNumber, 21 * sizeof (CHAR16));
  BufferSize = (gOptionBlockWidth + 1) * 2 * gStatementDimensions.BottomRow;

  Question = MenuOption->ThisTag;
  QuestionValue = &Question->CurrentValue;

  switch (Question->OpCode->OpCode) {
  case EFI_IFR_ORDERED_LIST_OP:

    //
    // Check whether there are Options of this OrderedList
    //
    if (IsListEmpty (&Question->OptionListHead)) {
      break;
    }

    OrderList = (EFI_IFR_ORDERED_LIST *) Question->OpCode;

    Link = GetFirstNode (&Question->OptionListHead);
    OneOfOption = DISPLAY_QUESTION_OPTION_FROM_LINK (Link);

    ValueType =  OneOfOption->OptionOpCode->Type;
    ValueArray = Question->CurrentValue.Buffer;

    if (Selected) {
      //
      // Go ask for input
      //
      Status = GetSelectionInputPopUp (MenuOption);
    } else {
      //
      // We now know how many strings we will have, so we can allocate the
      // space required for the array or strings.
      //
      MaxLen = OrderList->MaxContainers * BufferSize / sizeof (CHAR16);
      *OptionString = AllocateZeroPool (MaxLen * sizeof (CHAR16));
      ASSERT (*OptionString);

      HiiValue.Type = ValueType;
      HiiValue.Value.u64 = 0;
      for (Index = 0; Index < OrderList->MaxContainers; Index++) {
        HiiValue.Value.u64 = GetArrayData (ValueArray, ValueType, Index);
        if (HiiValue.Value.u64 == 0) {
          //
          // Values for the options in ordered lists should never be a 0
          //
          break;
        }

        OneOfOption = ValueToOption (Question, &HiiValue);
        if (OneOfOption == NULL) {
          if (SkipErrorValue) {
            //
            // Just try to get the option string, skip the value which not has option.
            //
            continue;
          }

          //
          // Show error message
          //
          do {
            CreateDialog (&Key, gEmptyString, gOptionMismatch, gPressEnter, gEmptyString, NULL);
          } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN);

          //
          // The initial value of the orderedlist is invalid, force to be valid value
          // Exit current DisplayForm with new value.
          //
          gUserInput->SelectedStatement = Question;
          gMisMatch = TRUE;
          ValueArray = AllocateZeroPool (Question->CurrentValue.BufferLen);
          ASSERT (ValueArray != NULL);
          gUserInput->InputValue.Buffer    = ValueArray;
          gUserInput->InputValue.BufferLen = Question->CurrentValue.BufferLen;
          gUserInput->InputValue.Type      = Question->CurrentValue.Type;

          Link = GetFirstNode (&Question->OptionListHead);
          Index2 = 0;
          while (!IsNull (&Question->OptionListHead, Link) && Index2 < OrderList->MaxContainers) {
            Option = DISPLAY_QUESTION_OPTION_FROM_LINK (Link);
            Link = GetNextNode (&Question->OptionListHead, Link);
            SetArrayData (ValueArray, ValueType, Index2, Option->OptionOpCode->Value.u64);
            Index2++;
          }
          SetArrayData (ValueArray, ValueType, Index2, 0);

          FreePool (*OptionString);
          *OptionString = NULL;
          return EFI_NOT_FOUND;
        }

        Character[0] = LEFT_ONEOF_DELIMITER;
        NewStrCat (OptionString[0], MaxLen, Character);
        StringPtr = GetToken (OneOfOption->OptionOpCode->Option, gFormData->HiiHandle);
        ASSERT (StringPtr != NULL);
        NewStrCat (OptionString[0], MaxLen, StringPtr);
        Character[0] = RIGHT_ONEOF_DELIMITER;
        NewStrCat (OptionString[0], MaxLen, Character);
        Character[0] = CHAR_CARRIAGE_RETURN;
        NewStrCat (OptionString[0], MaxLen, Character);
        FreePool (StringPtr);
      }

      //
      // If valid option more than the max container, skip these options.
      //
      if (Index >= OrderList->MaxContainers) {
        break;
      }

      //
      // Search the other options, try to find the one not in the container.
      //
      Link = GetFirstNode (&Question->OptionListHead);
      while (!IsNull (&Question->OptionListHead, Link)) {
        OneOfOption = DISPLAY_QUESTION_OPTION_FROM_LINK (Link);
        Link = GetNextNode (&Question->OptionListHead, Link);

        if (FindArrayData (ValueArray, ValueType, OneOfOption->OptionOpCode->Value.u64, NULL)) {
          continue;
        }

        if (SkipErrorValue) {
          //
          // Not report error, just get the correct option string info.
          //
          Character[0] = LEFT_ONEOF_DELIMITER;
          NewStrCat (OptionString[0], MaxLen, Character);
          StringPtr = GetToken (OneOfOption->OptionOpCode->Option, gFormData->HiiHandle);
          ASSERT (StringPtr != NULL);
          NewStrCat (OptionString[0], MaxLen, StringPtr);
          Character[0] = RIGHT_ONEOF_DELIMITER;
          NewStrCat (OptionString[0], MaxLen, Character);
          Character[0] = CHAR_CARRIAGE_RETURN;
          NewStrCat (OptionString[0], MaxLen, Character);
          FreePool (StringPtr);

          continue;
        }

        if (!ValueInvalid) {
          ValueInvalid = TRUE;
          //
          // Show error message
          //
          do {
            CreateDialog (&Key, gEmptyString, gOptionMismatch, gPressEnter, gEmptyString, NULL);
          } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN);

          //
          // The initial value of the orderedlist is invalid, force to be valid value
          // Exit current DisplayForm with new value.
          //
          gUserInput->SelectedStatement = Question;
          gMisMatch = TRUE;
          ValueArray = AllocateCopyPool (Question->CurrentValue.BufferLen, Question->CurrentValue.Buffer);
          ASSERT (ValueArray != NULL);
          gUserInput->InputValue.Buffer    = ValueArray;
          gUserInput->InputValue.BufferLen = Question->CurrentValue.BufferLen;
          gUserInput->InputValue.Type      = Question->CurrentValue.Type;
        }

        SetArrayData (ValueArray, ValueType, Index++, OneOfOption->OptionOpCode->Value.u64);
      }

      if (ValueInvalid) {
        FreePool (*OptionString);
        *OptionString = NULL;
        return EFI_NOT_FOUND;
      }
    }
    break;

  case EFI_IFR_ONE_OF_OP:
    //
    // Check whether there are Options of this OneOf
    //
    if (IsListEmpty (&Question->OptionListHead)) {
      break;
    }
    if (Selected) {
      //
      // Go ask for input
      //
      Status = GetSelectionInputPopUp (MenuOption);
    } else {
      MaxLen = BufferSize / sizeof(CHAR16);
      *OptionString = AllocateZeroPool (BufferSize);
      ASSERT (*OptionString);

      OneOfOption = ValueToOption (Question, QuestionValue);
      if (OneOfOption == NULL) {
        if (SkipErrorValue) {
          //
          // Not report error, just get the correct option string info.
          //
          Link = GetFirstNode (&Question->OptionListHead);
          OneOfOption = DISPLAY_QUESTION_OPTION_FROM_LINK (Link);
        } else {
          //
          // Show error message
          //
          do {
            CreateDialog (&Key, gEmptyString, gOptionMismatch, gPressEnter, gEmptyString, NULL);
          } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN);

          //
          // Force the Question value to be valid
          // Exit current DisplayForm with new value.
          //
          Link = GetFirstNode (&Question->OptionListHead);
          Option = DISPLAY_QUESTION_OPTION_FROM_LINK (Link);

          gUserInput->InputValue.Type = Option->OptionOpCode->Type;
          switch (gUserInput->InputValue.Type) {
          case EFI_IFR_TYPE_NUM_SIZE_8:
            gUserInput->InputValue.Value.u8 = Option->OptionOpCode->Value.u8;
            break;
          case EFI_IFR_TYPE_NUM_SIZE_16:
            CopyMem (&gUserInput->InputValue.Value.u16, &Option->OptionOpCode->Value.u16, sizeof (UINT16));
            break;
          case EFI_IFR_TYPE_NUM_SIZE_32:
            CopyMem (&gUserInput->InputValue.Value.u32, &Option->OptionOpCode->Value.u32, sizeof (UINT32));
            break;
          case EFI_IFR_TYPE_NUM_SIZE_64:
            CopyMem (&gUserInput->InputValue.Value.u64, &Option->OptionOpCode->Value.u64, sizeof (UINT64));
            break;
          default:
            ASSERT (FALSE);
            break;
          }
          gUserInput->SelectedStatement = Question;
          gMisMatch = TRUE;
          FreePool (*OptionString);
          *OptionString = NULL;
          return EFI_NOT_FOUND;
        }
      }

      Character[0] = LEFT_ONEOF_DELIMITER;
      NewStrCat (OptionString[0], MaxLen, Character);
      StringPtr = GetToken (OneOfOption->OptionOpCode->Option, gFormData->HiiHandle);
      ASSERT (StringPtr != NULL);
      NewStrCat (OptionString[0], MaxLen, StringPtr);
      Character[0] = RIGHT_ONEOF_DELIMITER;
      NewStrCat (OptionString[0], MaxLen, Character);

      FreePool (StringPtr);
    }
    break;

  case EFI_IFR_CHECKBOX_OP:
    if (Selected) {
      //
      // Since this is a BOOLEAN operation, flip it upon selection
      //
      gUserInput->InputValue.Type    = QuestionValue->Type;
      gUserInput->InputValue.Value.b = (BOOLEAN) (QuestionValue->Value.b ? FALSE : TRUE);

      //
      // Perform inconsistent check
      //
      return EFI_SUCCESS;
    } else {
      *OptionString = AllocateZeroPool (BufferSize);
      ASSERT (*OptionString);

      *OptionString[0] = LEFT_CHECKBOX_DELIMITER;

      if (QuestionValue->Value.b) {
        *(OptionString[0] + 1) = CHECK_ON;
      } else {
        *(OptionString[0] + 1) = CHECK_OFF;
      }
      *(OptionString[0] + 2) = RIGHT_CHECKBOX_DELIMITER;
    }
    break;

  case EFI_IFR_NUMERIC_OP:
    if (Selected) {
      //
      // Go ask for input
      //
      Status = GetNumericInput (MenuOption);
    } else {
      *OptionString = AllocateZeroPool (BufferSize);
      ASSERT (*OptionString);

      *OptionString[0] = LEFT_NUMERIC_DELIMITER;

      //
      // Formatted print
      //
      PrintFormattedNumber (Question, FormattedNumber, 21 * sizeof (CHAR16));
      Number = (UINT16) GetStringWidth (FormattedNumber);
      CopyMem (OptionString[0] + 1, FormattedNumber, Number);

      *(OptionString[0] + Number / 2) = RIGHT_NUMERIC_DELIMITER;
    }
    break;

  case EFI_IFR_DATE_OP:
    if (Selected) {
      //
      // This is similar to numerics
      //
      Status = GetNumericInput (MenuOption);
    } else {
      *OptionString = AllocateZeroPool (BufferSize);
      ASSERT (*OptionString);

      switch (MenuOption->Sequence) {
      case 0:
        *OptionString[0] = LEFT_NUMERIC_DELIMITER;
        if (QuestionValue->Value.date.Month == 0xff){
          UnicodeSPrint (OptionString[0] + 1, 21 * sizeof (CHAR16), L"??");
        } else {
          UnicodeSPrint (OptionString[0] + 1, 21 * sizeof (CHAR16), L"%02d", QuestionValue->Value.date.Month);
        }
        *(OptionString[0] + 3) = DATE_SEPARATOR;
        break;

      case 1:
        SetUnicodeMem (OptionString[0], 4, L' ');
        if (QuestionValue->Value.date.Day == 0xff){
          UnicodeSPrint (OptionString[0] + 4, 21 * sizeof (CHAR16), L"??");
        } else {
          UnicodeSPrint (OptionString[0] + 4, 21 * sizeof (CHAR16), L"%02d", QuestionValue->Value.date.Day);
        }
        *(OptionString[0] + 6) = DATE_SEPARATOR;
        break;

      case 2:
        SetUnicodeMem (OptionString[0], 7, L' ');
        if (QuestionValue->Value.date.Year == 0xff){
          UnicodeSPrint (OptionString[0] + 7, 21 * sizeof (CHAR16), L"????");
        } else {
          UnicodeSPrint (OptionString[0] + 7, 21 * sizeof (CHAR16), L"%04d", QuestionValue->Value.date.Year);
        }
        *(OptionString[0] + 11) = RIGHT_NUMERIC_DELIMITER;
        break;
      }
    }
    break;

  case EFI_IFR_TIME_OP:
    if (Selected) {
      //
      // This is similar to numerics
      //
      Status = GetNumericInput (MenuOption);
    } else {
      *OptionString = AllocateZeroPool (BufferSize);
      ASSERT (*OptionString);

      switch (MenuOption->Sequence) {
      case 0:
        *OptionString[0] = LEFT_NUMERIC_DELIMITER;
        if (QuestionValue->Value.time.Hour == 0xff){
          UnicodeSPrint (OptionString[0] + 1, 21 * sizeof (CHAR16), L"??");
        } else {
          UnicodeSPrint (OptionString[0] + 1, 21 * sizeof (CHAR16), L"%02d", QuestionValue->Value.time.Hour);
        }
        *(OptionString[0] + 3) = TIME_SEPARATOR;
        break;

      case 1:
        SetUnicodeMem (OptionString[0], 4, L' ');
        if (QuestionValue->Value.time.Minute == 0xff){
          UnicodeSPrint (OptionString[0] + 4, 21 * sizeof (CHAR16), L"??");
        } else {
          UnicodeSPrint (OptionString[0] + 4, 21 * sizeof (CHAR16), L"%02d", QuestionValue->Value.time.Minute);
        }
        *(OptionString[0] + 6) = TIME_SEPARATOR;
        break;

      case 2:
        SetUnicodeMem (OptionString[0], 7, L' ');
        if (QuestionValue->Value.time.Second == 0xff){
          UnicodeSPrint (OptionString[0] + 7, 21 * sizeof (CHAR16), L"??");
        } else {
          UnicodeSPrint (OptionString[0] + 7, 21 * sizeof (CHAR16), L"%02d", QuestionValue->Value.time.Second);
        }
        *(OptionString[0] + 9) = RIGHT_NUMERIC_DELIMITER;
        break;
      }
    }
    break;

  case EFI_IFR_STRING_OP:
    if (Selected) {
      StringPtr = AllocateZeroPool (Question->CurrentValue.BufferLen + sizeof (CHAR16));
      ASSERT (StringPtr);
      CopyMem(StringPtr, Question->CurrentValue.Buffer, Question->CurrentValue.BufferLen);

      Status = ReadString (MenuOption, gPromptForData, StringPtr);
      if (EFI_ERROR (Status)) {
        FreePool (StringPtr);
        return Status;
      }

      gUserInput->InputValue.Buffer = AllocateCopyPool (Question->CurrentValue.BufferLen, StringPtr);
      gUserInput->InputValue.BufferLen = Question->CurrentValue.BufferLen;
      gUserInput->InputValue.Type = Question->CurrentValue.Type;
      gUserInput->InputValue.Value.string = HiiSetString(gFormData->HiiHandle, gUserInput->InputValue.Value.string, StringPtr, NULL);
      FreePool (StringPtr);
      return EFI_SUCCESS;
    } else {
      *OptionString = AllocateZeroPool (BufferSize);
      ASSERT (*OptionString);

      if (((CHAR16 *) Question->CurrentValue.Buffer)[0] == 0x0000) {
        *(OptionString[0]) = '_';
      } else {
        if (Question->CurrentValue.BufferLen < BufferSize) {
          BufferSize = Question->CurrentValue.BufferLen;
        }
        CopyMem (OptionString[0], (CHAR16 *) Question->CurrentValue.Buffer, BufferSize);
      }
    }
    break;

  case EFI_IFR_PASSWORD_OP:
    if (Selected) {
      Status = PasswordProcess (MenuOption);
    }
    break;

  default:
    break;
  }

  return Status;
}