INLINE BOOL gdi_PtInRect(HGDI_RECT rc, INT32 x, INT32 y)
{
	/*
	 * points on the left and top sides are considered in,
	 * while points on the right and bottom sides are considered out
	 */
	if (x >= rc->left && x <= rc->right)
	{
		if (y >= rc->top && y <= rc->bottom)
		{
			return TRUE;
		}
	}

	return FALSE;
}