#!/usr/bin/env python3

# SPDX-License-Identifier: MIT
#
# Copyright (c) 2022 Antonio Niño Díaz <antonio_nd@outlook.com>

import os

from collections import namedtuple
from math import sqrt

from display_list import DisplayList, float_to_f32

class MD5FormatError(Exception):
    pass

VALID_TEXTURE_SIZES = [8, 16, 32, 64, 128, 256, 512, 1024]

def is_valid_texture_size(size):
    return size in VALID_TEXTURE_SIZES

def assert_num_args(cmd, real, expected, tokens):
    if real != expected:
        raise MD5FormatError(f"Unexpected nargs for '{cmd}' ({real} != {expected}): {tokens}")

class Quaternion():
    def __init__(self, w, x, y, z):
        self.w = w
        self.x = x
        self.y = y
        self.z = z

    def to_v3(self):
        return Vector(self.x, self.y, self.z)

    def complement(self):
        return Quaternion(self.w, -self.x, -self.y, -self.z)

    def normalize(self):
        mag = sqrt((self.w ** 2) + (self.x ** 2) + (self.y ** 2) + (self.z ** 2))
        return Quaternion(self.w / mag, self.x /mag, self.y / mag, self.z / mag)

    def mul(self, other):
        w = (self.w * other.w) - (self.x * other.x) - (self.y * other.y) - (self.z * other.z)
        x = (self.x * other.w) + (self.w * other.x) + (self.y * other.z) - (self.z * other.y)
        y = (self.y * other.w) + (self.w * other.y) + (self.z * other.x) - (self.x * other.z)
        z = (self.z * other.w) + (self.w * other.z) + (self.x * other.y) - (self.y * other.x)
        return Quaternion(w, x, y, z)

def quaternion_fill_incomplete_w(v):
    """
    This expands an incomplete quaternion, not a regular vector. This is
    needed if a quaternion is stored as the components x, y and z and it is
    expected that the code will fill the value of w.
    """
    t = 1.0 - (v[0] * v[0]) - (v[1] * v[1]) - (v[2] * v[2])
    if t < 0:
        w = 0
    else:
        w = -sqrt(t)
    return Quaternion(w, v[0], v[1], v[2])

class Vector():
    def __init__(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z

    def to_q(self):
        return Quaternion(0, self.x, self.y, self.z)

    def length(self):
        return sqrt((self.x ** 2) + (self.y ** 2) + (self.z ** 2))

    def normalize(self):
        mag = self.length()
        return Vector(self.x / mag, self.y / mag, self.z / mag)

    def add(self, other):
        return Vector(self.x + other.x, self.y + other.y, self.z + other.z)

    def sub(self, other):
        return Vector(self.x - other.x, self.y - other.y, self.z - other.z)

    def cross(self, other):
        x = (self.y * other.z) - (other.y * self.z)
        y = (self.z * other.x) - (other.z * self.x)
        z = (self.x * other.y) - (other.x * self.y)
        return Vector(x, y, z)

    def mul_m4x3(self, m):
        x = (self.x * m[0][0]) + (self.y * m[0][1]) + (self.z * m[0][2]) + (m[0][3] * 1)
        y = (self.x * m[1][0]) + (self.y * m[1][1]) + (self.z * m[1][2]) + (m[1][3] * 1)
        z = (self.x * m[2][0]) + (self.y * m[2][1]) + (self.z * m[2][2]) + (m[2][3] * 1)
        return Vector(x, y, z)

def joint_info_to_m4x3(q, trans):
    """
    This generates a 4x3 matrix that represents a rotation and a translation.
    q is a Quaternion with a orientation, trans is a Vector with a translation.
    """
    wx = 2 * q.w * q.x
    wy = 2 * q.w * q.y
    wz = 2 * q.w * q.z
    x2 = 2 * q.x * q.x
    xy = 2 * q.x * q.y
    xz = 2 * q.x * q.z
    y2 = 2 * q.y * q.y
    yz = 2 * q.y * q.z
    z2 = 2 * q.z * q.z

    return [[1 - y2 - z2,     xy - wz,     xz + wy, trans.x],
            [    xy + wz, 1 - x2 - z2,     yz - wx, trans.y],
            [    xz - wy,     yz + wx, 1 - x2 - y2, trans.z]]

def parse_md5mesh(input_file):
    Joint = namedtuple("Joint", "name parent pos orient")
    Vert = namedtuple("Vert", "st startWeight countWeight")
    Weight = namedtuple("Weight", "joint bias pos")
    Mesh = namedtuple("Mesh", "numverts verts numtris tris numweights weights")

    joints = []
    meshes = []

    with open(input_file, 'r') as md5mesh_file:

        numJoints = None
        numMeshes = None

        # This can have three values:
        # - "root": Parsing commands in the md5mesh outside of any group
        # - "joints": Inside a "joints" node.
        # - "mesh": Inside a "mesh" node.
        mode = "root"

        # Temporary variables used to store mesh information before packing it
        numverts = None
        verts = None
        numtris = None
        tris = None
        numweights = None
        weights = None

        for line in md5mesh_file:
            # Remove comments
            line = line.split('//')[0]

            # Parse line
            tokens = line.split()

            if len(tokens) == 0:  # Empty line
                continue

            cmd = tokens[0]
            tokens = tokens[1:]
            nargs = len(tokens)

            if mode == "root":
                if cmd == 'MD5Version':
                    assert_num_args('MD5Version', nargs, 1, tokens)
                    version = int(tokens[0])
                    if version != 10:
                        raise MD5FormatError(f"Invalid 'MD5Version': {version} != 10")

                elif cmd == 'commandline':
                    # Ignore this
                    pass

                elif cmd == 'numJoints':
                    assert_num_args('numJoints', nargs, 1, tokens)
                    numJoints = int(tokens[0])
                    if numJoints == 0:
                        raise MD5FormatError(f"'numJoints' is 0")

                elif cmd == 'numMeshes':
                    assert_num_args('numMeshes', nargs, 1, tokens)
                    numMeshes = int(tokens[0])
                    if numMeshes == 0:
                        raise MD5FormatError(f"'numMeshes' is 0")

                elif cmd == 'joints':
                    assert_num_args('joints', nargs, 1, tokens)
                    if tokens[0] != '{':
                        raise MD5FormatError(f"Unexpected token for 'joints': {tokens}")
                    if numJoints is None:
                        raise MD5FormatError("'joints' command before 'numJoints'")
                    mode = "joints"

                elif cmd == 'mesh':
                    assert_num_args('mesh', nargs, 1, tokens)
                    if tokens[0] != '{':
                        raise MD5FormatError(f"Unexpected token for 'mesh': {tokens}")
                    if numMeshes is None:
                        raise MD5FormatError("'mesh' command before 'numMeshes'")
                    mode = "mesh"

                else:
                    print(f"Ignored unsupported command: {cmd} {tokens}")

            elif mode == "joints":
                if cmd == '}':
                    if nargs > 0:
                        raise MD5FormatError(f"Unexpected tokens after 'joints {{}}': {tokens}")
                    mode = "root"
                else:
                    _, name, line = line.split('"')
                    tokens = line.strip().split(" ")
                    nargs = len(tokens)

                    assert_num_args('joint entry', nargs, 11, tokens)

                    parent = int(tokens[0])

                    if tokens[1] != '(':
                        raise MD5FormatError(f"Unexpected token 1 for joint': {tokens}")
                    pos = Vector(float(tokens[2]), float(tokens[3]), float(tokens[4]))
                    if tokens[5] != ')':
                        raise MD5FormatError(f"Unexpected token 5 for joint': {tokens}")

                    if tokens[6] != '(':
                        raise MD5FormatError(f"Unexpected token 6 for joint': {tokens}")
                    orient = (float(tokens[7]), float(tokens[8]), float(tokens[9]))
                    q_orient = quaternion_fill_incomplete_w(orient)
                    if tokens[10] != ')':
                        raise MD5FormatError(f"Unexpected token 10 for joint': {tokens}")

                    joints.append(Joint(name, parent, pos, q_orient))

            elif mode == "mesh":
                if cmd == '}':
                    if nargs != 0:
                        raise MD5FormatError(f"Unexpected tokens after 'mesh {{}}': {tokens}")
                    mode = "root"

                    meshes.append(Mesh(numverts, verts, numtris, tris, numweights, weights))

                    numverts = None
                    verts = None
                    numtris = None
                    tris = None
                    numweights = None
                    weights = None

                elif cmd == 'shader':
                    # Ignore this
                    pass

                elif cmd == 'numverts':
                    assert_num_args('numverts', nargs, 1, tokens)
                    numverts = int(tokens[0])
                    verts = [None] * numverts

                elif cmd == 'vert':
                    assert_num_args('vert', nargs, 7, tokens)
                    if numverts is None:
                        raise MD5FormatError("'vert' command before 'numverts'")

                    index = int(tokens[0])

                    if tokens[1] != '(':
                        raise MD5FormatError(f"Unexpected token 1 for vert': {tokens}")
                    st = (float(tokens[2]), float(tokens[3]))
                    if tokens[4] != ')':
                        raise MD5FormatError(f"Unexpected token 4 for vert': {tokens}")

                    startWeight = int(tokens[5])
                    countWeight = int(tokens[6])

                    if countWeight != 1:
                        raise MD5FormatError(
                            f"Vertex with {countWeight} weights detected, but this tool "
                            "only supports vertices with one weight. Ensure that all your "
                            "vertices are assigned exactly one weight with a bias of 1.0."
                        )

                    verts[index] = Vert(st, startWeight, countWeight)

                elif cmd == 'numtris':
                    assert_num_args('numtris', nargs, 1, tokens)
                    numtris = int(tokens[0])
                    tris = [None] * numtris

                elif cmd == 'tri':
                    assert_num_args('tri', nargs, 4, tokens)
                    if numtris is None:
                        raise MD5FormatError("'tri' command before 'numtris'")

                    index = int(tokens[0])
                    # Reverse order so that they face the right direction
                    vertIndices = (int(tokens[3]), int(tokens[2]), int(tokens[1]))

                    tris[index] = vertIndices

                elif cmd == 'numweights':
                    assert_num_args('numweights', nargs, 1, tokens)
                    numweights = int(tokens[0])
                    weights = [None] * numweights

                elif cmd == 'weight':
                    assert_num_args('weight', nargs, 8, tokens)
                    if numverts is None:
                        raise MD5FormatError("'weight' command before 'numweights'")

                    index = int(tokens[0])
                    jointIndex = int(tokens[1])
                    bias = float(tokens[2])

                    if bias != 1.0:
                        raise MD5FormatError(
                            f"Weight with bias {bias} detected, but this tool only"
                            "supports weights with bias equal to 1.0. Ensure that all"
                            "your vertices are assigned exactly one weight with a"
                            "bias of 1.0."
                        )

                    if tokens[3] != '(':
                        raise MD5FormatError(f"Unexpected token 3 for weight': {tokens}")
                    pos = Vector(float(tokens[4]), float(tokens[5]), float(tokens[6]))
                    if tokens[7] != ')':
                        raise MD5FormatError(f"Unexpected token 7 for weight': {tokens}")

                    weights[index] = Weight(jointIndex, bias, pos)

                else:
                    print(f"Ignored unsupported command: {cmd} {tokens}")

        if mode != "root":
            raise MD5FormatError("Unexpected end of file (expected '}')")

    realJoints = len(joints)
    if numJoints != realJoints:
        raise MD5FormatError(f"Incorrect number of joints: {numJoints} != {realJoints}")

    realMeshes = len(meshes)
    if numJoints != realJoints:
        raise MD5FormatError(f"Incorrect number of joints: {numJoints} != {realJoints}")

    return (joints, meshes)

def parse_md5anim(input_file):
    Joint = namedtuple("Joint", "name parent pos orient")

    joints = []
    frames = []

    with open(input_file, 'r') as md5anim_file:

        numFrames = None
        numJoints = None

        baseframe = []
        hierarchy = []

        # This can have three values:
        # - "root": Parsing commands in the md5mesh outside of any group
        # - "hierarchy": Inside a "hierarchy" node.
        # - "bounds": Inside a "bounds" node.
        # - "baseframe": Inside a "baseframe" node.
        # - "frame": Inside a "frame" node.
        mode = "root"

        frame_index = None

        for line in md5anim_file:
            # Remove comments
            line = line.split('//')[0]

            # Parse line
            tokens = line.split()

            if len(tokens) == 0:  # Empty line
                continue

            cmd = tokens[0]
            tokens = tokens[1:]
            nargs = len(tokens)

            if mode == "root":
                if cmd == 'MD5Version':
                    assert_num_args('MD5Version', nargs, 1, tokens)
                    version = int(tokens[0])
                    if version != 10:
                        raise MD5FormatError(f"Invalid 'MD5Version': {version} != 10")

                elif cmd == 'commandline':
                    # Ignore this
                    pass

                elif cmd == 'numFrames':
                    assert_num_args('numFrames', nargs, 1, tokens)
                    numFrames = int(tokens[0])
                    if numFrames == 0:
                        raise MD5FormatError(f"'numFrames' is 0")
                    frames = [None] * numFrames

                elif cmd == 'numJoints':
                    assert_num_args('numJoints', nargs, 1, tokens)
                    numJoints = int(tokens[0])
                    if numJoints == 0:
                        raise MD5FormatError(f"'numJoints' is 0")

                elif cmd == 'frameRate':
                    # Ignore this
                    pass

                elif cmd == 'numAnimatedComponents':
                    # Ignore this
                    pass

                elif cmd == 'hierarchy':
                    assert_num_args('hierarchy', nargs, 1, tokens)
                    if tokens[0] != '{':
                        raise MD5FormatError(f"Unexpected token for 'hierarchy': {tokens}")
                    mode = "hierarchy"

                elif cmd == 'bounds':
                    assert_num_args('bounds', nargs, 1, tokens)
                    if tokens[0] != '{':
                        raise MD5FormatError(f"Unexpected token for 'bounds': {tokens}")
                    mode = "bounds"

                elif cmd == 'baseframe':
                    assert_num_args('baseframe', nargs, 1, tokens)
                    if tokens[0] != '{':
                        raise MD5FormatError(f"Unexpected token for 'baseframe': {tokens}")
                    mode = "baseframe"

                elif cmd == 'frame':
                    assert_num_args('frame', nargs, 2, tokens)
                    frame_index = int(tokens[0])

                    if tokens[1] != '{':
                        raise MD5FormatError(f"Unexpected token for 'frame': {tokens}")
                    if numFrames is None:
                        raise MD5FormatError("'frame' command before 'numFrames'")
                    mode = "frame"
                    joints = []

                else:
                    print(f"Ignored unsupported command: {cmd} {tokens}")

            elif mode == "hierarchy":
                if cmd == '}':
                    if nargs > 0:
                        raise MD5FormatError(f"Unexpected tokens after 'hierarchy {{}}': {tokens}")
                    mode = "root"
                else:
                    _, name, line = line.split('"')
                    tokens = line.strip().split(" ")
                    nargs = len(tokens)

                    assert_num_args('hierarchy entry', nargs, 3, tokens)

                    parent_index = int(tokens[0])
                    flags = int(tokens[1])
                    if flags != 63:
                        raise MD5FormatError(f"Unexpected flags in hierarchy: {flags}")
                    frame_data_index = int(tokens[2])

                    hierarchy.append(parent_index)

            elif mode == "bounds":
                if cmd == '}':
                    if nargs > 0:
                        raise MD5FormatError(f"Unexpected tokens after 'bounds {{}}': {tokens}")
                    mode = "root"
                else:
                    # Ignore everything else
                    pass

            elif mode == "baseframe":
                if cmd == '}':
                    if nargs > 0:
                        raise MD5FormatError(f"Unexpected tokens after 'baseframe {{}}': {tokens}")
                    mode = "root"
                else:
                    values = line.strip().split()
                    assert_num_args('baseframe joint', len(values), 10, values)

                    if values[0] != '(':
                        raise MD5FormatError(f"Unexpected token 0 for baseframe': {values}")
                    pos = Vector(float(values[1]), float(values[2]), float(values[3]))
                    if values[4] != ')':
                        raise MD5FormatError(f"Unexpected token 4 for baseframe': {values}")

                    if values[5] != '(':
                        raise MD5FormatError(f"Unexpected token 5 for baseframe': {values}")
                    orient = (float(values[6]), float(values[7]), float(values[8]))
                    q_orient = quaternion_fill_incomplete_w(orient)
                    if values[9] != ')':
                        raise MD5FormatError(f"Unexpected token 9 for baseframe': {values}")

                    baseframe.append(Joint("", -1, pos, q_orient))

            elif mode == "frame":
                if cmd == '}':
                    if nargs > 0:
                        raise MD5FormatError(f"Unexpected tokens after 'frame {{}}': {tokens}")
                    mode = "root"

                    # Now that the frame has been read, process the real
                    # positions and orientations of the bones before storing
                    # them.

                    transformed_joints = []

                    for joint, parent_index in zip(joints, hierarchy):
                        if parent_index == -1:
                            # Root bone
                            transformed_joints.append(joint)
                        else:
                            parent_pos = transformed_joints[parent_index].pos
                            parent_orient = transformed_joints[parent_index].orient

                            this_pos = joint.pos
                            this_orient = joint.orient

                            q = parent_orient
                            qt = q.complement()
                            q_pos_delta = q.mul(this_pos.to_q()).mul(qt)
                            pos_delta = q_pos_delta.to_v3()

                            pos = parent_pos.add(pos_delta)
                            orient = parent_orient.mul(this_orient).normalize()

                            transformed_joints.append(Joint("", -1, pos, orient))

                    frames[frame_index] = transformed_joints
                else:
                    values = line.strip().split()
                    assert_num_args('frame joint', len(values), 6, values)

                    pos = Vector(float(values[0]), float(values[1]), float(values[2]))

                    orient = (float(values[3]), float(values[4]), float(values[5]))
                    q_orient = quaternion_fill_incomplete_w(orient)

                    joints.append(Joint("", -1, pos, q_orient))

        if mode != "root":
            raise MD5FormatError("Unexpected end of file (expected '}')")

    realJoints = len(joints)
    if numJoints != realJoints:
        raise MD5FormatError(f"Incorrect number of joints: {numJoints} != {realJoints}")

    realFrames = len(frames)
    if numFrames != realFrames:
        raise MD5FormatError(f"Incorrect number of frames: {numFrames} != {realFrames}")

    return frames

def save_animation(frames, output_file, blender_fix):

    version = 1
    num_frames = len(frames)
    num_bones = len(frames[0])

    u32_array = [version, num_frames, num_bones]

    for joints in frames:
        if num_bones != len(joints):
            raise MD5FormatError("Different number of bones across frames")

        for joint in joints:
            this_pos = joint.pos
            this_orient = joint.orient

            if blender_fix:
                # It is needed to rotate all bones because all bones have
                # absolute transformations. Rotate orientation and position by
                # -90 degrees on the X axis.
                q_rot = Quaternion(0.7071068, -0.7071068, 0, 0)
                this_orient = q_rot.mul(this_orient)
                this_pos = Vector(this_pos.x, this_pos.z, -this_pos.y)

            pos = [float_to_f32(this_pos.x), float_to_f32(this_pos.y),
                   float_to_f32(this_pos.z)]
            orient = [float_to_f32(this_orient.w), float_to_f32(this_orient.x),
                      float_to_f32(this_orient.y), float_to_f32(this_orient.z)]

            u32_array.extend(pos)
            u32_array.extend(orient)

    with open(output_file, "wb") as f:
        for u32 in u32_array:
            b = [u32 & 0xFF, \
                (u32 >> 8) & 0xFF, \
                (u32 >> 16) & 0xFF, \
                (u32 >> 24) & 0xFF]
            f.write(bytearray(b))

def convert_md5mesh(model_file, name, output_folder, texture_size,
                    draw_normal_polygons, suffix, blender_fix,
                    export_base_pose):

    print(f"Converting model: {model_file}")

    # Parse md5mesh file
    joints, meshes = parse_md5mesh(model_file)

    print(f"Loaded {len(joints)} joint(s) and {len(meshes)} mesh(es).")

    if len(meshes) > 1:
        print("WARNING: More than one mesh found. All meshes will share the same "
              "texture. If you want them to have different textures, you must use "
              "multiple .md5mesh files.")

    if export_base_pose:
        print("Converting base pose...")

        save_animation([joints],
                       os.path.join(output_folder, f"{name}.dsa{suffix}"),
                       blender_fix)

    print("Converting meshes...")

    # Display list shared between all meshes
    dl = DisplayList()
    dl.switch_vtxs("triangles")

    base_matrix = 30 - len(joints) + 1
    last_joint_index = None

    for mesh in meshes:
        print(f"  Vertices: {mesh.numverts}")
        print(f"  Tris:     {mesh.numtris}")
        print(f"  Weights:  {mesh.numweights}")

        print("  Generating per-triangle normals...")

        tri_normal = []
        for tri in mesh.tris:
            verts = [mesh.verts[i] for i in tri]
            weights = [mesh.weights[v.startWeight] for v in verts]

            vtx = []
            for vert, weight in zip(verts, weights):
                joint = joints[weight.joint]
                m = joint_info_to_m4x3(joint.orient, joint.pos)
                final = weight.pos.mul_m4x3(m)
                vtx.append(final)

            a = vtx[0].sub(vtx[1])
            b = vtx[1].sub(vtx[2])

            n = a.cross(b)

            if n.length() > 0:
                n = n.normalize()
                tri_normal.append(n)
            else:
                tri_normal.append(Vector(0, 0, 0))

        print("  Generating display list...")

        for tri, norm in zip(mesh.tris, tri_normal):
            verts = [mesh.verts[i] for i in tri]
            weights = [mesh.weights[v.startWeight] for v in verts]

            finals = []

            for vert, weight in zip(verts, weights):

                # Texture
                # -------

                st = vert.st

                u = st[0] * texture_size[0]
                v = st[1] * texture_size[1]
                dl.texcoord(u, v)

                # Vertex and normal
                # -----------------

                # Load joint matrix. When drawing normal polygons it has to be
                # loaded every time, because drawing the normal restores the
                # original matrix.

                joint_index = weight.joint
                if draw_normal_polygons or joint_index != last_joint_index:
                    dl.mtx_restore(base_matrix + joint_index)
                    last_joint_index = joint_index

                # Calculate normal in joint space

                joint = joints[joint_index]

                q = joint.orient
                qt = q.complement()
                n = norm.to_q()

                # Transform by the inverted quaternion
                n = qt.mul(n).mul(q).to_v3()
                if n.length() > 0:
                    n = n.normalize()
                dl.normal(n.x, n.y, n.z)

                # The vertex is already in joint space

                dl.vtx(weight.pos.x, weight.pos.y, weight.pos.z)

                if draw_normal_polygons:
                    # Calculate actual location of the vertex so that the
                    # vertices of the triangle can be averaged as origin of the
                    # normal polygon.
                    q = joint.orient
                    qt = q.complement()
                    v = weight.pos.to_q()

                    delta = q.mul(v).mul(qt).to_v3()

                    final = joint.pos.add(delta)
                    finals.append(final)

            if draw_normal_polygons:

                # Don't use any of the joint transformation matrices
                dl.mtx_restore(1)

                vert_avg = Vector(
                    (finals[0].x + finals[1].x + finals[2].x) / 3,
                    (finals[0].y + finals[1].y + finals[2].y) / 3,
                    (finals[0].z + finals[1].z + finals[2].z) / 3
                )

                vert_avg_end = vert_avg.add(norm)

                dl.texcoord(0, 0)

                dl.color(1, 0, 0)
                dl.vtx(vert_avg.x + 0.1, vert_avg.y, vert_avg.z)
                dl.vtx(vert_avg.x, vert_avg.y, vert_avg.z)
                dl.color(0, 1, 0)
                dl.vtx(vert_avg_end.x, vert_avg_end.y, vert_avg_end.z)

                dl.color(1, 0, 0)
                dl.vtx(vert_avg.x, vert_avg.y, vert_avg.z)
                dl.vtx(vert_avg.x, vert_avg.y + 0.1, vert_avg.z)
                dl.color(0, 1, 0)
                dl.vtx(vert_avg_end.x, vert_avg_end.y, vert_avg_end.z)

                dl.color(1, 0, 0)
                dl.vtx(vert_avg.x, vert_avg.y, vert_avg.z)
                dl.vtx(vert_avg.x, vert_avg.y, vert_avg.z + 0.1)
                dl.color(0, 1, 0)
                dl.vtx(vert_avg_end.x, vert_avg_end.y, vert_avg_end.z)

    dl.end_vtxs()
    dl.finalize()

    dl.save_to_file(os.path.join(output_folder, f"{name}.dsm{suffix}"))


def convert_md5anim(name, output_folder, anim_file, skip_frames, suffix,
                    blender_fix):

    print(f"Converting animation: {anim_file}")

    frames = parse_md5anim(anim_file)

    # Create name of animation based on file name
    file_basename = os.path.basename(anim_file).replace(".md5anim", "")
    anim_name = file_basename.replace(".", "_").lower()

    frames = frames[::skip_frames+1]
    save_animation(frames, os.path.join(output_folder,
                   f"{name}_{anim_name}.dsa{suffix}"), blender_fix)


if __name__ == "__main__":

    import argparse
    import sys
    import traceback

    print("md5_to_dsma v0.1.0")
    print("Copyright (c) 2022 Antonio Niño Díaz <antonio_nd@outlook.com>")
    print("All rights reserved")
    print("")

    parser = argparse.ArgumentParser(
            description='Converts md5mesh and md5anim files into DSM and DSA files.')

    # Required arguments
    parser.add_argument("--name", required=True,
                        help="model name to be used in output files")
    parser.add_argument("--output", required=True,
                        help="output folder")

    # Optional arguments
    parser.add_argument("--model", required=False, type=str, default=None,
                        help="input md5mesh file")
    parser.add_argument("--texture", required=False, type=int, default=[],
                        nargs="+", action="extend",
                        help="texture width and height (e.g. '--texture 32 64')")
    parser.add_argument("--anims", required=False, type=str, default=[],
                        nargs="+", action="extend",
                        help="list of md5anim files to convert")
    parser.add_argument("--bin", required=False,
                        action='store_true',
                        help="add '.bin' to the name of the output files")
    parser.add_argument("--blender-fix", required=False,
                        action='store_true',
                        help="rotate model -90 degrees on X axis to match Blender's orientation")
    parser.add_argument("--export-base-pose", required=False,
                        action='store_true',
                        help="export base pose of a md5mesh as a DSA file")
    parser.add_argument("--skip-frames", required=False,
                        default=0, type=int,
                        help="number of frames to skip in an animation (0 = export all, 1 = export half, 2 = export 33%, etc)")
    parser.add_argument("--draw-normal-polygons", required=False,
                        action='store_true',
                        help="draw polygons with the shape of normals for debugging")

    args = parser.parse_args()

    if args.model is not None:
        if len(args.texture) != 2:
            print("Please, provide exactly 2 values to the --texture argument")
            sys.exit(1)

        if not is_valid_texture_size(args.texture[0]):
            print(f"Invalid texture width. Valid values: {VALID_TEXTURE_SIZES}")
            sys.exit(1)

        if not is_valid_texture_size(args.texture[1]):
            print(f"Invalid texture height. Valid values: {VALID_TEXTURE_SIZES}")
            sys.exit(1)

    # Create output directory if it doesn't exist
    os.makedirs(args.output, exist_ok=True)

    # Add '.bin' to the name of the files if requested
    suffix = ".bin" if args.bin else ""

    try:
        if args.model is not None:
            convert_md5mesh(args.model, args.name, args.output, args.texture,
                            args.draw_normal_polygons, suffix, args.blender_fix,
                            args.export_base_pose)

        for anim_file in args.anims:
            convert_md5anim(args.name, args.output, anim_file, args.skip_frames,
                            suffix, args.blender_fix)

    except BaseException as e:
        print("ERROR: " + str(e))
        traceback.print_exc()
        sys.exit(1)
    except MD5FormatError as e:
        print("ERROR: Invalid MD5 file: " + str(e))
        traceback.print_exc()
        sys.exit(1)

    print("Done!")

    sys.exit(0)
