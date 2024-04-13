static void *AcquireBZIPMemory(void *context,int items,int size)
{
  (void) context;
  return((void *) AcquireQuantumMemory((size_t) items,(size_t) size));
}