img2ds
======

Converts images in several formats to NDS textures and palettes. It is recommended to use PNG files with transparency.

Dependencies
------------

Install the dependencies with:

.. code:: bash

   pip3 install -r requirements.txt

Usage
-----

.. code:: python

   python3 img2ds.py --input teapot.png --name texture --output data --format A1RGB5

In this example, the file "teapot.png" is converted to an NDS texture with the ".bin" extension. The converted file is in the A1RGB5 format and is saved to the "data" subfolder of the current working directory. 

All four arguments are required: the input file, preferably a ".png", the name for the converted texture, the output directory, and the format. Texture dimensions must be powers of two, at least 8px, and no more than 1024 px. Alpha values of 0 in the input will be preserved in the result, but any non-zero alpha values will be 1 in the result. 

Valid formats
-------------
- "A1RGB5"
- "PAL256"
- "PAL16"
- "PAL4"
- "A3PAL32" 
- "A5PAL8"
- "DEPTHBMP"
