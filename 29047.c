static inline GfxColorComp clip01(GfxColorComp x) {
  return (x < 0) ? 0 : (x > gfxColorComp1) ? gfxColorComp1 : x;
}
