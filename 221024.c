calculateCRC(const void *buf, guint len) {
  guint16 crc = crc16_0x3D65_seed((const guint8 *)buf, len, 0);
  return ~crc;
}