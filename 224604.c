u32 sqlite3VdbeSerialTypeLen(u32 serial_type){
  if( serial_type>=128 ){
    return (serial_type-12)/2;
  }else{
    assert( serial_type<12 
            || sqlite3SmallTypeSizes[serial_type]==(serial_type - 12)/2 );
    return sqlite3SmallTypeSizes[serial_type];
  }
}