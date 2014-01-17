BitmapManager
=============

A simple QT application written in C++ which grant to open different kind of Windows Bitmap (BMP) images

Actually the supported images' bit-depth are: 1, 4, 8, 24.
The bitmap file format supported is based on version 3 of the Windows Bitmap, more info about the 
format could be found on the MSDN pages: http://msdn.microsoft.com/en-us/library/windows/desktop/dd183371%28v=vs.85%29.aspx

FUTURE WORKS
--------------
 - Implement RLE compression algorithm for images with a bit-depth equal to 4 and 8 bit per pixel (needs some tests)
 - Support 16, 32 bit BMP images
