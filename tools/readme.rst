Nitro Engine Tools
==================

The following tools are used to export models created on the PC to the NDS:

- **obj2dl**

  Converts a Wavefront OBJ file into a NDS display list.

- **md5_to_dsma**

  Converts MD5 models with skeletal animation (md5mesh and md5anim files) into a
  format that allows them to be displayed on the NDS efficiently.

  https://github.com/AntonioND/dsma-library

- **img2ds**

  Converts images in several formats to NDS textures and palettes. It is
  recommended to use PNG files with transparency.

  This tool has been deprecated. You should only use it for the depth bitmap
  (DEPTHBMP), as this conversion isn't supported by any other tool.
