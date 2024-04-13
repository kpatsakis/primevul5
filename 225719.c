static inline guint64 KEEP_32MSB_OF_GUINT64(guint64 nb) {
    return (nb >> 32) << 32;
}