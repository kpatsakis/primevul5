passGetAttributes(CharsString *passLine, int *passLinepos,
		TranslationTableCharacterAttributes *attributes, const FileInfo *file) {
	int more = 1;
	*attributes = 0;
	while (more) {
		switch (passLine->chars[*passLinepos]) {
		case pass_any:
			*attributes = 0xffffffff;
			break;
		case pass_digit:
			*attributes |= CTC_Digit;
			break;
		case pass_litDigit:
			*attributes |= CTC_LitDigit;
			break;
		case pass_letter:
			*attributes |= CTC_Letter;
			break;
		case pass_math:
			*attributes |= CTC_Math;
			break;
		case pass_punctuation:
			*attributes |= CTC_Punctuation;
			break;
		case pass_sign:
			*attributes |= CTC_Sign;
			break;
		case pass_space:
			*attributes |= CTC_Space;
			break;
		case pass_uppercase:
			*attributes |= CTC_UpperCase;
			break;
		case pass_lowercase:
			*attributes |= CTC_LowerCase;
			break;
		case pass_class1:
			*attributes |= CTC_UserDefined9;
			break;
		case pass_class2:
			*attributes |= CTC_UserDefined10;
			break;
		case pass_class3:
			*attributes |= CTC_UserDefined11;
			break;
		case pass_class4:
			*attributes |= CTC_UserDefined12;
			break;
		default:
			more = 0;
			break;
		}
		if (more) (*passLinepos)++;
	}
	if (!*attributes) {
		compileError(file, "missing attribute");
		(*passLinepos)--;
		return 0;
	}
	return 1;
}