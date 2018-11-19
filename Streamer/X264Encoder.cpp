#include "X264Encoder.h"
#include <iostream>
#include <stdlib.h>
#include <string> 
#include "LogUtil.h"
#define X264_ALLOC_PIC_IN 0

void X264Encoder::encodeNALs(MediaPacket *pkt, const x264_nal_t *nals, int nnal)
{
	uint8_t *p;
	int i;
	int size = 0;

	p = pkt->data;

	for (i = 0; i < nnal; i++)
		size += nals[i].i_payload;
	pkt->size = size;
	for (i = 0; i < nnal; i++) {
		memcpy(p, nals[i].p_payload, nals[i].i_payload);
		p += nals[i].i_payload;
	}
}

X264Encoder::X264Encoder()
{
}

X264Encoder::~X264Encoder()
{
}

int X264Encoder::Init(VideoEncoderConfig &encoder_config)
{
	// 限制最小值
	if (encoder_config.gop < 5)
	{
		encoder_config.gop = 10;
	}
	if (encoder_config.bitrate < 100)
	{
		encoder_config.bitrate = 1000;
	}
	if (encoder_config.qp <= 0 || encoder_config.qp > 51)
	{
		encoder_config.qp = 26;
	}
	if (encoder_config.fps < 10)
	{
		encoder_config.fps = 10;
	}
	if (encoder_config.slice_max_size < 1000)
	{
		encoder_config.slice_max_size = 1000;
	}
	// 配置宽高，计算yuv数据大小
	width_ = encoder_config.width;
	height_ = encoder_config.height;
	luma_size_ = width_ * height_;	// y分量数据大小
	chroma_size_ = width_ * height_ / 4;// u或v分量数据大小
																						// 先使用缺省参数初始化x264_params参数
	x264_param_default(&x264_params_);

	// 如果是直播编码，一定要"zerolatency"零延迟
	// 在选择preset时需要考虑编码效率和图像质量的折衷，级别越高编码效率越低，但图像质量也高
	x264_param_default_preset(&x264_params_,
		X264Encoder::GetPresetString(encoder_config.preset), "zerolatency");	// 默认无延迟

																		// 选择profile，如果使用了H264_PROFILE_AUTO，则保留preset的设置
	if (encoder_config.profile != H264_PROFILE_AUTO)
	{
		x264_param_apply_profile(&x264_params_,
			X264Encoder::GetProfileString(encoder_config.profile));
	}

	// 设置分辨率
	x264_params_.i_width = width_;
	x264_params_.i_height = height_;
	// 默认情况下已为X264_CSP_I420，在这个demo程序也只支持YUV420P（即是X264_CSP_I420）
	//x264_params_.i_csp = X264_CSP_I420;

	x264_params_.i_log_level = X264_LOG_NONE;		// X264 LOG级别
	x264_params_.i_fps_num = encoder_config.fps;	// 帧率分子
	x264_params_.i_fps_den = 1;						// 帧率分母
	x264_params_.i_keyint_min = X264_KEYINT_MIN_AUTO;	// 最小I帧间隔自动
	x264_params_.i_keyint_max = encoder_config.gop;	// 最大I帧间隔

	x264_params_.b_annexb = encoder_config.b_annexb;		    // 如果设置为0则没有startcode
	x264_params_.b_repeat_headers = encoder_config.b_repeat_headers;	// =1时重复SPS/PPS 放到关键帧前面，=0则不会 	   

	x264_params_.i_timebase_num = x264_params_.i_fps_den;
	x264_params_.i_timebase_den = x264_params_.i_fps_num;

	if (encoder_config.use_qp == 1)
	{
		if (encoder_config.qp>0 && encoder_config.qp <= 51)
		{
			x264_params_.rc.i_rc_method = X264_RC_CQP;//恒定质量	
			x264_params_.rc.i_qp_constant = encoder_config.qp;
		}
	}
	else
	{
		if (encoder_config.bitrate>0)
		{
			if (encoder_config.use_qp == 0)
			{
				x264_params_.rc.i_rc_method = X264_RC_ABR;////平均码率
				x264_params_.rc.i_bitrate = encoder_config.bitrate*0.65;//单位kbps
			 //	x264_params_.rc.i_vbv_buffer_size = encoder_config.bitrate * 1.1;
			 	x264_params_.rc.i_vbv_max_bitrate = encoder_config.bitrate;
			}
			else if (encoder_config.use_qp == 2)
			{
				x264_params_.rc.i_rc_method = X264_RC_CRF;// 恒定码率
				x264_params_.rc.i_bitrate = encoder_config.bitrate *0.7;//单位kbps
				//x264_params_.rc.i_vbv_buffer_size = encoder_config.bitrate;
				x264_params_.rc.i_vbv_max_bitrate = encoder_config.bitrate;
				
			}
		}
	}
	if (encoder_config.slice_max_size > 0)	// 否则使用默认的
		x264_params_.i_slice_max_size = encoder_config.slice_max_size;
	x264_params_.i_threads = 1;			//
	x264_params_.i_slice_count = 1;		// i_threads为1的时候才生效
	x264_params_.i_slice_count_max = 1;
	x264_params_.rc.b_mb_tree = 0;		//实时编码为0
	if (encoder_config.bframes >= 0)	// 如果encoder_config.bframes <0，则说明直接使用preset的设置
		x264_params_.i_bframe = encoder_config.bframes;

	x264_handle_ = x264_encoder_open(&x264_params_);
	if (!x264_handle_)
	{
		printf("x264_encoder_open failed\n");
		return -1;
	}

 	UpdateEncoderInfo();

#if X264_ALLOC_PIC_IN
	// 如果不x264_picture_alloc则也不用x264_picture_clean
	x264_picture_alloc(&pic_in_, X264_CSP_I420,
		width_, height_);
#else
	// 这里要做初始化，但不会像x264_picture_alloc一样分配内存，
	// 如果使用x264_picture_alloc分配内存，则在编码的时候需要先进行拷贝
	memset(&pic_in_, 0, sizeof(x264_picture_t));
	pic_in_.img.i_plane = 3;		// YUV 3个plane
	pic_in_.img.i_csp = X264_CSP_I420; // 固定只支持YUV420P格式
	pic_in_.img.i_stride[0] = width_;	// 宽
	pic_in_.img.i_stride[1] = width_ / 2;	// 宽度一半 由YUV420格式决定的
	pic_in_.img.i_stride[2] = width_ / 2; // 宽度一半
#endif
	return 0;
}

void X264Encoder::Deinit()
{
	if(x264_handle_)
	{ 
		// 关闭编码器
		x264_encoder_close(x264_handle_);
		x264_handle_ = NULL;
	#if X264_ALLOC_PIC_IN
		//释放 x264_picture_t allocated的内存
		//如果不x264_picture_alloc则也不用x264_picture_clean
		x264_picture_clean(&pic_in_);
	#endif
	}
}

int X264Encoder::EncodeFrame(MediaPacket * pkt, const MediaFrame * frame)
{
	x264_nal_t *nal;
	int nnal, i, ret;
	x264_picture_t pic_out = { 0 };
	int pict_type;
	x264_picture_init(&pic_out);

	if (frame)
	{
#if  X264_ALLOC_PIC_IN		// 这种模式多一次拷贝内存
		memcpy(pic_in_.img.plane[0], frame->data[0], x4->luma_size);
		memcpy(pic_in_.img.plane[1], frame->data[1], x4->chroma_size);
		memcpy(pic_in_.img.plane[2], frame->data[2], x4->chroma_size);
#else
		for (i = 0; i < pic_in_.img.i_plane; i++)
		{
			pic_in_.img.plane[i] = frame->data[i];
		}
		
#endif

		pic_in_.i_pts = frame->pts;

		switch (frame->pict_type) {
		case MD_PICTURE_TYPE_I:
			pic_in_.i_type = X264_TYPE_KEYFRAME;
			break;
		case MD_PICTURE_TYPE_IDR:
			pic_in_.i_type = X264_TYPE_IDR;
			break;
		case MD_PICTURE_TYPE_P:
			pic_in_.i_type = X264_TYPE_P;
			break;
		case MD_PICTURE_TYPE_B:
			pic_in_.i_type = X264_TYPE_B;
			break;
		default:
			pic_in_.i_type = X264_TYPE_AUTO;
			break;
		}
	}
	
	if (!frame)
	{
		int num = x264_encoder_delayed_frames(x264_handle_);
		printf("delayed_frames = %d\n", num);	//这里只是为debug
		if (num <= 0)
		{
			return H264_ENCODE_EOF;		// 已经解码完毕
		}
	}
	// >0 返回nal个数
	// =0 没有nal返回
	// -1 返回错误
	if ((ret = x264_encoder_encode(x264_handle_, &nal, &nnal, frame ? &pic_in_ : NULL, &pic_out)) < 0)
		return H264_ENCODE_ERROR;

	if (0 == ret)
	{
		return H264_ENCODE_EAGAIN;			// 继续扔frame进来
	}

	encodeNALs(pkt, nal, nnal);		// 读取packet

	pkt->pts = pic_out.i_pts;
	pkt->dts = pic_out.i_dts;

	switch (pic_out.i_type) {
	case X264_TYPE_IDR:
		pict_type = MD_PICTURE_TYPE_IDR;
		break;
	case X264_TYPE_I:
		pict_type = MD_PICTURE_TYPE_I;
		break;
	case X264_TYPE_P:
		pict_type = MD_PICTURE_TYPE_P;
		break;
	case X264_TYPE_B:
	case X264_TYPE_BREF:
		pict_type = MD_PICTURE_TYPE_B;
		break;
	default:
		pict_type = MD_PICTURE_TYPE_NONE;
	}
	pkt->pkt_type = pict_type;		// 目前pkt_type只是为了调试用

	return H264_ENCODE_GOT_PACKET;
}

int X264Encoder::UpdateEncoderInfo()
{
	if (!x264_handle_)
	{
		LogError("x264_handle_ is null\n");
		return -1;
	}

	x264_nal_t *p_nal;
	int i_nal;

	if (x264_encoder_headers(x264_handle_, &p_nal, &i_nal) < 0)
	{
		LogError("x264_encoder_headers failed");
		return -1;
	}

	for (int i = 0; i < i_nal; ++i)
	{
		switch (p_nal[i].i_type)
		{
		case NAL_SPS: //0x0696F040 + 28
			sps_.clear();
			sps_.append(p_nal[i].p_payload, p_nal[i].p_payload + p_nal[i].i_payload);
			LogDebug("sps size = %d", sps_.size());
			break;
		case NAL_PPS: //
			pps_.clear();
			pps_.append(p_nal[i].p_payload, p_nal[i].p_payload + p_nal[i].i_payload);
			LogDebug("pps size = %d", pps_.size());
			break;
		case NAL_SEI:
			sei_.clear();
			sei_.append(p_nal[i].p_payload, p_nal[i].p_payload + p_nal[i].i_payload);
			LogDebug("sei size = %d", sei_.size());
			break;
		default:
			break;
		}
	}

	return 0;
}


int X264Encoder::GetSPS(char * sps_buf, int & sps_len)
{
	if (sps_len < sps_.size())
	{
		return -1;
	}
	memcpy(sps_buf, sps_.c_str(), sps_.size());

	return 0;
}

int X264Encoder::GetSPS(std::string & sps)
{
	sps = sps_;
	return 0;
}

int X264Encoder::GetPPS(char * pps_buf, int & pps_len)
{
	if (pps_len < pps_.size())
	{
		return -1;
	}
	memcpy(pps_buf, pps_.c_str(), pps_.size());

	return 0;
}

int X264Encoder::GetPPS(std::string & pps)
{
	pps = pps_;
	return 0;
}

int X264Encoder::GetSEI(char * sei_buf, int & sei_len)
{
	if (sei_len < pps_.size())
	{
		return -1;
	}
	memcpy(sei_buf, sei_.c_str(), sei_.size());
	return 0;
}

int X264Encoder::GetSEI(std::string & sei)
{
	sei = sei_;
	return 0;
}

int X264Encoder::SetParam(const char * name, const char * value)
{
	if (!x264_handle_) 
		return x264_param_parse(&x264_params_, name, value);

	x264_encoder_parameters(x264_handle_, &x264_params_);
	if (x264_param_parse(&x264_params_, name, value) < 0)
		return -1;
	if (x264_encoder_reconfig(x264_handle_, &x264_params_) < 0)
		return -1;

	return 0;
}

int X264Encoder::SetBitrate(int bitrate)
{
	//return 0;
	x264_encoder_parameters(x264_handle_, &x264_params_);

	// x264_params_.rc.i_rc_method = X264_RC_CRF;////平均码率
	 x264_params_.rc.i_bitrate = bitrate*0.65;//单位kbps
	 x264_params_.rc.i_vbv_max_bitrate = bitrate;		// 调整该参数时 画面抖动马赛克
	// x264_params_.rc.i_vbv_buffer_size = bitrate;
	if (x264_encoder_reconfig(x264_handle_, &x264_params_) < 0)
	{
		LogError("x264_encoder_reconfig failed");
		return -1;
	}
	//x264_encoder_intra_refresh(x264_handle_);
	return 0;
}

const char * X264Encoder::GetPresetString(int preset)
{
	switch (preset)
	{
		case H264_PRESET_DEFAULT: return "ultrafast";
		case H264_PRESET_ULTRAFAST: return "ultrafast";
		case H264_PRESET_SUPERFAST: return "superfast";
		case H264_PRESET_VERYFAST: return "veryfast";
		case H264_PRESET_FASTER: return "faster";
		case H264_PRESET_FAST: return "fast";
		case H264_PRESET_MEDIUM: return "medium";
		case H264_PRESET_SLOW: return "slow";
		case H264_PRESET_SLOWER: return "slower";
		case H264_PRESET_VERYSLOW: return "veryslow";
		case H264_PRESET_PLACEBO: return "placebo";
		default: return "ultrafast";
	}
}

const char * X264Encoder::GetProfileString(int profile)
{
	switch (profile)
	{
		case H264_PROFILE_AUTO: return "auto profile";
		case H264_PROFILE_BASELINE: return "baseline";
		case H264_PROFILE_MAIN: return "main";
		case H264_PROFILE_HIGH: return "high";
		case H264_PROFILE_HIGH10: return "high10";
		case H264_PROFILE_HIGH422: return "high422";
		case H264_PROFILE_HIGH444_PREDICTIVE: return "high444";
		default: return "main";
	}
}

const char * X264Encoder::GetErrorString(int ret)
{
	switch (ret)
	{
		case H264_ENCODE_EOF: return "H264_ENCODE_EOF";
		case H264_ENCODE_EAGAIN: return "H264_ENCODE_EAGAIN";
		case H264_ENCODE_GOT_PACKET: return "H264_ENCODE_GOT_PACKET";
		case H264_ENCODE_ERROR: return "H264_ENCODE_ERROR";
		default: return "Unknown H264_ENCODE error";
	}
}
