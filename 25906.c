int my_aio_read(struct aiocb *aiocbp)
{
   if( (MYFSEEK(aiocbp->aio_fildes, (off_t)aiocbp->aio_offset, SEEK_SET) == 0)
   &&  (fread(aiocbp->aio_buf, 1, aiocbp->aio_nbytes, aiocbp->aio_fildes)
       == aiocbp->aio_nbytes) )
   {
      aiocbp->aio_lio_opcode = 0;
   }
   else
   {
      aiocbp->aio_lio_opcode = -1;
   }

   return(aiocbp->aio_lio_opcode);
}