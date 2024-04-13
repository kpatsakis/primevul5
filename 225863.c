template <class T, int s> bool Predicate<T, s>::CalculateValue(
    uchar code_point) {
  bool result = T::Is(code_point);
  entries_[code_point & kMask] = CacheEntry(code_point, result);
  return result;
}