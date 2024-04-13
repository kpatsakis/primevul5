static inline double clip01(double x) {
  return (x < 0) ? 0 : (x > 1) ? 1 : x;
}
