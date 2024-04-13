get_dnp3_message_len(packet_info *pinfo _U_, tvbuff_t *tvb,
                     int offset, void *data _U_)
{
  guint16 message_len;  /* need 16 bits as total can exceed 255 */
  guint16 data_crc;     /* No. of user data CRC bytes */

  message_len = tvb_get_guint8(tvb, offset + 2);

  /* Add in 2 bytes for header start octets,
            1 byte for len itself,
            2 bytes for header CRC
            data CRC bytes (2 bytes per 16 bytes of data
  */

  data_crc = (guint16)(ceil((message_len - 5) / 16.0)) * 2;
  message_len += 2 + 1 + 2 + data_crc;
  return message_len;
}