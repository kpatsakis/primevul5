u8 sqlite3VdbeOneByteSerialTypeLen(u8 serial_type){
  assert( serial_type<128 );
  return sqlite3SmallTypeSizes[serial_type];  
}