Those tools are used to export models created on the PC to the NDS:

Made for Nitro Engine:

- Nitro_Texture_Converter:
    It converts any PNG into any DS texture format (except compressed format).
    It uses the alpha channel (if any) for texture transparency.

- MD2_2_BIN:
    Exports the first frame of an MD2 model to a NDS display list. This is more
    optimized than NDS_Model_Exporter.

- MD2_2_NEA:
    Exports every frame of an MD2 model to a NEA file that can be used by Nitro
    Engine.

Made by others:

- NDS_Model_Exporter:
    Windows only! It exports models in some formats to NDS display lists. It can
    export textures too, but only to RGB format.
