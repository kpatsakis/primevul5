static inline bool IsStart(int32_t entry) {
  return (entry & kStartBit) != 0;
}