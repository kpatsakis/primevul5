  int getPixel(int x, int y)
    { return (x < 0 || x >= w || y < 0 || y >= h) ? 0 :
             (data[y * line + (x >> 3)] >> (7 - (x & 7))) & 1; }
