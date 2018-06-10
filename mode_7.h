

#ifndef MODE_7_H
#define MODE_7_H

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

/* http://www.coranac.com/tonc/text/mode7.htm */
/* http://www.coranac.com/tonc/text/mode7ex.htm */
/* http://gamedev.stackexchange.com/questions/24957/doing-an-snes-mode-7-affine-transform-effect-in-pygame */

#define X 0
#define Y 1
#define Z 2

typedef int m7_vec3_t[3];
typedef int m7_vec2_t[2];


typedef struct
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} m7_pixel_t;

typedef struct
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} m7_texel_t;

typedef unsigned char m7_dexel_t;

typedef struct
{
    m7_dexel_t* dexels;
    short w;
    short h;
} m7_depth_buffer_t;

typedef struct
{
    m7_pixel_t* pixels;
    short w;
    short h;
} m7_color_buffer_t;

typedef struct
{
    short w;
    short h;
    m7_texel_t* texels;
    int texture_id;
    int free;
} m7_texture_t;

typedef struct
{
    int texture_id;
    int free;
} m7_background_t;

typedef struct
{
    m7_vec3_t pos;
    int angle;
    float angle_rad;
} m7_cam_t;

typedef struct
{
    m7_vec3_t pos;
    m7_vec2_t anchor;
    
    m7_vec3_t pos_relative;
    
    short tx;
    short ty;
    
    short tw;
    short th;

    int texture_id;
    int free;
} m7_sprite_t;

#define M7_MAX_TEXTURES 32
#define M7_MAX_SPRITES 32
#define M7_MAX_BACKGROUNDS 4

typedef struct
{
    m7_texture_t textures[M7_MAX_TEXTURES];
    m7_background_t backgrounds[M7_MAX_BACKGROUNDS];
    m7_sprite_t sprites[M7_MAX_SPRITES];
    
    m7_color_buffer_t color_buffer;
    m7_depth_buffer_t depth_buffer;
    m7_cam_t cam;
    int horizon;
} m7_context_t;

extern void m7_texture_unpack(const m7_texture_t* tex,
                              short x,
                              short y,
                              m7_texel_t* out_texel);

extern void m7_texture_pack(m7_texture_t* tex,
                            short x,
                            short y,
                            const m7_texel_t* in_texel);

extern void m7_context_init(m7_context_t* ctx,
                            short w,
                            short h);

extern void m7_context_shutdown(m7_context_t* ctx);
                             

extern void m7_context_render(m7_context_t* ctx);

extern m7_texture_t* m7_context_create_texture(m7_context_t* ctx,
                                               short w,
                                               short h);

extern m7_background_t* m7_context_create_background(m7_context_t* ctx,
                                                     int texture_id);

extern m7_sprite_t* m7_context_create_sprite(m7_context_t* ctx,
                                             int texture_id);

#endif
