#include "SDL2Display.h"

#include "LogUtil.h"



SDL2Display::SDL2Display(const bool b_fix_size):
	b_fix_size_(b_fix_size),
	img_convert_ctx_(NULL),
	scale_frame_(NULL)
{
}


SDL2Display::~SDL2Display()
{
	if (vid_texture)
		display_deinit();
}

/**
 * default_width 提供缺省的宽， 如果为0则系统默认为
 * default_height 提供缺省的高
 */
int SDL2Display::display_init(const int default_width, const int default_height)
{
	if (default_width != 0)
	{
		width_ = default_width;
		height_ = default_height;
	}
	window = SDL_CreateWindow("Simple Player",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width_,
		height_,
		SDL_WINDOW_RESIZABLE);  //SDL_WINDOW_OPENGL
	if (!window)
	{
		LogError("SDL: could not create window - %s\n",
			SDL_GetError());
		return -1;
	}

	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer)
	{
		LogError("SDL_CreateRenderer failed");
		return -1;
	}
	return 0;
}

void SDL2Display::display_deinit(void)
{
	if (vid_texture)
	{
		SDL_DestroyTexture(vid_texture);
		vid_texture = NULL;
	}
	if (renderer)
	{
		SDL_DestroyRenderer(renderer);
		renderer = NULL;
	}
	if (window)
	{
		SDL_DestroyWindow(window);
		window = NULL;
	}

	if (scale_frame_)
	{
		av_freep(&scale_frame_->data[0]);
		av_frame_free(&scale_frame_);
	}
	if (img_convert_ctx_)
	{
		sws_freeContext(img_convert_ctx_);
		img_convert_ctx_ = NULL;
	}
}

int SDL2Display::display_open(void)
{
	//SDL_SetWindowTitle(window, filename);
	SDL_SetWindowSize(window, width_, height_);
	SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_ShowWindow(window);

	vid_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_IYUV,	// 设定为YUV输出
		SDL_TEXTUREACCESS_STREAMING, width_, height_);
	if (!vid_texture)
	{
		LogError("SDL_CreateTexture failed");
		return -1;
	}

	sdlRect.x = 0;
	sdlRect.y = 0;
	sdlRect.w = width_;
	sdlRect.h = height_;

	return 0;
}

void SDL2Display::display_frame(AVFrame * frame)
{
	int ret = 0;
	AVFrame *p_display_frame = frame;
	// 使用固定分辨率时，如果传入的尺寸和设定的不统一则做强换
	if (b_fix_size_ && frame->width!= width_ && frame->height!= height_)
	{
		if (!img_convert_ctx_)
		{
			scale_frame_ = av_frame_alloc();
			if (!scale_frame_)
			{
				LogError("ffmpeg get context error!");
				return;
			}
			scale_frame_->format = frame->format;
			scale_frame_->width = width_;
			scale_frame_->height = height_;
			// 分配空间
			if (av_image_alloc(scale_frame_->data, scale_frame_->linesize, 
				scale_frame_->width, scale_frame_->height,
				(enum AVPixelFormat)scale_frame_->format, 32) < 0) 
			{
				LogError( "Could not allocate raw picture buffer");
				return;
			}
	
			img_convert_ctx_ = sws_getContext(frame->width, frame->height,(enum AVPixelFormat)frame->format,
				scale_frame_->width, scale_frame_->height, (enum AVPixelFormat)scale_frame_->format,
				SWS_BICUBIC,
				NULL,
				NULL,
				NULL
			);
			if (!img_convert_ctx_)
			{
				LogError("ffmpeg get context error!");
				return;
			}
			
		}
		ret = sws_scale(img_convert_ctx_, frame->data, frame->linesize
			, 0, frame->height, scale_frame_->data, scale_frame_->linesize);
		p_display_frame = scale_frame_;
	}
	SDL_UpdateYUVTexture(vid_texture, &sdlRect,
		p_display_frame->data[0], p_display_frame->linesize[0],
		p_display_frame->data[1], p_display_frame->linesize[1],
		p_display_frame->data[2], p_display_frame->linesize[2]);


	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, vid_texture, NULL, &sdlRect);
	SDL_RenderPresent(renderer);
}
