int32_t getGrowCapacity(int32_t newLength) {
  int32_t growSize = (newLength >> 2) + kGrowSize;
  if(growSize <= (kMaxCapacity - newLength)) {
    return newLength + growSize;
  } else {
    return kMaxCapacity;
  }
}