
/*
 * Copyright (C) Roman Arutyunyan
 */


#ifndef _NGX_RTMP_LIVE_H_INCLUDED_
#define _NGX_RTMP_LIVE_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>
#include "ngx_rtmp.h"
#include "ngx_rtmp_cmd_module.h"
#include "ngx_rtmp_bandwidth.h"
#include "ngx_rtmp_streams.h"
#include "./http/ngx_media_data_cache.h"

typedef struct ngx_rtmp_live_ctx_s ngx_rtmp_live_ctx_t;
typedef struct ngx_rtmp_live_stream_s ngx_rtmp_live_stream_t;


typedef struct {
    unsigned                            active:1;
    uint32_t                            timestamp;
    uint32_t                            csid;
    uint32_t                            dropped;
} ngx_rtmp_live_chunk_stream_t;


struct ngx_rtmp_live_ctx_s {
    ngx_rtmp_session_t                 *session;
    ngx_rtmp_live_stream_t             *stream;
    ngx_rtmp_live_ctx_t                *next;
    ngx_uint_t                          ndropped;
    ngx_rtmp_live_chunk_stream_t        cs[2];
    ngx_uint_t                          meta_version;
    ngx_event_t                         idle_evt;
    unsigned                            active:1;
    unsigned                            publishing:1;
    unsigned                            silent:1;
    unsigned                            paused:1;

    ngx_media_data_cache_t              *media_cache;
    
    /*
    ngx_uint_t                          log_type;
    ngx_uint_t                          log_lts;    // 记录日志使用（5s记录一次）
    ngx_uint_t                          request_ts;
    ngx_str_t                           client_ip;
    ngx_str_t                           server_ip;
    ngx_str_t                           host;
    ngx_str_t                           pull_url;
    
    ngx_uint_t                          video_size;
    ngx_uint_t                          audio_size;
    ngx_uint_t                          send_frame; 
    
    ngx_uint_t                       dropVideoFrame;
    ngx_uint_t                       cacheVideoFrame; 
    */
};


struct ngx_rtmp_live_stream_s {
    u_char                              name[NGX_RTMP_MAX_NAME];
    ngx_rtmp_live_stream_t             *next;
    ngx_rtmp_live_ctx_t                *ctx;
    ngx_rtmp_bandwidth_t                bw_in;
    ngx_rtmp_bandwidth_t                bw_in_audio;
    ngx_rtmp_bandwidth_t                bw_in_video;
    ngx_rtmp_bandwidth_t                bw_out;
    ngx_msec_t                          epoch;
    unsigned                            active:1;
    unsigned                            publishing:1;
};


typedef struct {
    ngx_int_t                           nbuckets;
    ngx_rtmp_live_stream_t            **streams;
    ngx_flag_t                          live;
    ngx_flag_t                          meta;
    ngx_msec_t                          sync;
    ngx_msec_t                          idle_timeout;
    ngx_flag_t                          atc;
    ngx_flag_t                          interleave;
    ngx_flag_t                          wait_key;
    ngx_flag_t                          wait_video;
    ngx_flag_t                          publish_notify;
    ngx_flag_t                          play_restart;
    ngx_flag_t                          idle_streams;
    ngx_msec_t                         buflen;
    ngx_pool_t                         *pool;
    ngx_rtmp_live_stream_t             *free_streams;

    //liw
    ngx_msec_t                          cache_gop_duration;
    ngx_uint_t                          cache_gop_num;
    ngx_flag_t                          cache_gop;
} ngx_rtmp_live_app_conf_t;


extern ngx_module_t  ngx_rtmp_live_module;

ngx_int_t   
ngx_rtmp_live_find_stream(ngx_rtmp_live_app_conf_t* lacf, u_char* name);

ngx_uint_t  
ngx_rtmp_live_current_msec();

#endif /* _NGX_RTMP_LIVE_H_INCLUDED_ */
