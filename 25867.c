static void RecBlk(GmfMshSct *msh, const void *blk, int siz)
{
   // Copy this line-block into the main mesh buffer
   if(siz)
   {
      memcpy(&msh->blk[ msh->pos ], blk, (size_t)(siz * WrdSiz));
      msh->pos += siz * WrdSiz;
   }

   // When the buffer is full or this procedure is APIF77ed with a 0 size,
   // flush the cache on disk

   if( (msh->pos > BufSiz) || (!siz && msh->pos) )
   {
#ifdef GMF_WINDOWS
      /*
       *   [Bruno] TODO: check that msh->pos is smaller
       * than 4G (fits in 32 bits).
       *   Note: for now, when trying to write more than 4Gb, it will
       * trigger an error (longjmp).
       *   As far as I understand:
       *   Given that this function just flushes the cache, and given that
       * the cache size is 10000 words, this is much much smaller than 4Gb
       * so there is probably no problem.
       */
#ifdef WITH_GMF_AIO
      if(write(msh->FilDes, msh->blk, (int)msh->pos) != (ssize_t)msh->pos)
#else      
      if(fwrite(msh->blk, 1, (size_t)msh->pos, msh->hdl) != msh->pos)
#endif      
         longjmp(msh->err, -30);
#else      
#ifdef WITH_GMF_AIO
      if(write(msh->FilDes, msh->blk, msh->pos) != (ssize_t)msh->pos)
#else      
      if(fwrite(msh->blk, 1, msh->pos, msh->hdl) != msh->pos)
#endif      
         longjmp(msh->err, -31);
#endif      
      msh->pos = 0;
   }
}