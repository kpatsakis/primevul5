TfLiteStatus TfLiteTensorCopy(const TfLiteTensor* src, TfLiteTensor* dst) {
  if (!src || !dst)
    return kTfLiteOk;
  if (src->bytes != dst->bytes)
    return kTfLiteError;
  if (src == dst)
    return kTfLiteOk;

  dst->type = src->type;
  if (dst->dims)
    TfLiteIntArrayFree(dst->dims);
  dst->dims = TfLiteIntArrayCopy(src->dims);
  memcpy(dst->data.raw, src->data.raw, src->bytes);
  dst->buffer_handle = src->buffer_handle;
  dst->data_is_stale = src->data_is_stale;
  dst->delegate = src->delegate;

  return kTfLiteOk;
}