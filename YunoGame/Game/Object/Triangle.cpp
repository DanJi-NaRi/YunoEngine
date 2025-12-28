#include "pch.h"
#include "Triangle.h"
#include "IMesh.h"

VERTEX_Pos g_Triangle_pos[] = {
    {  0.0f,  0.5f,  0.0f},
    {  0.5f, -0.5f,  0.0f},
    { -0.5f, -0.5f,  0.0f}
};

VERTEX_Nrm g_Triangle_nrm[] =
{
    {  0.0f,  0.0f,  1.0f},
    {  0.0f,  0.0f,  1.0f},
    {  0.0f,  0.0f,  1.0f}
};

VERTEX_UV g_Triangle_uv[] =
{
    {  0.0f,  1.0f},
    {  0.0f,  1.0f},
    {  0.0f,  1.0f}
};