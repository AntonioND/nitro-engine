# SPDX-License-Identifier: MIT
#
# Copyright (c) 2022 Antonio Niño Díaz <antonio_nd@outlook.com>

def float_to_v16(val):
    res = int(val * (1 << 12))
    if res < -0x8000:
        raise OverflowError(f"{val} too small for v16: {res:#04x}")
    if res > 0x7FFF:
        raise OverflowError(f"{val} too big for v16: {res:#04x}")
    if res < 0:
        res = 0x10000 + res
    return res

def v16_to_float(val):
    return val / (1 << 12)

def float_to_v10(val):
    res = int(val * (1 << 6))
    if res < -0x200:
        raise OverflowError(f"{val} too small for v10: {res:#03x}")
    if res > 0x1FF:
        raise OverflowError(f"{val} too big for v10: {res:#03x}")
    if res < 0:
        res = 0x400 + res
    return res

def v10_to_float(val):
    return val / (1 << 6)

def float_to_diff10(val):
    res = int(val * (1 << 9))
    if res < -0x200:
        raise OverflowError(f"{val} too small for diff10: {res:#03x}")
    if res > 0x1FF:
        raise OverflowError(f"{val} too big for diff10: {res:#03x}")
    if res < 0:
        res = 0x400 + res
    return res

def diff10_to_float(val):
    return val / (1 << 9)

def float_to_t16(val):
    res = int(val * (1 << 4))
    if res < -0x8000:
        raise OverflowError(f"{val} too small for t16: {res:#04x}")
    if res > 0x7FFF:
        raise OverflowError(f"{val} too big for t16: {res:#04x}")
    if res < 0:
        res = 0x10000 + res
    return res

def float_to_n10(val):
    res = int(val * (1 << 9))
    if res < -0x200:
        #raise OverflowError(f"{val} too small for n10: {res:#03x}")
        res = -0x200
    if res > 0x1FF:
        #raise OverflowError(f"{val} too big for n10: {res:#03x}")
        res = 0x1FF
    if res < 0:
        res = 0x400 + res
    return res

def command_name_to_id(name):
    commands = {
        "NOP": 0x00, # (0) No Operation (for padding packed GXFIFO commands)
        "MTX_MODE": 0x10, # (1) Set Matrix Mode
        "MTX_PUSH": 0x11,  # (0) Push Current Matrix on Stack
        "MTX_POP": 0x12, # (1) Pop Current Matrix from Stack
        "MTX_STORE": 0x13, # (1) Store Current Matrix on Stack
        "MTX_RESTORE": 0x14, # (1) Restore Current Matrix from Stack
        "MTX_IDENTITY": 0x15, # (0) Load Unit Matrix to Current Matrix
        "MTX_LOAD_4x4": 0x16, # (16) Load 4x4 Matrix to Current Matrix
        "MTX_LOAD_4x3": 0x17, # (12) Load 4x3 Matrix to Current Matrix
        "MTX_MULT_4x4": 0x18, # (16) Multiply Current Matrix by 4x4 Matrix
        "MTX_MULT_4x3": 0x19, # (12) Multiply Current Matrix by 4x3 Matrix
        "MTX_MULT_3x3": 0x1A, # (9) Multiply Current Matrix by 3x3 Matrix
        "MTX_SCALE": 0x1B, # (3) Multiply Current Matrix by Scale Matrix
        "MTX_TRANS": 0x1C, # (3) Mult. Curr. Matrix by Translation Matrix
        "COLOR": 0x20, # (1) Directly Set Vertex Color
        "NORMAL": 0x21, # (1) Set Normal Vector
        "TEXCOORD": 0x22, # (1) Set Texture Coordinates
        "VTX_16": 0x23, # (2) Set Vertex XYZ Coordinates
        "VTX_10": 0x24, # (1) Set Vertex XYZ Coordinates
        "VTX_XY": 0x25, # (1) Set Vertex XY Coordinates
        "VTX_XZ": 0x26, # (1) Set Vertex XZ Coordinates
        "VTX_YZ": 0x27, # (1) Set Vertex YZ Coordinates
        "VTX_DIFF": 0x28, # (1) Set Relative Vertex Coordinates
        "POLYGON_ATTR": 0x29, # (1) Set Polygon Attributes
        "TEXIMAGE_PARAM": 0x2A, # (1) Set Texture Parameters
        "PLTT_BASE": 0x2B, # (1) Set Texture Palette Base Address
        "DIF_AMB": 0x30, # (1) MaterialColor0 # Diffuse/Ambient Reflect.
        "SPE_EMI": 0x31, # (1) MaterialColor1 # Specular Ref. & Emission
        "LIGHT_VECTOR": 0x32, # (1) Set Light's Directional Vector
        "LIGHT_COLOR": 0x33, # (1) Set Light Color
        "SHININESS": 0x34, # (32) Specular Reflection Shininess Table
        "BEGIN_VTXS": 0x40, # (1) Start of Vertex List
        "END_VTXS": 0x41, # (0) End of Vertex List
        "SWAP_BUFFERS": 0x50, # (1) Swap Rendering Engine Buffer
        "VIEWPORT": 0x60, # (1) Set Viewport
        "BOX_TEST": 0x70, # (3) Test if Cuboid Sits inside View Volume
        "POS_TEST": 0x71, # (2) Set Position Coordinates for Test
        "VEC_TEST": 0x72, # (1) Set Directional Vector for Test
    }
    return commands[name]

def poly_type_to_id(name):
    types = {
        "triangles": 0,
        "quads": 1,
        "triangle_strip": 2,
        "quad_strip": 3,
    }
    return types[name]

def error(x1, x2, y1, y2, z1, z2):
    return (abs(x1 - x2) ** 2) + (abs(y1 - y2) ** 2) + (abs(z1 - z2) ** 2)

class DisplayList():

    def __init__(self):
        self.commands = []
        self.parameters = []
        self.vtx_last = None
        self.texcoord_last = None
        self.normal_last = None
        self.begin_vtx_last = None

        self.display_list = []

    def add_command(self, command, *args):
        self.commands.append(command)
        if len(args) > 0:
            self.parameters.extend(args)

        if len(self.commands) == 4:
            header = self.commands[0] | self.commands[1] << 8 | \
                     self.commands[2] << 16 | self.commands[3] << 24

            self.display_list.append(header)
            self.display_list.extend(self.parameters)

            self.commands = []
            self.parameters = []

    def finalize(self):
        # If there are pending commands, add NOPs to complete the display list
        if len(self.commands) > 0:
            padding = 4 - len(self.commands)
            for i in range(padding):
                self.nop()

        # Prepend size to the list
        self.display_list.insert(0, len(self.display_list))

    def save_to_file(self, path):
        with open(path, "wb") as f:
            for u32 in self.display_list:
                b = [u32 & 0xFF, \
                    (u32 >> 8) & 0xFF, \
                    (u32 >> 16) & 0xFF, \
                    (u32 >> 24) & 0xFF]
                f.write(bytearray(b))

    def nop(self):
        self.add_command(command_name_to_id("NOP"))

    def mtx_restore(self, index):
        self.add_command(command_name_to_id("MTX_RESTORE"), index)

    def color(self, r, g, b):
        arg = int(r * 31) | (int(g * 31) << 5) | (int(b * 31) << 10)
        self.add_command(command_name_to_id("COLOR"), arg)

    def normal(self, x, y, z):
        # Skip if it's the same normal
        if self.normal_last is not None:
            if self.normal_last[0] == x and self.normal_last[1] == y and \
               self.normal_last[2] == z:
                return

        arg = float_to_n10(x) | (float_to_n10(y) << 10) | float_to_n10(z) << 20
        self.add_command(command_name_to_id("NORMAL"), arg)
        self.normal_last = (x, y, z)

    def texcoord(self, u, v):
        # Skip if it's the same texcoord
        if self.texcoord_last is not None:
            if self.texcoord_last[0] == u and self.texcoord_last[1] == v:
                return

        arg = float_to_t16(u) | (float_to_t16(v) << 16)
        self.add_command(command_name_to_id("TEXCOORD"), arg)
        self.texcoord_last = (u, v)

    def vtx_16(self, x, y, z):
        args = [float_to_v16(x) | (float_to_v16(y) << 16), float_to_v16(z)]
        self.add_command(command_name_to_id("VTX_16"), *args)
        self.vtx_last = (x, y, z)

    def vtx_10(self, x, y, z):
        arg = float_to_v10(x) | (float_to_v10(y) << 10) | float_to_v10(z) << 20
        self.add_command(command_name_to_id("VTX_10"), arg)
        self.vtx_last = (x, y, z)

    def vtx_xy(self, x, y):
        arg = float_to_v16(x) | (float_to_v16(y) << 16)
        self.add_command(command_name_to_id("VTX_XY"), arg)
        self.vtx_last = (x, y, self.vtx_last[2])

    def vtx_xz(self, x, z):
        arg = float_to_v16(x) | (float_to_v16(z) << 16)
        self.add_command(command_name_to_id("VTX_XZ"), arg)
        self.vtx_last = (x, self.vtx_last[1], z)

    def vtx_yz(self, y, z):
        arg = float_to_v16(y) | (float_to_v16(z) << 16)
        self.add_command(command_name_to_id("VTX_YZ"), arg)
        self.vtx_last = (self.vtx_last[0], y, z)

    def vtx_diff(self, x, y, z):
        arg = float_to_diff10(x - self.vtx_last[0]) | \
             (float_to_diff10(y - self.vtx_last[1]) << 10) | \
             (float_to_diff10(z - self.vtx_last[2]) << 20)
        self.add_command(command_name_to_id("VTX_DIFF"), arg)
        self.vtx_last = (x, y, z)

    def vtx(self, x, y, z):
        """
        Picks the best vtx command based on the previous vertex and the error of
        the conversion.
        """
        # Allow {vtx_xy, vtx_yz, vtx_xz, vtx_diff} if there is a previous vertex
        allow_diff = self.vtx_last is not None

        # First, check if any of the coordinates is exactly the same as the
        # previous command. We can trivially use vtx_xy, vtx_xz, vtx_yz because
        # they have the min possible size and the max possible accuracy
        if allow_diff:
            if float_to_v16(self.vtx_last[0]) == float_to_v16(x):
                self.vtx_yz(y, z)
                return
            elif float_to_v16(self.vtx_last[1]) == float_to_v16(y):
                self.vtx_xz(x, z)
                return
            elif float_to_v16(self.vtx_last[2]) == float_to_v16(z):
                self.vtx_xy(x, y)
                return

        # If not, there are three options: vtx_16, vtx_10, vtx_diff. Pick the
        # one with the lowest error.

        # TODO: Maybe use vtx_diff, but this may cause accuracy issues if it is
        # used several times in a row.

        error_vtx_16 = error(v16_to_float(float_to_v16(x)), x,
                             v16_to_float(float_to_v16(y)), y,
                             v16_to_float(float_to_v16(z)), z)

        error_vtx_10 = error(v10_to_float(float_to_v10(x)), x,
                             v10_to_float(float_to_v10(y)), y,
                             v10_to_float(float_to_v10(z)), z)

        if error_vtx_10 <= error_vtx_16:
            self.vtx_10(x, y, z)
        else:
            self.vtx_16(x, y, z)

        return

    def begin_vtxs(self, poly_type):
        self.add_command(command_name_to_id("BEGIN_VTXS"), poly_type_to_id(poly_type))
        self.begin_vtx_last = poly_type

    def end_vtxs(self):
        self.add_command(command_name_to_id("END_VTXS"))
        self.begin_vtx_last = None

    def switch_vtxs(self, poly_type):
        """Sends a new BEGIN_VTXS if the polygon type has changed."""
        if self.begin_vtx_last != poly_type:
            if self.begin_vtx_last is not None:
                self.end_vtxs()
            self.begin_vtxs(poly_type)

if __name__ == "__main__":
    dl = DisplayList()
    dl.begin_vtxs("triangles")
    dl.color(1.0, 0, 0)
    dl.vtx_16(1.0, -1.0, 0)
    dl.color(0, 1.0, 0)
    dl.vtx_10(1.0, 1.0, 0)
    dl.color(0, 0, 1.0)
    dl.vtx_xy(-1.0, -1.0)
    dl.end_vtxs()
    dl.finalize()
    print(', '.join([hex(i) for i in dl.display_list]))
    dl.save_to_file("test.bin")
