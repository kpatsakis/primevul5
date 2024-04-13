TfLiteFloatArray* TfLiteFloatArrayCreate(int size) {
  TfLiteFloatArray* ret =
      (TfLiteFloatArray*)malloc(TfLiteFloatArrayGetSizeInBytes(size));
  ret->size = size;
  return ret;
}