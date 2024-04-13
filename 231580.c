void LineBufferTaskIIF::getWritePointerStereo 
                          (int y,
                           unsigned short*& outWritePointerRight,
                           unsigned short*& outWritePointerLeft,
                           size_t& outPixelsToCopySSE,
                           size_t& outPixelsToCopyNormal) const
{
   getWritePointer<TYPE>(y,outWritePointerRight,outPixelsToCopySSE,outPixelsToCopyNormal,0);
   
   
   if(outWritePointerRight)
   {
       getWritePointer<TYPE>(y,outWritePointerLeft,outPixelsToCopySSE,outPixelsToCopyNormal,1);
   }
   
}