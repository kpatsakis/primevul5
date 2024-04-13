calculateCRCtvb(tvbuff_t *tvb, guint offset, guint len) {
  guint16 crc = crc16_0x3D65_tvb_offset_seed(tvb, offset, len, 0);
  return ~crc;
}