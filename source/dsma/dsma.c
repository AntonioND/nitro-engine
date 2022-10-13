// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022 Antonio Niño Díaz <antonio_nd@outlook.com>

// DS Model Animation Library v0.1.1

#include <nds.h>

#include "dsma.h"

// Format of a joint in a DSA file.
typedef struct {
    int32_t pos[3];    // Translation (x, y, z)
    int32_t orient[4]; // Orientation (w, x, y, z)
} dsa_joint_t;

#define DSA_VERSION_NUMBER 1

// Format of a DSA file.
typedef struct {
    uint32_t version;      // Version number
    uint32_t num_frames;   // Frames in the file
    uint32_t num_joints;   // Joints per frame
    dsa_joint_t joints[0]; // Array of joints
} dsa_t;

// Private functions
// =================

// Helper that multiplies two fixed point values in 20.12 format and multiplies
// the result again by 2.
ITCM_CODE ARM_CODE static inline
int32_t mulf32_by_2(int32_t a, int32_t b)
{
    return (a * b) >> (12 - 1);
}

// Generates a 4x3 matrix from the orientation in the provided quaternion and
// the translation in the provided vector. Then, it multiplies the matrix that
// is currently active in the geometry engine by the generated matrix.
ITCM_CODE ARM_CODE static inline
void matrix_mult_by_joint(const int32_t *v, const int32_t *q)
{
    int32_t wx = mulf32_by_2(q[0], q[1]);
    int32_t wy = mulf32_by_2(q[0], q[2]);
    int32_t wz = mulf32_by_2(q[0], q[3]);
    int32_t x2 = mulf32_by_2(q[1], q[1]);
    int32_t xy = mulf32_by_2(q[1], q[2]);
    int32_t xz = mulf32_by_2(q[1], q[3]);
    int32_t y2 = mulf32_by_2(q[2], q[2]);
    int32_t yz = mulf32_by_2(q[2], q[3]);
    int32_t z2 = mulf32_by_2(q[3], q[3]);

    MATRIX_MULT4x3 = inttof32(1) - y2 - z2;
    MATRIX_MULT4x3 = xy + wz;
    MATRIX_MULT4x3 = xz - wy;

    MATRIX_MULT4x3 = xy - wz;
    MATRIX_MULT4x3 = inttof32(1) - x2 - z2;
    MATRIX_MULT4x3 = yz + wx;

    MATRIX_MULT4x3 = xz + wy;
    MATRIX_MULT4x3 = yz - wx;
    MATRIX_MULT4x3 = inttof32(1) - x2 - y2;

    MATRIX_MULT4x3 = v[0];
    MATRIX_MULT4x3 = v[1];
    MATRIX_MULT4x3 = v[2];
}

// Gets a pointer to the list of joints of the specified frame.
ITCM_CODE ARM_CODE static inline
const dsa_joint_t *dsa_get_frame(const dsa_t *dsa, uint32_t frame)
{
    return &dsa->joints[frame * dsa->num_joints];
}

// Interpolates linearly between 'start' and 'end'. The position is a floating
// point number in 20.12 format, and it should be between 0.0 and 1.0 (the
// function doesn't check bounds).
ITCM_CODE ARM_CODE static inline
int32_t lerp(int32_t start, int32_t end, int32_t pos)
{
    int32_t diff = end - start;
    return start + ((diff * pos) >> 12);
}

// Interpolates between quaternions 'q1' and 'q2. The position is a floating
// point number in 20.12 format, and it should be between 0.0 and 1.0 (the
// function doesn't check bounds). It stores the result in 'qdest'.
ITCM_CODE ARM_CODE static inline
void q_nlerp(const int32_t *q1, const int32_t *q2, int32_t pos, int32_t *qdest)
{
    qdest[0] = lerp(q1[0], q2[0], pos);
    qdest[1] = lerp(q1[1], q2[1], pos);
    qdest[2] = lerp(q1[2], q2[2], pos);
    qdest[3] = lerp(q1[3], q2[3], pos);

    // TODO: Normalize? It needs way too much CPU time (at least we need one
    // square root and one division), but it may be needed in the future if the
    // animations look bad. Maybe it can be optional.
}

// Public functions
// ================

uint32_t DSMA_GetNumFrames(const void *dsa_file)
{
    const dsa_t *dsa = dsa_file;
    return dsa->num_frames;
}

ITCM_CODE ARM_CODE
int DSMA_DrawModel(const void *dsm_file, const void *dsa_file, uint32_t frame_interp)
{
    const dsa_t *dsa = dsa_file;

    if (dsa->version != DSA_VERSION_NUMBER)
        return DSMA_INVALID_VERSION;

    uint32_t num_joints = dsa->num_joints;
    uint32_t num_frames = dsa->num_frames;

    uint32_t frame = frame_interp >> 12;
    uint32_t interp = frame_interp & 0xFFF;

    if (frame >= num_frames)
        return DSMA_INVALID_FRAME;

    // Make sure that there is enough space in the matrix stack
    // --------------------------------------------------------

    uint32_t base_matrix = 30 - num_joints + 1;

    // Wait for matrix push/pop operations to end
    while (GFX_STATUS & BIT(14));

    uint32_t curr_stack_level = (GFX_STATUS >> 8) & 0x1F;
    if (curr_stack_level >= base_matrix)
        return DSMA_MATRIX_STACK_FULL;

    MATRIX_PUSH = 0;

    // Generate matrices with bone transformations
    // -------------------------------------------

    if (interp != 0)
    {
        uint32_t next_frame = frame + 1;
        if (next_frame == num_frames)
            next_frame = 0;

        const dsa_joint_t *frame_ptr_1 = dsa_get_frame(dsa, frame);
        const dsa_joint_t *frame_ptr_2 = dsa_get_frame(dsa, next_frame);

        for (uint32_t i = 0; i < num_joints; i++)
        {
            // Interpolate transformations
            const int32_t *v_pos_1 = frame_ptr_1->pos;
            const int32_t *q_orient_1 = frame_ptr_1->orient;
            frame_ptr_1++;

            const int32_t *v_pos_2 = frame_ptr_2->pos;
            const int32_t *q_orient_2 = frame_ptr_2->orient;
            frame_ptr_2++;

            int32_t v_pos[3];
            v_pos[0] = lerp(v_pos_1[0], v_pos_2[0], interp);
            v_pos[1] = lerp(v_pos_1[1], v_pos_2[1], interp);
            v_pos[2] = lerp(v_pos_1[2], v_pos_2[2], interp);

            int32_t q_orient[4];
            q_nlerp(q_orient_1, q_orient_2, interp, &q_orient[0]);

            // Generate new matrix
            MATRIX_RESTORE = curr_stack_level;
            matrix_mult_by_joint(v_pos, q_orient);

            // Store it in the right position in the stack
            MATRIX_STORE = base_matrix + i;
        }
    }
    else
    {
        const dsa_joint_t *frame_ptr = dsa_get_frame(dsa, frame);

        for (uint32_t i = 0; i < num_joints; i++)
        {
            // Get transformation
            const int32_t *v_pos = frame_ptr->pos;
            const int32_t *q_orient = frame_ptr->orient;
            frame_ptr++;

            // Generate new matrix
            MATRIX_RESTORE = curr_stack_level;
            matrix_mult_by_joint(v_pos, q_orient);

            // Store it in the right position in the stack
            MATRIX_STORE = base_matrix + i;
        }
    }

    // Draw model
    // ----------

    glCallList((uint32_t *)dsm_file);

    MATRIX_POP = 1;

    return DSMA_SUCCESS;
}
