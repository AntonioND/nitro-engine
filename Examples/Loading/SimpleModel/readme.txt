First you have to put the .bin files in data folder. This will generate (after doing "make") some files named "binfilename_bin.h". For example, "model.bin" will generate a file named "model_bin.h". You have to include this in "main.c".

The name you will have to use is "binfilename_bin". For example, for loading "model.bin" you will use NE_ModelLoadStaticMesh(Model,(u32*)binfilename_bin);