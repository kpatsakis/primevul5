static INLINE void cliprdr_write_lock_unlock_clipdata(wStream* s, UINT32 clipDataId)
{
	Stream_Write_UINT32(s, clipDataId);
}