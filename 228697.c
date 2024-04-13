enum uni_interp interp_from_encoding(Encoding *enc,enum uni_interp interp) {

    if ( enc==NULL )
return( interp );

    if ( enc->is_japanese )
	interp = ui_japanese;
    else if ( enc->is_korean )
	interp = ui_korean;
    else if ( enc->is_tradchinese )
	interp = ui_trad_chinese;
    else if ( enc->is_simplechinese )
	interp = ui_simp_chinese;
return( interp );
}