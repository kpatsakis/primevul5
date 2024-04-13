check_dnp3_header(tvbuff_t *tvb, gboolean dnp3_heuristics)
{
  /* Assume the CRC will be bad */
  gboolean goodCRC = FALSE;

  /* How big is the actual buffer */
  gint length = tvb_captured_length(tvb);

  /* Calculate the header CRC if the bytes are available */
  if (length >= DNP_HDR_LEN) {
    guint16 calc_crc = calculateCRCtvb(tvb, 0, DNP_HDR_LEN - 2);
    goodCRC = (calc_crc == tvb_get_letohs(tvb, 8));
  }

  /* For a heuristic match we must have at least a header, beginning with 0x0564
     and a valid header CRC */
  if (dnp3_heuristics) {
    if ( !goodCRC || (tvb_get_ntohs(tvb, 0) != 0x0564)) {
      return FALSE;
    }
  }
  else {
    /* For a non-heuristic match, at least the first byte is 0x05 and if available
       the second byte is 64 and if available the CRC is valid */
    if (tvb_get_guint8(tvb, 0) != 0x05) {
      return FALSE;
    }
    if ((length > 1) && (tvb_get_guint8(tvb, 1) != 0x64)) {
      return FALSE;
    }
    if ((length >= DNP_HDR_LEN) && !goodCRC) {
      return FALSE;
    }
  }
  return TRUE;
}