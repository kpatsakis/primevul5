void UnicodeString::swap(UnicodeString &other) U_NOEXCEPT {
  UnicodeString temp;  // Empty short string: Known not to need releaseArray().
  // Copy fields without resetting source values in between.
  temp.copyFieldsFrom(*this, FALSE);
  this->copyFieldsFrom(other, FALSE);
  other.copyFieldsFrom(temp, FALSE);
  // Set temp to an empty string so that other's memory is not released twice.
  temp.fUnion.fFields.fLengthAndFlags = kShortString;
}