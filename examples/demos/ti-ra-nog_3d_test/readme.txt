Nitro Engine POC 3D Test by Ti-Ra-Nog
-------------------------------------

If you want to enable the FAT mesh and/or texture reading, first uncomment the
line: "#define FAT_MESH_TEXT" and then compile it.

After you have the binary file with the FAT texture/mesh reading enabled, copy
this files to the root of your card or put them in the same folder where the nds
is for test it on an emulator (with fat support).

In orther to use the screenshot or "video" recording function you need to
enable the FAT access to your card, if you have a new card reader for nds you
may no need to patch it, but if you have an older one may you need to patch the
binary file with DLDI patch for you card reader.

In the binary folder there are three files, one no patched binary and two
patched for R4 and Super card SD.

For making screenshots you need to create the "/screen" folder in the root
directory of your card and then the screens should be there.

The "video" recording function make one screenshot per VBL and save it in to the
video flother that you need to create in the root of the memory (i whasn't able
to create the directory if it doesn't exist). When you push the "R" button the
ds will make one screnshot for every vbl and you'll see the screen blinking and
making some strange with the scenes, don't worry, the screenshots will be ok.
The video will be recorded to  "/video/vid".

When you have finished the the "video" recording you'll need to make a animated
gif or avi video with some software.

So, that's all, I think that you'll love this Engine :D
