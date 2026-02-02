#pragma once

struct VERTEX_Pos { float x, y, z; };
struct VERTEX_Nrm { float nx, ny, nz; };
struct VERTEX_UV { float u, v; };
struct VERTEX_T { float tx, ty, tz, tw; };// w성분에는 handedness
struct VERTEX_B { float bx, by, bz; };
struct VERTEX_BoneIndex { uint32_t i0, i1, i2, i3; };
struct VERTEX_BoneWeight { float w0, w1, w2, w3; };
struct INDEX { uint32_t V0, V1, V2; };
