bool IsSubsetOf(const T& sub, const T& super) {
  for (const auto& o : sub) {
    bool found = false;
    for (const auto& n : super) {
      if (o == n) {
        found = true;
        break;
      }
    }
    if (!found) return false;
  }
  return true;
}