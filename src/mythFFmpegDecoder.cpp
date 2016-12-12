#include "mythFFmpegDecoder.hh"
#include "mythLog.hh"
#include <chrono>
#ifdef USEPIPELINE
#include <memory.h>
extern "C"{
#include "libavformat/avformat.h"
#include "libavutil/mathematics.h"
#include "libavutil/time.h"
}

mythFFmpegDecoder::mythFFmpegDecoder(const char* filename)
	:mythVirtualDecoder()
{
	char tmp[256] = { 0 };
	int len = strlen(filename);
	memcpy(tmp, &filename[7], len - 7);
	_filename = tmp;
}


int mythFFmpegDecoder::MainLoop()
{
	AVFormatContext *ifmt_ctx = NULL;
	AVPacket pkt;
	int ret, i;
	int videoindex = -1;

	av_register_all();

	int frame_index = 0;
	while (flag == 0){
		if ((ret = avformat_open_input(&ifmt_ctx, _filename.c_str(), 0, 0)) < 0) {
			mythLog::GetInstance()->printf("Could not open input file:%s\n",_filename.c_str());
			return -1;
		}
		if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
			printf("Failed to retrieve input stream information");
			return -1;
		}

		videoindex = -1;
		for (i = 0; i < ifmt_ctx->nb_streams; i++) {
			if (ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
				videoindex = i;
				break;
			}
		}

		av_dump_format(ifmt_ctx, 0, _filename.c_str(), 0);

		/*
		FIX: H.264 in some container format (FLV, MP4, MKV etc.) need
		"h264_mp4toannexb" bitstream filter (BSF)
		*Add SPS,PPS in front of IDR frame
		*Add start code ("0,0,0,1") in front of NALU
		H.264 in some container (MPEG2TS) don't need this BSF.
		*/
		AVBitStreamFilterContext* h264bsfc = av_bitstream_filter_init("h264_mp4toannexb");
		auto start_time = av_gettime();
		while (av_read_frame(ifmt_ctx, &pkt) >= 0){
			if (pkt.stream_index == videoindex){
				av_bitstream_filter_filter(h264bsfc, ifmt_ctx->streams[videoindex]->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);
				if (pkt.pts == AV_NOPTS_VALUE){
					//Write PTS
					AVRational time_base1 = ifmt_ctx->streams[videoindex]->time_base;
					//Duration between 2 frames (us)
					int64_t calc_duration = (double) AV_TIME_BASE / av_q2d(ifmt_ctx->streams[videoindex]->r_frame_rate);
					//Parameters
					pkt.pts = (double) (frame_index*calc_duration) / (double) (av_q2d(time_base1)*AV_TIME_BASE);
					pkt.dts = pkt.pts;
					pkt.duration = (double) calc_duration / (double) (av_q2d(time_base1)*AV_TIME_BASE);
				}

				//Important:Delay
				if (pkt.stream_index == videoindex){
					AVRational time_base = ifmt_ctx->streams[videoindex]->time_base;
					AVRational time_base_q = { 1, AV_TIME_BASE };
					int64_t pts_time = av_rescale_q(pkt.dts, time_base, time_base_q);
					int64_t now_time = av_gettime() - start_time;
					if (pts_time > now_time)
						av_usleep(pts_time - now_time);

				}
				//av_usleep(400);
				auto in_stream = ifmt_ctx->streams[pkt.stream_index];
				auto out_stream = ifmt_ctx->streams[pkt.stream_index];
				/* copy packet */
				//Convert PTS/DTS
				pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
				pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
				pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
				pkt.pos = -1;
				//Print to Screen
				//printf("Send %8d video frames to output URL\n", frame_index);
				frame_index++;
				put(pkt.data, pkt.size, pkt.pts);
			}
			av_free_packet(&pkt);
		}

		av_bitstream_filter_close(h264bsfc);

		avformat_close_input(&ifmt_ctx);
	}
	return 0;
}

mythFFmpegDecoder::~mythFFmpegDecoder()
{
}
#endif