#!/usr/bin/env python3

# SPDX-License-Identifier: MIT
#
# Copyright (c) 2022 Antonio Niño Díaz <antonio_nd@outlook.com>

from display_list import DisplayList

class OBJFormatError(Exception):
    pass

VALID_TEXTURE_SIZES = [8, 16, 32, 64, 128, 256, 512, 1024]

def is_valid_texture_size(size):
    return size in VALID_TEXTURE_SIZES

def convert_obj(input_file, output_file, texture_size,
                model_scale, model_translation):

    vertices = []
    texcoords = []
    normals = []
    faces = []

    with open(input_file, 'r') as obj_file:
        for line in obj_file:
            # Remove comments
            line = line.split('#')[0]

            # Parse line
            tokens = line.split()

            # Empty line, skip it
            if len(tokens) < 2:
                continue

            cmd = tokens[0]
            tokens = tokens[1:]

            if cmd == 'v': # Vertex
                v = (float(tokens[0]), float(tokens[1]), float(tokens[2]))
                vertices.append(v)

            elif cmd == 'vt': # Texture coordinate
                v = (float(tokens[0]), float(tokens[1]))
                texcoords.append(v)

            elif cmd == 'vn': # Normal
                v = (float(tokens[0]), float(tokens[1]), float(tokens[2]))
                normals.append(v)

            elif cmd == 'f': # Face
                v = (tokens[0], tokens[1], tokens[2])
                faces.append(v)

            elif cmd == 'g': # Group
                print(f"Ignored group command: {tokens}")

            elif cmd == 'o': # Object
                print(f"Ignored object command: {tokens}")

            elif cmd == 'l': # Polyline
                raise OBJFormatError("Unsupported polyline command: {tokens}")

            else:
                print("Ignored unsupported command: {cmd} {tokens}")

    print("Vertices:  " + str(len(vertices)))
    print("Texcoords: " + str(len(texcoords)))
    print("Normals:   " + str(len(normals)))
    print("Faces:     " + str(len(faces)))
    print("")

    dl = DisplayList()
    dl.begin_vtxs("triangles")

    for face in faces:
        for vertex in face:
            tokens = vertex.split('/')

            vertex_index = None
            texcoord_index = None
            normal_index = None

            if len(tokens) == 1:
                vertex_index = int(tokens[0])
            elif len(tokens) == 2:
                vertex_index = int(tokens[0])
                texcoord_index = int(tokens[1])
            elif len(tokens) == 3:
                vertex_index = int(tokens[0])
                if len(tokens[1]) > 0:
                    texcoord_index = int(tokens[1])
                normal_index = int(tokens[2])
            else:
                raise OBJFormatError(f"Invalid face {face}")

            # Vertex indices must always be provided
            if vertex_index < 0:
                raise OBJFormatError(f"Unsupported negative indices")
            vertex_index -= 1

            if texcoord_index is not None:
                if texcoord_index < 0:
                    raise OBJFormatError(f"Unsupported negative indices")
                texcoord_index -= 1

            if normal_index is not None:
                if normal_index < 0:
                    raise OBJFormatError(f"Unsupported negative indices")
                normal_index -= 1

            if texcoord_index is not None:
                u, v = texcoords[texcoord_index]
                u *= texture_size[0]
                v *= texture_size[1]
                dl.texcoord(u, v)

            if normal_index is not None:
                n = normals[normal_index]
                dl.normal(n[0], n[1], n[2])

            vtx = []
            for i in range(3):
                v = vertices[vertex_index][i]
                v += model_translation[i]
                v *= model_scale
                vtx.append(v)

            # Let the DisplayList class pick the best vtx command
            dl.vtx(vtx[0], vtx[1], vtx[2])

    dl.end_vtxs()
    dl.finalize()
    dl.save_to_file(output_file)

if __name__ == "__main__":

    import argparse
    import sys
    import traceback

    print("obj2dl v0.1.0")
    print("Copyright (c) 2022 Antonio Niño Díaz <antonio_nd@outlook.com>")
    print("All rights reserved")
    print("")

    parser = argparse.ArgumentParser(
            description='Convert Wavefront OBJ files into NDS display lists.')

    # Required arguments
    parser.add_argument("--input", required=True,
                        help="input file")
    parser.add_argument("--output", required=True,
                        help="output file")
    parser.add_argument("--texture", required=True, type=int,
                        nargs="+", action="extend",
                        help="texture width and height (e.g. '--texture 32 64')")

    # Optional arguments
    parser.add_argument("--translation", default=[0, 0, 0], type=float,
                        nargs="+", action="extend",
                        help="translate model by this value")
    parser.add_argument("--scale", default=1.0, type=float,
                        help="scale model by this value (after the translation)")

    args = parser.parse_args()

    if len(args.texture) != 2:
        print("Please, provide exactly 2 values to the --texture argument")
        sys.exit(1)

    if not is_valid_texture_size(args.texture[0]):
        print(f"Invalid texture width. Valid values: {VALID_TEXTURE_SIZES}")
        sys.exit(1)
    if not is_valid_texture_size(args.texture[1]):
        print(f"Invalid texture height. Valid values: {VALID_TEXTURE_SIZES}")
        sys.exit(1)

    if len(args.translation) != 3:
        print("Please, provide exactly 3 values to the --translation argument")
        sys.exit(1)

    try:
        convert_obj(args.input, args.output, args.texture,
                    args.scale, args.translation)
    except BaseException as e:
        print("ERROR: " + str(e))
        traceback.print_exc()
        sys.exit(1)
    except OBJFormatError as e:
        print("ERROR: Invalid OBJ file: " + str(e))
        traceback.print_exc()
        sys.exit(1)

    print("Done!")

    sys.exit(0)
