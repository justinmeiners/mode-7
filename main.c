//
//  main.c
//  Doom
//
//  Created by Justin Meiners on 8/12/13.
//  Copyright (c) 2013 Inline Studios. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "targa.h"
#include "vec_math.h"

#include <SDL2/SDL.h>
#include "mode_7.h"

int keys[6];

void Render(m7_context_t* ctx)
{
    m7_cam_t* cam = &ctx->cam;
    
    if (keys[2])
    {
        cam->angle -= 2;
    }
    
    if (keys[3])
    {
        cam->angle += 2;
    }
    
    cam->angle_rad = (cam->angle * M_PI) / 180.0f;
    
    const float angle = cam->angle_rad;
    
    float dir_x = 2 * cosf(angle);
    float dir_y = 2 * -sinf(angle);
    
    if (keys[0])
    {
        cam->pos[X] += dir_y;
        cam->pos[Y] += dir_x;
    }
    
    if (keys[1])
    {
        cam->pos[X] -= dir_y;
        cam->pos[Y] -= dir_x;
    }
    
    if (keys[4])
    {
        cam->pos[Z] += 2;
    }
    
    if (keys[5])
    {
        cam->pos[Z] -= 2;
    }
    
    m7_context_render(ctx);
}


int main(int argc, const char * argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == -1)
    {
        return 1;
    }
    
    short vw = 640;
    short vh = 480;
    
    SDL_Window* window = SDL_CreateWindow("Mode 7",
                                          0,
                                          0,
                                          vw,
                                          vh,
                                          SDL_WINDOW_SHOWN);
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    
    
    SDL_Texture* tex = SDL_CreateTexture(renderer,
                                         SDL_PIXELFORMAT_ARGB8888,
                                         SDL_TEXTUREACCESS_STREAMING,
                                         vw, vh);
    m7_context_t ctx;
    m7_context_init(&ctx, vw, vh);
    
    tga_image image;
    tga_read(&image, "data/tex.tga");
    
    m7_texture_t* bg_tex = m7_context_create_texture(&ctx, image.width, image.height);

    memcpy(bg_tex->texels, image.image_data, 4 * image.width * image.height);

    m7_context_create_background(&ctx, bg_tex->texture_id);
    
    tga_image image2;
    tga_read(&image2, "data/tex.tga");
    tga_swap_red_blue(&image2);
    
    m7_texture_t* dk_tex = m7_context_create_texture(&ctx, image2.width, image2.height);
    memcpy(dk_tex->texels, image2.image_data, 4 * image2.width * image2.height);

    m7_sprite_t* sprite = m7_context_create_sprite(&ctx, dk_tex->texture_id);
    sprite->tx = 0;
    sprite->ty = 0;
    sprite->tw = 128;
    sprite->th = 128;
    
    keys[0] = 0;
    keys[1] = 0;
    keys[2] = 0;
    keys[3] = 0;
    keys[4] = 0;
    keys[5] = 0;
    
    while (1)
    {
        SDL_Event e;
        if (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                break;
            }
            else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
            {
                int st = (e.type == SDL_KEYDOWN);
                
                switch (e.key.keysym.sym)
                {
                    case SDLK_UP:
                        keys[0] = st;
                        break;
                    case SDLK_DOWN:
                        keys[1] = st;
                        break;
                    case SDLK_LEFT:
                        keys[2] = st;
                        break;
                    case SDLK_RIGHT:
                        keys[3] = st;
                        break;
                    case SDLK_w:
                        keys[4] = st;
                        break;
                    case SDLK_s:
                        keys[5] = st;
                        break;
                    default:
                        break;
                }
            }
        }
        
        
        SDL_RenderClear(renderer);
        
        void* pixels;
        uint32_t* dst;
        int src;
        int pitch;
        
        SDL_LockTexture(tex, NULL,
                        &pixels,
                        &pitch);
        
        Render(&ctx);
        
        int row;
        int col;
        src = 0;
        
        for (row = 0; row < vh; ++row)
        {
            dst = (uint32_t*)((uint8_t*)pixels + row * pitch);
            
            for (col = 0; col < vw; ++col)
            {
                const m7_pixel_t* color = &ctx.color_buffer.pixels[src];
                *dst++ = (0xFF000000|(color->r<<16)|(color->g<<8)|color->b);
                src ++;
            }
        }
        
        
        SDL_UnlockTexture(tex);
        SDL_RenderCopy(renderer, tex, NULL, NULL);
        SDL_RenderPresent(renderer);
    }
    
    SDL_Quit();
    
    return 0;
}

