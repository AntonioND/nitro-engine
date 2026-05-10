#!/usr/bin/env python3

# SPDX-License-Identifier: MIT
#
# Copyright (c) 2022-2024 Antonio Niño Díaz <antonio_nd@outlook.com>

from display_list import DisplayList
from collections import defaultdict

class OBJFormatError(Exception):
    pass

VALID_TEXTURE_SIZES = [8, 16, 32, 64, 128, 256, 512, 1024]

def is_valid_texture_size(size):
    return size in VALID_TEXTURE_SIZES

def parse_face_vertex(vertex_str, use_vertex_color):
    """Parse 'v/vt/vn' string, return (vertex_idx, texcoord_idx, normal_idx) 0-based."""
    tokens = vertex_str.split('/')
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
        if not use_vertex_color:
            normal_index = int(tokens[2])
    else:
        raise OBJFormatError(f"Invalid face vertex {vertex_str}")

    if vertex_index < 0:
        raise OBJFormatError("Unsupported negative indices")
    vertex_index -= 1

    if texcoord_index is not None:
        if texcoord_index < 0:
            raise OBJFormatError("Unsupported negative indices")
        texcoord_index -= 1

    if normal_index is not None:
        if normal_index < 0:
            raise OBJFormatError("Unsupported negative indices")
        normal_index -= 1

    return (vertex_index, texcoord_index, normal_index)

# ---------------------------------------------------------------------------
# Triangle strip helpers
# ---------------------------------------------------------------------------

def _tri_directed_edge_third(tri, p, q):
    """If triangle (a,b,c) CCW has directed edge p->q, return third vertex."""
    a, b, c = tri
    if a == p and b == q: return c
    if b == p and c == q: return a
    if c == p and a == q: return b
    return None

def stripify_triangles(resolved_tris):
    """Greedy triangle stripification.
    resolved_tris: list of (vk0, vk1, vk2) in CCW order.
    Returns (strips, singles) where:
      strips = list of vertex-key lists (each is a triangle strip sequence)
      singles = list of face indices not in any strip
    """
    if not resolved_tris:
        return [], []

    edge_to_faces = defaultdict(list)
    for fi, tri in enumerate(resolved_tris):
        for i in range(3):
            edge = frozenset([tri[i], tri[(i + 1) % 3]])
            edge_to_faces[edge].append(fi)

    used = set()
    strips = []
    singles = []

    for start_fi in range(len(resolved_tris)):
        if start_fi in used:
            continue

        best_strip = None
        best_faces = None

        face = resolved_tris[start_fi]
        # Try all 3 CCW rotations as forward direction
        for rot in range(3):
            a = face[rot]
            b = face[(rot + 1) % 3]
            c = face[(rot + 2) % 3]

            strip = [a, b, c]
            faces = [start_fi]
            local_used = set(used)
            local_used.add(start_fi)

            # Extend forward
            while True:
                n = len(faces)  # index of next triangle in strip
                p, q = strip[-2], strip[-1]
                edge_key = frozenset([p, q])

                found = False
                for cfi in edge_to_faces[edge_key]:
                    if cfi in local_used:
                        continue
                    tri = resolved_tris[cfi]
                    # Even position: need directed edge p->q in candidate
                    # Odd position: need directed edge q->p in candidate
                    if n % 2 == 0:
                        new_v = _tri_directed_edge_third(tri, p, q)
                    else:
                        new_v = _tri_directed_edge_third(tri, q, p)

                    if new_v is not None:
                        strip.append(new_v)
                        faces.append(cfi)
                        local_used.add(cfi)
                        found = True
                        break

                if not found:
                    break

            if best_faces is None or len(faces) > len(best_faces):
                best_strip = strip
                best_faces = faces

        for fi in best_faces:
            used.add(fi)

        if len(best_faces) >= 2:
            strips.append(best_strip)
        else:
            singles.append(start_fi)

    return strips, singles

# ---------------------------------------------------------------------------
# Quad strip helpers
# ---------------------------------------------------------------------------

def stripify_quads(resolved_quads):
    """Greedy quad stripification.
    resolved_quads: list of (vk0, vk1, vk2, vk3) in CCW order.

    NDS quad strip layout:
      Strip sequence: s0, s1, s2, s3, s4, s5, ...
      Quad i uses: (s[2i], s[2i+1], s[2i+3], s[2i+2]) in CCW
      So for OBJ quad (a,b,c,d) CCW -> strip emits a, b, d, c
      Exit directed edge (right side) = c->d in CCW
      Next quad must have entry directed edge d->c (reversed)

    Returns (strips, singles).
    """
    if not resolved_quads:
        return [], []

    edge_to_faces = defaultdict(list)
    for fi, quad in enumerate(resolved_quads):
        for i in range(4):
            edge = frozenset([quad[i], quad[(i + 1) % 4]])
            edge_to_faces[edge].append(fi)

    used = set()
    strips = []
    singles = []

    for start_fi in range(len(resolved_quads)):
        if start_fi in used:
            continue

        best_strip = None
        best_faces = None
        quad = resolved_quads[start_fi]

        # Try all 4 rotations (determines which edge pair is entry/exit)
        for rot in range(4):
            v = [quad[(rot + j) % 4] for j in range(4)]
            # CCW: v[0], v[1], v[2], v[3]
            # Strip order: v[0], v[1], v[3], v[2]
            strip = [v[0], v[1], v[3], v[2]]
            faces = [start_fi]
            local_used = set(used)
            local_used.add(start_fi)

            # Exit directed edge in CCW: v[2]->v[3]
            exit_p, exit_q = v[2], v[3]

            while True:
                edge_key = frozenset([exit_p, exit_q])
                found = False
                for cfi in edge_to_faces[edge_key]:
                    if cfi in local_used:
                        continue
                    candidate = resolved_quads[cfi]
                    # Need directed edge exit_q->exit_p in candidate CCW
                    for ci in range(4):
                        if candidate[ci] == exit_q and candidate[(ci + 1) % 4] == exit_p:
                            cv = [candidate[(ci + j) % 4] for j in range(4)]
                            # cv CCW: cv[0]=exit_q, cv[1]=exit_p, cv[2], cv[3]
                            # Strip adds: cv[3], cv[2]
                            strip.extend([cv[3], cv[2]])
                            faces.append(cfi)
                            local_used.add(cfi)
                            exit_p, exit_q = cv[2], cv[3]
                            found = True
                            break
                    if found:
                        break
                if not found:
                    break

            if best_faces is None or len(faces) > len(best_faces):
                best_strip = strip
                best_faces = faces

        for fi in best_faces:
            used.add(fi)

        if len(best_faces) >= 2:
            strips.append(best_strip)
        else:
            singles.append(start_fi)

    return strips, singles

# ---------------------------------------------------------------------------
# Vertex emission
# ---------------------------------------------------------------------------

def emit_vertex(dl, vk, vertices, texcoords, normals, texture_size,
                model_scale, model_translation, use_vertex_color):
    """Emit all display-list commands for one vertex key."""
    vertex_idx, texcoord_idx, normal_idx = vk

    if texcoord_idx is not None:
        u, v = texcoords[texcoord_idx]
        # OBJ (0,0) = bottom-left, NDS (0,0) = top-left
        v = 1.0 - v
        u *= texture_size[0]
        v *= texture_size[1]
        dl.texcoord(u, v)

    if normal_idx is not None:
        n = normals[normal_idx]
        dl.normal(n[0], n[1], n[2])

    vtx = []
    for i in range(3):
        val = vertices[vertex_idx][i]
        val += model_translation[i]
        val *= model_scale
        vtx.append(val)

    if use_vertex_color:
        rgb = [vertices[vertex_idx][i] for i in range(3, 6)]
        dl.color(*rgb)

    dl.vtx(vtx[0], vtx[1], vtx[2])

# ---------------------------------------------------------------------------
# Main conversion
# ---------------------------------------------------------------------------

def convert_obj(input_file, output_file, texture_size,
                model_scale, model_translation, use_vertex_color,
                no_strip=False):

    vertices = []
    texcoords = []
    normals = []
    faces = []

    with open(input_file, 'r') as obj_file:
        for line in obj_file:
            line = line.split('#')[0]
            tokens = line.split()
            if len(tokens) < 2:
                continue

            cmd = tokens[0]
            tokens = tokens[1:]

            if cmd == 'v':
                if len(tokens) == 3:
                    if use_vertex_color:
                        raise OBJFormatError(f"Found vertex with no color info: {tokens}")
                    v = [float(tokens[i]) for i in range(3)]
                elif len(tokens) == 6:
                    v = [float(tokens[i]) for i in range(6)]
                else:
                    raise OBJFormatError(f"Unsupported vertex command: {tokens}")
                vertices.append(v)

            elif cmd == 'vt':
                v = (float(tokens[0]), float(tokens[1]))
                texcoords.append(v)

            elif cmd == 'vn':
                v = (float(tokens[0]), float(tokens[1]), float(tokens[2]))
                normals.append(v)

            elif cmd == 'f':
                faces.append(tokens)

            elif cmd == 'l':
                raise OBJFormatError(f"Unsupported polyline command: {tokens}")

            else:
                print(f"Ignored unsupported command: {cmd} {tokens}")

    print("Vertices:  " + str(len(vertices)))
    print("Texcoords: " + str(len(texcoords)))
    print("Normals:   " + str(len(normals)))
    print("Faces:     " + str(len(faces)))
    print("")

    # Resolve faces into vertex-key tuples
    resolved_tris = []
    resolved_quads = []

    for face in faces:
        n = len(face)
        if n != 3 and n != 4:
            raise OBJFormatError(
                f"Unsupported polygons with {n} faces. "
                "Please, split the polygons in your model to triangles."
            )
        vkeys = tuple(parse_face_vertex(v, use_vertex_color) for v in face)
        if n == 3:
            resolved_tris.append(vkeys)
        else:
            resolved_quads.append(vkeys)

    # Stripify or fall back to separate primitives
    if no_strip:
        tri_strips, tri_singles = [], list(range(len(resolved_tris)))
        quad_strips, quad_singles = [], list(range(len(resolved_quads)))
    else:
        tri_strips, tri_singles = stripify_triangles(resolved_tris)
        quad_strips, quad_singles = stripify_quads(resolved_quads)

    # Statistics
    separate_vtx = len(resolved_tris) * 3 + len(resolved_quads) * 4
    strip_vtx = (sum(len(s) for s in tri_strips) + len(tri_singles) * 3
               + sum(len(s) for s in quad_strips) + len(quad_singles) * 4)

    tri_stripped = len(resolved_tris) - len(tri_singles)
    quad_stripped = len(resolved_quads) - len(quad_singles)

    print(f"Triangle strips: {len(tri_strips)} ({tri_stripped} faces stripped, "
          f"{len(tri_singles)} separate)")
    print(f"Quad strips:     {len(quad_strips)} ({quad_stripped} faces stripped, "
          f"{len(quad_singles)} separate)")
    print(f"GPU vertices:    {separate_vtx} -> {strip_vtx} "
          f"(saved {separate_vtx - strip_vtx})")
    print("")

    # Emit display list
    dl = DisplayList()

    # Emit triangle strips
    for strip_verts in tri_strips:
        dl.begin_vtxs("triangle_strip")
        for vk in strip_verts:
            emit_vertex(dl, vk, vertices, texcoords, normals, texture_size,
                        model_scale, model_translation, use_vertex_color)
        dl.end_vtxs()

    # Emit separate triangles (original CCW order)
    if tri_singles:
        dl.begin_vtxs("triangles")
        for fi in tri_singles:
            for vk in resolved_tris[fi]:
                emit_vertex(dl, vk, vertices, texcoords, normals, texture_size,
                            model_scale, model_translation, use_vertex_color)
        dl.end_vtxs()

    # Emit quad strips
    for strip_verts in quad_strips:
        dl.begin_vtxs("quad_strip")
        for vk in strip_verts:
            emit_vertex(dl, vk, vertices, texcoords, normals, texture_size,
                        model_scale, model_translation, use_vertex_color)
        dl.end_vtxs()

    # Emit separate quads (original CCW order)
    if quad_singles:
        dl.begin_vtxs("quads")
        for fi in quad_singles:
            for vk in resolved_quads[fi]:
                emit_vertex(dl, vk, vertices, texcoords, normals, texture_size,
                            model_scale, model_translation, use_vertex_color)
        dl.end_vtxs()

    dl.finalize()
    dl.save_to_file(output_file)

if __name__ == "__main__":

    import argparse
    import sys
    import traceback

    print("obj2dl v0.2.0")
    print("Copyright (c) 2022-2024 Antonio Niño Díaz <antonio_nd@outlook.com>")
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
    parser.add_argument("--use-vertex-color", required=False,
                        action='store_true',
                        help="use vertex colors instead of normals")
    parser.add_argument("--no-strip", required=False,
                        action='store_true',
                        help="disable strip generation (original behavior)")

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
                    args.scale, args.translation, args.use_vertex_color,
                    args.no_strip)
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
