  void clearPixel(int x, int y)
    { data[y * line + (x >> 3)] &= 0x7f7f >> (x & 7); }
