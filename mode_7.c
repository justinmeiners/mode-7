
#include "mode_7.h"

static void _m7_texture_build(m7_texture_t* tex, short w, short h)
{
    assert(w > 0);
    assert(h > 0);
    tex->w = w;
    tex->h = h;
    tex->texels = malloc(w * h * sizeof(m7_texel_t));
}

void m7_texture_unpack(const m7_texture_t* tex,
                       short x,
                       short y,
                       m7_texel_t* out_texel)
{
    assert(x >= 0);
    assert(y >= 0);
    assert(x < tex->w);
    assert(y < tex->h);
    memcpy(out_texel, tex->texels + x + y * tex->w, sizeof(m7_texel_t));
}

void m7_texture_pack(m7_texture_t* tex,
                     short x,
                     short y,
                     const m7_texel_t* in_texel)
{
    assert(x >= 0);
    assert(y >= 0);
    assert(x < tex->w);
    assert(y < tex->h);
    memcpy(tex->texels + x + y * tex->w, in_texel, sizeof(m7_texel_t));
}

static void _m7_color_buffer_clear(m7_color_buffer_t* buffer)
{
    memset(buffer->pixels, 0, buffer->w * buffer->h * sizeof(m7_pixel_t));
}

static void _m7_depth_buffer_clear(m7_depth_buffer_t* buffer)
{
    memset(buffer->dexels, 255, buffer->w * buffer->h * sizeof(m7_dexel_t));
}

void m7_context_init(m7_context_t* ctx,
                     short w,
                     short h)
{
    int i;
    for (i = 0; i < M7_MAX_TEXTURES; i ++)
    {
        ctx->textures[i].free = 1;
        ctx->textures[i].texture_id = i;
    }
    
    for (i = 0; i < M7_MAX_BACKGROUNDS; i ++)
    {
        ctx->backgrounds[i].free = 1;
        ctx->backgrounds[i].texture_id = -1;
    }
    
    for (i = 0; i < M7_MAX_SPRITES; i ++)
    {
        ctx->sprites[i].free = 1;
        ctx->sprites[i].texture_id = -1;
    }
    
    ctx->cam.pos[0] = 0;
    ctx->cam.pos[1] = 0;
    ctx->cam.pos[2] = 0;
    ctx->cam.angle = 0;
    
    ctx->color_buffer.w = w;
    ctx->color_buffer.h = h;
    
    ctx->depth_buffer.w = w;
    ctx->depth_buffer.h = h;
    
    ctx->color_buffer.pixels = malloc(w * h * sizeof(m7_pixel_t));
    ctx->depth_buffer.dexels = malloc(w * h * sizeof(m7_dexel_t));
}

void m7_context_shutdown(m7_context_t* ctx)
{
    
}

static void _m7_cacl_horizon(m7_context_t* ctx)
{
    ctx->horizon = 0;
}

void m7_context_render(m7_context_t* ctx)
{
    _m7_color_buffer_clear(&ctx->color_buffer);
    _m7_depth_buffer_clear(&ctx->depth_buffer);
    _m7_cacl_horizon(ctx);
    
    const m7_cam_t* cam = &ctx->cam;
    
    const float angle = cam->angle_rad;
    const float cx = cam->pos[X];
    const float cy = cam->pos[Y];
    
    int bw = ctx->color_buffer.w;
    int bh = ctx->color_buffer.h;
    
    int x;
    int y;
    float fov = 300.0f;
    
    float px;
    float py;
    float pz;
    
    int scaling = 60;
    
    float tx;
    float ty;
    
    float sx;
    float sy;
    
    for (y = -bh / 2 ; y < ctx->horizon; y++)
    {
        for (x = -bw / 2 ; x < bw / 2 ; x++)
        {
            px = x;
            py = y - ctx->horizon - (fov);
            pz = y - ctx->horizon;
            
            tx = (px / pz);
            ty = (py / pz);
            
            sx = tx * cosf(angle) - ty * sinf(angle);
            sy = tx * sinf(angle) + ty * cosf(angle);
            
            sx = (unsigned int)(sx * scaling + cx);
            sy = (unsigned int)(sy * scaling + cy);
            
            m7_background_t* bg = &ctx->backgrounds[0];
            m7_texture_t* bg_tex = &ctx->textures[bg->texture_id];
            
            int rx = x + bw / 2;
            int ry = bh - ((y + bh / 2) + 1);
            

            if (sx >= 0 && sx < bg_tex->w &&
                sy >= 0 && sy < bg_tex->h)
            {
                if (pz < ctx->depth_buffer.dexels[rx + ry * bw])
                {
                    m7_texel_t texel;
                    m7_texture_unpack(bg_tex, sx, sy, &texel);
                    
                    texel.r *= fabs(pz) / 255.0f;
                    texel.g *= fabs(pz) / 255.0f;
                    texel.b *= fabs(pz) / 255.0f;
                    
                    ctx->color_buffer.pixels[rx + ry * bw].r = texel.r;
                    ctx->color_buffer.pixels[rx + ry * bw].g = texel.g;
                    ctx->color_buffer.pixels[rx + ry * bw].b = texel.b;
                    ctx->color_buffer.pixels[rx + ry * bw].a = texel.a;

                    ctx->depth_buffer.dexels[rx + ry * bw] = pz;
                }
            }
        }
    }
    
    /* Debug texture in corder
    m7_sprite_t* sprite = &ctx->sprites[0];
    m7_texture_t* sprite_tex = &ctx->textures[sprite->texture_id];
    
    for (y = 0; y < sprite->tw; y ++)
    {
        for (x = 0; x < sprite->th; x ++)
        {
            m7_texel_t texel;
            m7_texture_unpack(sprite_tex, sprite->tx + x, sprite->ty + y, &texel);
            
            ctx->color_buffer.pixels[x + y * bw].r = texel.r;
            ctx->color_buffer.pixels[x + y * bw].g = texel.g;
            ctx->color_buffer.pixels[x + y * bw].b = texel.b;
        }
    }
     */
}

m7_texture_t* m7_context_create_texture(m7_context_t* ctx, short w, short h)
{
    int i;
    for (i = 0; i < M7_MAX_TEXTURES; i ++)
    {
        if (ctx->textures[i].free)
        {
            ctx->textures[i].free = 0;
            _m7_texture_build(&ctx->textures[i], w, h);
            return &ctx->textures[i];
        }
    }
    
    return 0;
}

m7_background_t* m7_context_create_background(m7_context_t* ctx, int texture_id)
{
    int i;
    for (i = 0; i < M7_MAX_BACKGROUNDS; i ++)
    {
        if (ctx->backgrounds[i].free)
        {
            ctx->backgrounds[i].free = 0;
            ctx->backgrounds[i].texture_id = texture_id;
            return &ctx->backgrounds[i];
        }
    }
    
    return 0;
}

m7_sprite_t* m7_context_create_sprite(m7_context_t* ctx,
                                      int texture_id)
{
    int i;
    for (i = 0; i < M7_MAX_SPRITES; i ++)
    {
        if (ctx->sprites[i].free)
        {
            ctx->sprites[i].free = 0;
            
            ctx->sprites[i].texture_id = texture_id;
            return &ctx->sprites[i];
        }
    }
    
    return 0;
}

