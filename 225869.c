static inline uchar GetEntry(int32_t entry) {
  return entry & (kStartBit - 1);
}