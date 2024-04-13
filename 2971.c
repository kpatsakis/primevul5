int TfLiteIntArrayEqual(const TfLiteIntArray* a, const TfLiteIntArray* b) {
  if (a == b) return 1;
  if (a == NULL || b == NULL) return 0;
  return TfLiteIntArrayEqualsArray(a, b->size, b->data);
}