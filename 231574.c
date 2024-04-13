readLineOffsets (OPENEXR_IMF_INTERNAL_NAMESPACE::IStream &is,
		 LineOrder lineOrder,
		 vector<Int64> &lineOffsets,
		 bool &complete)
{
    for (unsigned int i = 0; i < lineOffsets.size(); i++)
    {
	OPENEXR_IMF_INTERNAL_NAMESPACE::Xdr::read <OPENEXR_IMF_INTERNAL_NAMESPACE::StreamIO> (is, lineOffsets[i]);
    }

    complete = true;

    for (unsigned int i = 0; i < lineOffsets.size(); i++)
    {
	if (lineOffsets[i] <= 0)
	{
	    //
	    // Invalid data in the line offset table mean that
	    // the file is probably incomplete (the table is
	    // the last thing written to the file).  Either
	    // some process is still busy writing the file,
	    // or writing the file was aborted.
	    //
	    // We should still be able to read the existing
	    // parts of the file.  In order to do this, we
	    // have to make a sequential scan over the scan
	    // line data to reconstruct the line offset table.
	    //

	    complete = false;
	    reconstructLineOffsets (is, lineOrder, lineOffsets);
	    break;
	}
    }
}