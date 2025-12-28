#pragma once

struct VERTEX_Pos { float x, y, z; };
struct VERTEX_Nrm { float nx, ny, nz; };
struct VERTEX_UV { float u, v; };
struct VERTEX_T { float tx, ty, tz, tw; };// w성분에는 handedness
struct VERTEX_B { float bx, by, bz; };
struct INDEX { uint32_t V0, V1, V2; };