static void *AcquireLZMAMemory(void *context,size_t items,size_t size)
{
  (void) context;
  return((void *) AcquireQuantumMemory((size_t) items,(size_t) size));
}