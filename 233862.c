ms_escher_read_RegroupItems (MSEscherState *state, MSEscherHeader *h)
{
	/* Each shape in a drawing has a regroup ID (separate from the shape
	 * ID), so that the regroup command can find shapes that were once
	 * grouped. In order to handle nested grouping and ungrouping
	 * there is a table logging changes to regroup IDs. Each entry has an
	 * old ID and a new ID and records the change of all instances of the
	 * old ID to the new ID.
	 *
	 * The instance of an msofbtRegroupItems record contains the number of
	 * entries, and the record itself is just that many FRITs (File Regroup
	 * Items).
	 *
	 * typedef struct { FRID fridNew; FRID fridOld; } FRIT;
	 */

	return FALSE;
}