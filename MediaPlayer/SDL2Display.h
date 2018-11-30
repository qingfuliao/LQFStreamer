#pragma once
// https://blog.csdn.net/robin912/article/details/79677997?utm_source=blogxgwz0
#ifdef __cplusplus
extern "C"
{
#include "SDL.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
}
#endif
class SDL2Display
{
	#define SDL2_DISPLAY_WIDTH   640		// 使用16:10
	#define SDL2_DISPLAY_HEIGHT   400
public:
	SDL2Display(const bool b_fix_size = true);
	~SDL2Display();
	int display_init(const int default_width = SDL2_DISPLAY_WIDTH, const int default_height = SDL2_DISPLAY_HEIGHT);
	void display_deinit(void);
	int display_open(void);
	void display_frame(AVFrame *frame);
private:
	SDL_Window   *window = NULL;
	SDL_Renderer *renderer = NULL;
	SDL_Texture  *vid_texture = NULL;
	SDL_Rect     sdlRect;
	int width_ = SDL2_DISPLAY_WIDTH;
	int height_= SDL2_DISPLAY_HEIGHT;
	bool b_fix_size_ = true;	// 固定分辨率输出，以便调试

	// 尺寸转换
	struct SwsContext *img_convert_ctx_ = NULL;	//
	AVFrame *scale_frame_ = NULL;
};

