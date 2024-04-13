template <class T, int s> bool Predicate<T, s>::get(uchar code_point) {
  CacheEntry entry = entries_[code_point & kMask];
  if (entry.code_point_ == code_point) return entry.value_;
  return CalculateValue(code_point);
}