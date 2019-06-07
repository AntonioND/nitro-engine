NDSModelExporter is a tool not created as part of Nitro Engine, and it is
provided here for conveniency. It supports more formats than MD2, but it exports
models with color commands included. This means that normal commands are
overriden and lights don't work on the models exported by this converter.

In order to make them work, you need to pass them on to dlfixer so that it
removes the color commands.
