ms_escher_read_CLSID (MSEscherState *state, MSEscherHeader *h)
{
	/* Holds a 'Class ID Record' ID record which is only included in the
	 * 'clipboard format'.  It contains an OLE CLSID record from the source
	 * app, and can be used check where the clipboard data originated.
	 *
	 * We ignore these.  What is an 'OLE CLSID' ?  Would we ever need this ?
	 */
	return FALSE;
}