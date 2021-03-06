/*
* Date 2017/4/12
* Author  li.wu
* Describe http-flv 方式直播请求处理模块，主要负责接收对外的http请求，并进行
* http请求保持，请求管理，数据分发等功能，实现自定义的http直播业务模块
*/

#ifndef NGX_HTTP_LIVE_PLAY_MODULE_H
#define NGX_HTTP_LIVE_PLAY_MODULE_H
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include "ngx_rtmp.h"
#include "ngx_http_live_play_relay_module.h"

#define HTTP_FLV_META_TAG 0
#define HTTP_FLV_AVC_TAG 1
#define HTTP_FLV_AAC_TAG 2
#define HTTP_FLV_AUDIO_TAG 3

#define HTTP_FLV_VIDEO_TAG 4
#define HTTP_FLV_VIDEO_KEY_FRAME_TAG 5

#define NGX_STREAM_REWART   888888

typedef struct ngx_str_map_list_s ngx_str_map_list_t;
typedef struct ngx_http_flv_frame_s ngx_http_flv_frame_t;

extern ngx_module_t        ngx_http_live_play_module;

typedef struct {
    ngx_str_t        server;
}ngx_http_live_play_srv_conf_t;

typedef struct {
	ngx_flag_t http_live_on;
    ngx_flag_t http_domain_on;
    ngx_flag_t live_md5_check_on;
    ngx_str_t  live_md5_key;

    ngx_msec_t http_send_timeout;
    ngx_msec_t http_send_header_timeout;

    ngx_uint_t  http_send_chunk_size; //每次发送包的大小
    ngx_uint_t  http_send_max_chunk_count; //每次最多发生包的个数

    ngx_str_t  http_live_app;
    ngx_msec_t http_idle_timeout;

    ngx_flag_t http_play_cache_on;  //播放缓存开启标记
    ngx_uint_t http_play_cahce_frame_num; //播放最大缓存的帧大小
    ngx_msec_t http_play_cahce_time_duration; //播放最大缓存的时间长度

    ngx_uint_t cut_play_before_drop_num;  //出现丢包几次后关闭掉链接
}ngx_http_live_play_loc_conf_t;

typedef struct {
    ngx_str_t key;
    ngx_str_t value;
}ngx_str_map_node_t;

struct ngx_str_map_list_s {
    ngx_str_map_node_t *node; //当前值
    ngx_str_map_list_t* next; //next指针
};

struct ngx_http_flv_frame_s {
    u_char mtype;  //类型
    unsigned int mpts;//时间戳
    unsigned int mdelte;//间隔
    unsigned int mlen;//长度
    ngx_chain_t * out; //数据
    ngx_http_flv_frame_t *next;//下一帧数据
};

typedef struct {
    ngx_str_t                        stream;
    ngx_str_t                        app;
    ngx_str_t                        suffix;
    ngx_str_t                        md5;
    ngx_str_t                        domain;
    ngx_str_map_list_t              *param_list_head; //参数头
    ngx_str_map_list_t              *param_list_tail; //参数尾
    ngx_str_t                        md5_public_key;   //MD5 签名key
    ngx_uint_t                       current_send_count;

    ngx_event_t                      send_header_timeout_ev; //发生header的检测定时器
    ngx_int_t                        send_header_ev_count;
    ngx_int_t                        send_header_flag; //表示是否发生过http header

    ngx_event_t                      close;
    ngx_event_t                      idle_evt;


    ngx_http_request_t              *s; 
    ngx_http_live_play_relay_ctx_t  *relay_ctx;
    uint32_t                         current_time;
    void                            *hr_ctx;
    
    ngx_chain_t                     *header_chain; //头

    ngx_http_flv_frame_t            *frame_chain_head; //内容数据，发送就从次链表拿数据发送
    ngx_http_flv_frame_t            *frame_chain_tail; //内容数据，发送就从次链表拿数据发送

    ngx_http_flv_frame_t            *frame_free;
    ngx_int_t                       drop_count;
    ngx_msec_t                      cache_time_duration; //当前缓存时间长度
    ngx_uint_t                      cache_frame_num; //当前缓存的视频帧数
    ngx_int_t                       cache_droping;   //丢帧标记
    ngx_uint_t                      cache_max_duration; //最大缓存时间

    ngx_int_t                       drop_vframe_num;  //丢弃的视频帧数
    ngx_int_t                       drop_vduration;   //丢帧视频的时长
    ngx_uint_t                      drop_video_size; 
    ngx_uint_t                      drop_audio_size; 
    
    // 日志相关
    u_char                           uuid[32];

    ngx_int_t                        log_type;      // 0:关闭, 1:打开,(由0->1 start，1 status )
    ngx_uint_t                       log_lts; 
    ngx_uint_t                       request_ts;    // 请求道来时间 毫秒
    ngx_uint_t                       current_ts;    // 每次数据发送时间 
    ngx_str_t                        client_ip; 
    ngx_str_t                        server_ip;
    char                             client_isp_name[1024];
    ngx_str_t                        host;
    ngx_str_t                        pull_url;
    
    ngx_uint_t                       stream_ts;
    ngx_uint_t                       recv_video_size;
    ngx_uint_t                       recv_audio_size;
    ngx_uint_t                       recv_video_frame; 
    ngx_uint_t                       lrecv_video_size;
    ngx_uint_t                       lrecv_audio_size;
    ngx_uint_t                       lrecv_video_frame; 

    ngx_int_t                       audio_pts;//音频时间戳
    ngx_int_t                       video_pts;//视频时间戳  

    ngx_int_t                       first_tag;
    ngx_int_t                       system_first_pts; //收到第一帧数据的系统时间
    ngx_int_t                       data_first_pts; //收到第一帧的数据时间 两个时间都采用视频做参考
    
    ngx_flag_t                       start_caton;    // 开始卡顿
    ngx_uint_t                       dropVideoFrame;
    ngx_uint_t                       cacheVideoFrame; 
    ngx_int_t                        status_code; // 关闭时与request结构体同步
} ngx_http_live_play_request_ctx_t;

typedef struct {
	char       *rs;
	ngx_int_t   ret;
} ngx_http_live_play_header_struct_t;


static const ngx_http_live_play_header_struct_t  ngx_http_live_play_status[] =
{
	{"200 OK",NGX_HTTP_OK},
    {"302 Moved Temporarily",NGX_HTTP_MOVED_TEMPORARILY},
	{"403 Forbidden",NGX_HTTP_FORBIDDEN},
	{"404 Not Found",NGX_HTTP_NOT_FOUND}
};

typedef enum {
	HTTP_STATUS_200 = 0,
    HTTP_STATUS_302 = 1,
	HTTP_STATUS_403 = 2,
	HTTP_STATUS_404 = 3
} ngx_http_respond_henader_status;

void 
ngx_str_format_string(ngx_str_t str, char *buf);

ngx_int_t 
ngx_http_live_send_message(ngx_http_live_play_request_ctx_t *s, ngx_chain_t *out, u_char mtype, unsigned int mlen, unsigned int pts, unsigned int delta);

ngx_int_t  
ngx_http_live_play_send_http_header(void *ptr);

void 
ngx_http_live_play_close(void * v);

#endif
