// HttpsqsWin.cpp : 定义控制台应用程序的入口点。
//


#pragma once

#include "targetver.h"
#include "stdafx.h"

#define VERSION "0.3"

/* 每个队列的默认最大长度为100万条 */
const _int64 HTTPSQS_DEFAULT_MAXQUEUE_ITEM = 1000000;
int _httpsqs_settings_syncinterval = 5;
char* _httpsqs_settings_pidfile;
char* _httpsqs_settings_auth = NULL;
KCDB *_httpsqs_db_tcbdb; /* 数据表 */

/* 读取队列写入点的值 */
static int httpsqs_read_putpos(const char* httpsqs_input_name)
{
	int queue_value = 0;
	char *queue_value_tmp;
	char queue_name[300] = {0}; /* 队列名称的总长度，用户输入的队列长度少于256字节 */
	size_t queue_value_size = 0;

	sprintf(queue_name, "%s:%s", httpsqs_input_name, "putpos");
	
	queue_value_tmp = kcdbget(_httpsqs_db_tcbdb, queue_name, strlen(queue_name), &queue_value_size);
	if(queue_value_tmp){
		queue_value = atoi(queue_value_tmp);
		free(queue_value_tmp);
	}
	
	return queue_value;
}

/* 读取队列读取点的值 */
static int httpsqs_read_getpos(const char* httpsqs_input_name)
{
	int queue_value = 0;
	char *queue_value_tmp;
	char queue_name[300] = {0}; /* 队列名称的总长度，用户输入的队列长度少于256字节 */
	size_t queue_value_size = 0;

	sprintf(queue_name, "%s:%s", httpsqs_input_name, "getpos");
	
	queue_value_tmp = kcdbget(_httpsqs_db_tcbdb, queue_name, strlen(queue_name), &queue_value_size);
	if(queue_value_tmp){
		queue_value = atoi(queue_value_tmp);
		free(queue_value_tmp);
	}
	
	return queue_value;
}

/* 读取用于设置的最大队列数 */
static int httpsqs_read_maxqueue(const char* httpsqs_input_name)
{
	int queue_value = 0;
	char *queue_value_tmp;
	char queue_name[300] = {0}; /* 队列名称的总长度，用户输入的队列长度少于256字节 */
	size_t queue_value_size = 0;

	sprintf(queue_name, "%s:%s", httpsqs_input_name, "maxqueue");
	
	queue_value_tmp = kcdbget(_httpsqs_db_tcbdb, queue_name, strlen(queue_name), &queue_value_size);
	if(queue_value_tmp){
		queue_value = atoi(queue_value_tmp);
		free(queue_value_tmp);
	} else {
		queue_value = HTTPSQS_DEFAULT_MAXQUEUE_ITEM; /* 默认队列长度 */
	}
	
	return queue_value;
}

/* 设置最大的队列数量，返回值为设置的队列数量。如果返回值为0，则表示设置取消（取消原因为：设置的最大的队列数量小于”当前队列写入位置点“和”当前队列读取位置点“，或者”当前队列写入位置点“小于”当前队列的读取位置点） */
static int httpsqs_maxqueue(const char* httpsqs_input_name, int httpsqs_input_num)
{
	int queue_put_value = 0;
	int queue_get_value = 0;
	int queue_maxnum_int = 0;
	
	/* 读取当前队列写入位置点 */
	queue_put_value = httpsqs_read_putpos(httpsqs_input_name);
	
	/* 读取当前队列读取位置点 */
	queue_get_value = httpsqs_read_getpos(httpsqs_input_name);

	/* 设置最大的队列数量，最小值为10条，最大值为10亿条 */
	queue_maxnum_int = httpsqs_input_num;
	
	/* 设置的最大的队列数量必须大于等于”当前队列写入位置点“和”当前队列读取位置点“，并且”当前队列写入位置点“必须大于等于”当前队列读取位置点“ */
	if (queue_maxnum_int >= queue_put_value && queue_maxnum_int >= queue_get_value && queue_put_value >= queue_get_value) {
		char queue_name[300] = {0}; /* 队列名称的总长度，用户输入的队列长度少于256字节 */
		char queue_maxnum[16] = {0};
		sprintf(queue_name, "%s:%s", httpsqs_input_name, "maxqueue");
		sprintf(queue_maxnum, "%d", queue_maxnum_int);
		kcdbset(_httpsqs_db_tcbdb, queue_name, strlen(queue_name), queue_maxnum, strlen(queue_maxnum));
		
		kcdbsync(_httpsqs_db_tcbdb, TRUE, NULL, NULL); /* 实时刷新到磁盘 */
		
		return queue_maxnum_int;
	}
	
	return 0;
}

/* 重置队列，0表示重置成功 */
static int httpsqs_reset(const char* httpsqs_input_name)
{
	char queue_name[300] = {0}; /* 队列名称的总长度，用户输入的队列长度少于256字节 */
	
	sprintf(queue_name, "%s:%s", httpsqs_input_name, "putpos");
	kcdbremove(_httpsqs_db_tcbdb, queue_name, strlen(queue_name));

	memset(queue_name, '\0', 300);
	sprintf(queue_name, "%s:%s", httpsqs_input_name, "getpos");
	kcdbremove(_httpsqs_db_tcbdb, queue_name, strlen(queue_name));
	
	memset(queue_name, '\0', 300);
	sprintf(queue_name, "%s:%s", httpsqs_input_name, "maxqueue");
	kcdbremove(_httpsqs_db_tcbdb, queue_name, strlen(queue_name));
	
	kcdbsync(_httpsqs_db_tcbdb, TRUE, NULL, NULL); /* 实时刷新到磁盘 */
	
	return 0;
}

/* 查看单条队列内容 */
char *httpsqs_view(const char* httpsqs_input_name, int pos)
{
	char *queue_value;
	char queue_name[300] = {0}; /* 队列名称的总长度，用户输入的队列长度少于256字节 */
	size_t queue_value_size = 0;

	sprintf(queue_name, "%s:%d", httpsqs_input_name, pos);
	
	queue_value = kcdbget(_httpsqs_db_tcbdb, queue_name, strlen(queue_name), &queue_value_size);
	
	return queue_value;
}

/* 修改定时更新内存内容到磁盘的间隔时间，返回间隔时间（秒） */
static int httpsqs_synctime(int httpsqs_input_num)
{
	if (httpsqs_input_num >= 1) {
		_httpsqs_settings_syncinterval = httpsqs_input_num;
	}
	return _httpsqs_settings_syncinterval;
}

/* 获取本次“入队列”操作的队列写入点 */
static int httpsqs_now_putpos(const char* httpsqs_input_name)
{
	int maxqueue_num = 0;
	int queue_put_value = 0;
	int queue_get_value = 0;
	char queue_name[300] = {0}; /* 队列名称的总长度，用户输入的队列长度少于256字节 */
	char queue_input[32] = {0};
	
	/* 获取最大队列数量 */
	maxqueue_num = httpsqs_read_maxqueue(httpsqs_input_name);
	
	/* 读取当前队列写入位置点 */
	queue_put_value = httpsqs_read_putpos(httpsqs_input_name);
	
	/* 读取当前队列读取位置点 */
	queue_get_value = httpsqs_read_getpos(httpsqs_input_name);	
	
	sprintf(queue_name, "%s:%s", httpsqs_input_name, "putpos");	
	
	/* 队列写入位置点加1 */
	queue_put_value = queue_put_value + 1;
	if (queue_put_value == queue_get_value) { /* 如果队列写入ID+1之后追上队列读取ID，则说明队列已满，返回0，拒绝继续写入 */
		queue_put_value = 0;
	}
	else if (queue_get_value <= 1 && queue_put_value > maxqueue_num) { /* 如果队列写入ID大于最大队列数量，并且从未进行过出队列操作（=0）或进行过1次出队列操作（=1），返回0，拒绝继续写入 */
		queue_put_value = 0;
	}	
	else if (queue_put_value > maxqueue_num) { /* 如果队列写入ID大于最大队列数量，则重置队列写入位置点的值为1 */
		if(kcdbset(_httpsqs_db_tcbdb, queue_name, strlen(queue_name), "1", 1)) {
			queue_put_value = 1;
		}
	} else { /* 队列写入位置点加1后的值，回写入数据库 */
		sprintf(queue_input, "%d", queue_put_value);
		kcdbset(_httpsqs_db_tcbdb, queue_name, strlen(queue_name), queue_input, strlen(queue_input));
	}
	
	return queue_put_value;
}

/* 获取本次“出队列”操作的队列读取点，返回值为0时队列全部读取完成 */
static int httpsqs_now_getpos(const char* httpsqs_input_name)
{
	int maxqueue_num = 0;
	int queue_put_value = 0;
	int queue_get_value = 0;
	char queue_name[300] = {0}; /* 队列名称的总长度，用户输入的队列长度少于256字节 */
	
	/* 获取最大队列数量 */
	maxqueue_num = httpsqs_read_maxqueue(httpsqs_input_name);
	
	/* 读取当前队列写入位置点 */
	queue_put_value = httpsqs_read_putpos(httpsqs_input_name);
	
	/* 读取当前队列读取位置点 */
	queue_get_value = httpsqs_read_getpos(httpsqs_input_name);
	
	/* 如果queue_get_value的值不存在，重置队列读取位置点为1 */
	sprintf(queue_name, "%s:%s", httpsqs_input_name, "getpos");
	/* 如果queue_get_value的值不存在，重置为1 */
	if (queue_get_value == 0 && queue_put_value > 0) {
		queue_get_value = 1;
		kcdbset(_httpsqs_db_tcbdb, queue_name, strlen(queue_name), "1", 1);
	/* 如果队列的读取值（出队列）小于队列的写入值（入队列） */
	} else if (queue_get_value < queue_put_value) {
		queue_get_value = queue_get_value + 1;
		char queue_input[32] = {0};
		sprintf(queue_input, "%d", queue_get_value);
		kcdbset(_httpsqs_db_tcbdb, queue_name, strlen(queue_name), queue_input, strlen(queue_input));
	/* 如果队列的读取值（出队列）大于队列的写入值（入队列），并且队列的读取值（出队列）小于最大队列数量 */
	} else if (queue_get_value > queue_put_value && queue_get_value < maxqueue_num) {
		queue_get_value = queue_get_value + 1;
		char queue_input[32] = {0};
		sprintf(queue_input, "%d", queue_get_value);
		kcdbset(_httpsqs_db_tcbdb, queue_name, strlen(queue_name), queue_input, strlen(queue_input));
	/* 如果队列的读取值（出队列）大于队列的写入值（入队列），并且队列的读取值（出队列）等于最大队列数量 */
	} else if (queue_get_value > queue_put_value && queue_get_value == maxqueue_num) {
		queue_get_value = 1;
		kcdbset(_httpsqs_db_tcbdb, queue_name, strlen(queue_name), "1", 1);
	/* 队列的读取值（出队列）等于队列的写入值（入队列），即队列中的数据已全部读出 */
	} else {
		queue_get_value = 0;
	}
	
	return queue_get_value;
}


/* 处理模块 */
void httpsqs_handler(struct evhttp_request *req, void *arg)
{
        struct evbuffer *buf;
        buf = evbuffer_new();
		
		/* 分析URL参数 */	
		const char *httpsqs_query_part;
		struct evkeyvalq httpsqs_http_query;
		httpsqs_query_part = evhttp_uri_get_query(req->uri_elems);
		evhttp_parse_query_str(httpsqs_query_part, &httpsqs_http_query);
		
		/* 接收GET表单参数 */
		const char *httpsqs_input_auth = evhttp_find_header (&httpsqs_http_query, "auth"); /* 队列名称 */
		const char *httpsqs_input_name = evhttp_find_header (&httpsqs_http_query, "name"); /* 队列名称 */
		const char *httpsqs_input_charset = evhttp_find_header (&httpsqs_http_query, "charset"); /* 操作类别 */
		const char *httpsqs_input_opt = evhttp_find_header (&httpsqs_http_query, "opt"); /* 操作类别 */
		const char *httpsqs_input_data = evhttp_find_header (&httpsqs_http_query, "data"); /* 操作类别 */
		const char *httpsqs_input_pos_tmp = evhttp_find_header (&httpsqs_http_query, "pos"); /* 队列位置点 字符型 */
		const char *httpsqs_input_num_tmp = evhttp_find_header (&httpsqs_http_query, "num"); /* 队列总长度 字符型 */
		int httpsqs_input_pos = 0;
		int httpsqs_input_num = 0;
		
		/* 返回给用户的Header头信息 */
		if (httpsqs_input_charset != NULL && strlen(httpsqs_input_charset) <= 40) {
			char content_type[64] = {0};
			sprintf(content_type, "text/plain; charset=%s", httpsqs_input_charset);
			evhttp_add_header(req->output_headers, "Content-Type", content_type);
		} else {
			evhttp_add_header(req->output_headers, "Content-Type", "text/plain");
		}
		evhttp_add_header(req->output_headers, "Connection", "keep-alive");
		evhttp_add_header(req->output_headers, "Cache-Control", "no-cache");
		//evhttp_add_header(req->output_headers, "Connection", "close");		
		
		/* 权限校验 */
		bool is_auth_pass = false; /* 是否验证通过 */
		if (_httpsqs_settings_auth != NULL) {
			/* 如果命令行启动参数设置了验证密码 */
			if (httpsqs_input_auth != NULL && strcmp(_httpsqs_settings_auth, httpsqs_input_auth) == 0) {
				is_auth_pass = true;
			} else {
				is_auth_pass = false;
			}
		} else {
			/* 如果命令行启动参数没有设置验证密码 */
			is_auth_pass = true;
		}

		if (is_auth_pass == false) {
			/* 校验失败 */
			evbuffer_add_printf(buf, "%s", "HTTPSQS_AUTH_FAILED");
		}
		else 
		{
			/* 校验成功，或者命令行启动参数没有设置校验密码 */
			if (httpsqs_input_pos_tmp != NULL) {
				httpsqs_input_pos = atoi(httpsqs_input_pos_tmp); /* 队列位置点 数值型 */
			}
			if (httpsqs_input_num_tmp != NULL) {
				httpsqs_input_num = atoi(httpsqs_input_num_tmp); /* 队列总长度 数值型 */
			}
	
			/*参数是否存在判断 */
			if (httpsqs_input_name != NULL && httpsqs_input_opt != NULL && strlen(httpsqs_input_name) <= 256) {
				/* 入队列 */
				if (strcmp(httpsqs_input_opt, "put") == 0) {
					/* 优先接收POST正文信息 */
					int buffer_data_len;
					buffer_data_len = evbuffer_get_length(req->input_buffer);
					if (buffer_data_len > 0) {
						int queue_put_value = httpsqs_now_putpos((char *)httpsqs_input_name);
						if (queue_put_value > 0) {
							char queue_name[300] = {0}; /* 队列名称的总长度，用户输入的队列长度少于256字节 */
							sprintf(queue_name, "%s:%d", httpsqs_input_name, queue_put_value);
							char *httpsqs_input_postbuffer;					
							char *buffer_data = (char *)calloc(1, buffer_data_len + 1);
							memcpy (buffer_data, evbuffer_pullup(req->input_buffer, -1), buffer_data_len);
							httpsqs_input_postbuffer = urldecode(buffer_data);
							kcdbset(_httpsqs_db_tcbdb, queue_name, strlen(queue_name), httpsqs_input_postbuffer, strlen(httpsqs_input_postbuffer));
							memset(queue_name, '\0', 300);
							sprintf(queue_name, "%d", queue_put_value);					
							evhttp_add_header(req->output_headers, "Pos", queue_name);
							evbuffer_add_printf(buf, "%s", "HTTPSQS_PUT_OK");
							free(httpsqs_input_postbuffer);
							free(buffer_data);
						} else {
							evbuffer_add_printf(buf, "%s", "HTTPSQS_PUT_END");
						}
					/* 如果POST正文无内容，则取URL中data参数的值 */
					} else if (httpsqs_input_data != NULL) {
						int queue_put_value = httpsqs_now_putpos((char *)httpsqs_input_name);
						if (queue_put_value > 0) {
							char queue_name[300] = {0}; /* 队列名称的总长度，用户输入的队列长度少于256字节 */
							sprintf(queue_name, "%s:%d", httpsqs_input_name, queue_put_value);				
							buffer_data_len = strlen(httpsqs_input_data);
							char *httpsqs_input_postbuffer;
							char *buffer_data = (char *)calloc(1, buffer_data_len + 1);					
							memcpy (buffer_data, httpsqs_input_data, buffer_data_len);
							httpsqs_input_postbuffer = urldecode(buffer_data);
							kcdbset(_httpsqs_db_tcbdb, queue_name, strlen(queue_name), httpsqs_input_postbuffer, strlen(httpsqs_input_postbuffer));
							memset(queue_name, '\0', 300);
							sprintf(queue_name, "%d", queue_put_value);					
							evhttp_add_header(req->output_headers, "Pos", queue_name);
							evbuffer_add_printf(buf, "%s", "HTTPSQS_PUT_OK");
							free(httpsqs_input_postbuffer);
							free(buffer_data);
						} else {
							evbuffer_add_printf(buf, "%s", "HTTPSQS_PUT_END");
						}
					} else {
						evbuffer_add_printf(buf, "%s", "HTTPSQS_PUT_ERROR");
					}
				}
				/* 出队列 */
				else if (strcmp(httpsqs_input_opt, "get") == 0) {
					int queue_get_value = 0;
					queue_get_value = httpsqs_now_getpos((char *)httpsqs_input_name);
					if (queue_get_value == 0) {
						evbuffer_add_printf(buf, "%s", "HTTPSQS_GET_END");
					} else {
						char queue_name[300] = {0}; /* 队列名称的总长度，用户输入的队列长度少于256字节 */
						sprintf(queue_name, "%s:%d", httpsqs_input_name, queue_get_value);
						char *httpsqs_output_value;
						size_t httpsqs_output_value_size = 0;
						httpsqs_output_value = kcdbget(_httpsqs_db_tcbdb, queue_name, strlen(queue_name), &httpsqs_output_value_size);
						if (httpsqs_output_value) {
							memset(queue_name, '\0', 300);
							sprintf(queue_name, "%d", queue_get_value);	
							evhttp_add_header(req->output_headers, "Pos", queue_name);
							evbuffer_add_printf(buf, "%s", httpsqs_output_value);
							free(httpsqs_output_value);
						} else {
							evbuffer_add_printf(buf, "%s", "HTTPSQS_GET_END");
						}
					}
				}
				/* 查看队列状态（普通浏览方式） */
				else if (strcmp(httpsqs_input_opt, "status") == 0) {
					int maxqueue = httpsqs_read_maxqueue((char *)httpsqs_input_name); /* 最大队列数量 */
					int putpos = httpsqs_read_putpos((char *)httpsqs_input_name); /* 入队列写入位置 */
					int getpos = httpsqs_read_getpos((char *)httpsqs_input_name); /* 出队列读取位置 */
					int ungetnum;
					const char *put_times;
					const char *get_times;
					if (putpos >= getpos) {
						ungetnum = abs(putpos - getpos); /* 尚未出队列条数 */
						put_times = "1st lap";
						get_times = "1st lap";
					} else if (putpos < getpos) {
						ungetnum = abs(maxqueue - getpos + putpos); /* 尚未出队列条数 */
						put_times = "2nd lap";
						get_times = "1st lap";
					}
					evbuffer_add_printf(buf, "HTTP Simple Queue Service v%s\n", VERSION);
					evbuffer_add_printf(buf, "------------------------------\n");
					evbuffer_add_printf(buf, "Queue Name: %s\n", httpsqs_input_name);
					evbuffer_add_printf(buf, "Maximum number of queues: %d\n", maxqueue);
					evbuffer_add_printf(buf, "Put position of queue (%s): %d\n", put_times, putpos);
					evbuffer_add_printf(buf, "Get position of queue (%s): %d\n", get_times, getpos);
					evbuffer_add_printf(buf, "Number of unread queue: %d\n", ungetnum);
				}
				/* 查看队列状态（JSON方式，方便客服端程序处理） */
				else if (strcmp(httpsqs_input_opt, "status_json") == 0) {
					int maxqueue = httpsqs_read_maxqueue((char *)httpsqs_input_name); /* 最大队列数量 */
					int putpos = httpsqs_read_putpos((char *)httpsqs_input_name); /* 入队列写入位置 */
					int getpos = httpsqs_read_getpos((char *)httpsqs_input_name); /* 出队列读取位置 */
					int ungetnum;
					const char *put_times;
					const char *get_times;
					if (putpos >= getpos) {
						ungetnum = abs(putpos - getpos); /* 尚未出队列条数 */
						put_times = "1";
						get_times = "1";
					} else if (putpos < getpos) {
						ungetnum = abs(maxqueue - getpos + putpos); /* 尚未出队列条数 */
						put_times = "2";
						get_times = "1";
					}
					evbuffer_add_printf(buf, "{\"name\":\"%s\",\"maxqueue\":%d,\"putpos\":%d,\"putlap\":%s,\"getpos\":%d,\"getlap\":%s,\"unread\":%d}\n", httpsqs_input_name, maxqueue, putpos, put_times, getpos, get_times, ungetnum);
				}			
				/* 查看单条队列内容 */
				else if (strcmp(httpsqs_input_opt, "view") == 0 && httpsqs_input_pos >= 1 && httpsqs_input_pos <= 1000000000) {
					char *httpsqs_output_value;
					httpsqs_output_value = httpsqs_view ((char *)httpsqs_input_name, httpsqs_input_pos);
					if (httpsqs_output_value) {
						evbuffer_add_printf(buf, "%s", httpsqs_output_value);
						free(httpsqs_output_value);
					}
				}
				/* 重置队列 */
				else if (strcmp(httpsqs_input_opt, "reset") == 0) {
					int reset = httpsqs_reset((char *)httpsqs_input_name);
					if (reset == 0) {
						evbuffer_add_printf(buf, "%s", "HTTPSQS_RESET_OK");
					} else {
						evbuffer_add_printf(buf, "%s", "HTTPSQS_RESET_ERROR");
					}
				}
				/* 设置最大的队列数量，最小值为10条，最大值为10亿条 */
				else if (strcmp(httpsqs_input_opt, "maxqueue") == 0 && httpsqs_input_num >= 10 && httpsqs_input_num <= 1000000000) {
					if (httpsqs_maxqueue((char *)httpsqs_input_name, httpsqs_input_num) != 0) {
						/* 设置成功 */
						evbuffer_add_printf(buf, "%s", "HTTPSQS_MAXQUEUE_OK");
					} else {
						/* 设置取消 */
						evbuffer_add_printf(buf, "%s", "HTTPSQS_MAXQUEUE_CANCEL");
					}
				}
				/* 设置定时更新内存内容到磁盘的间隔时间，最小值为1秒，最大值为10亿秒 */
				else if (strcmp(httpsqs_input_opt, "synctime") == 0 && httpsqs_input_num >= 1 && httpsqs_input_num <= 1000000000) {
					if (httpsqs_synctime(httpsqs_input_num) >= 1) {
						/* 设置成功 */
						evbuffer_add_printf(buf, "%s", "HTTPSQS_SYNCTIME_OK");
					} else {
						/* 设置取消 */
						evbuffer_add_printf(buf, "%s", "HTTPSQS_SYNCTIME_CANCEL");
					}				
				} else {
					/* 命令错误 */
					evbuffer_add_printf(buf, "%s", "HTTPSQS_ERROR");				
				}
			} else {
				/* 命令错误 */
				evbuffer_add_printf(buf, "%s", "HTTPSQS_ERROR");
			}
		}
	
		/* 输出内容给客户端 */
        evhttp_send_reply(req, HTTP_OK, "OK", buf);
		
		/* 内存释放 */
		evhttp_clear_headers(&httpsqs_http_query);
		evbuffer_free(buf);
}

/* 定时同步线程，定时将内存中的内容写入磁盘 */
static DWORD WINAPI sync_worker(LPVOID lpParam) {
	// pthread_detach(pthread_self());

	while(1)
	{
		/* 间隔httpsqs_settings_syncinterval秒同步一次数据到磁盘 */
		Sleep(_httpsqs_settings_syncinterval * 1000);	/* windows Sleep函数单位是毫秒 */
	
		/* 同步内存数据到磁盘 */
		kcdbsync(_httpsqs_db_tcbdb, TRUE, NULL, NULL);
	}
}


static void show_help(void)
{
	char *b = 
		  "HTTP Simple Queue Service for Windows - HttpsqsWin v" VERSION " (May 1, 2013)\n\n"
		  "Port from the Orignal Linux Httpsqs by: Jia Fan (jiafan319@gmail.com) \n"
		  "Orignal Linux edttion Author: Zhang Yan (http://blog.s135.com), E-mail: net@s135.com\n"
		  "This is free software, and you are welcome to modify and redistribute it under the New BSD License\n"
		  "\n"
		   "-l <ip_addr>  interface to listen on, default is 0.0.0.0\n"
		   "-p <num>      TCP port number to listen on (default: 1218)\n"
		   "-x <path>     database directory (example: C:\\HttpsqsWin\\Data)\n"
		   "-t <second>   keep-alive timeout for an http request (default: 60)\n"
		   "-s <second>   the interval to sync updated contents to the disk (default: 5)\n"
		   "-a <auth>     the auth password to access httpsqs (example: mypass123)\n"
		   "-h            print this help and exit\n\n"
		   "Use command \"killall httpsqs\", \"pkill httpsqs\" and \"kill `cat /tmp/httpsqs.pid`\" to stop httpsqs.\n"
		   "Please note that don't use the command \"pkill -9 httpsqs\" and \"kill -9 PID of httpsqs\"!\n"
		   "\n"
		   "Please visit \"http://code.google.com/p/httpsqs\" for more help information.\n\n"
		   "\n";
	fprintf(stderr, b, strlen(b));
}


static void
signal_cb(evutil_socket_t sig, short events, void *user_data)
{
	struct event_base *base = (event_base*)user_data;
	struct timeval delay = { 2, 0 };

	printf("Caught an interrupt signal; exiting cleanly in two seconds.\n");
	printf("Flush database files and close database.\n");
	kcdbsync(_httpsqs_db_tcbdb, TRUE, NULL, NULL);
	kcdbclose(_httpsqs_db_tcbdb);
	// tcbdbdel(httpsqs_db_tcbdb);

	event_base_loopexit(base, &delay);
}

void kill_signal_worker()
{
	kcdbsync(_httpsqs_db_tcbdb, TRUE, NULL, NULL);
	kcdbclose(_httpsqs_db_tcbdb);
	// tcbdbdel(httpsqs_db_tcbdb);
	printf("Flush database files and close database.\n");
}

int _tmain(int argc, _TCHAR* argv[])
{
	/* 默认参数设置 */

	char *httpsqs_settings_listen = "0.0.0.0";
	int httpsqs_settings_port = 1218;
	char *httpsqs_settings_datapath = "C:\\HTTPSQSWIN\\";
	bool httpsqs_settings_daemon = false;
	int httpsqs_settings_timeout = 60; /* 单位：毫秒 */

	int httpsqs_settings_cachenonleaf = 1024; /* 缓存非叶子节点数。单位：条 */
	int httpsqs_settings_cacheleaf = 2048; /* 缓存叶子节点数。叶子节点缓存数为非叶子节点数的两倍。单位：条 */
	int httpsqs_settings_mappedmemory = 104857600; /* 单位：字节 */

	int c;
	/* process arguments */
    while ((c = getopt(argc, argv, "l:p:x:t:s:c:m:i:a:dh")) != -1) {
        switch (c) {
        case 'l':
            httpsqs_settings_listen = _strdup(optarg);
            break;
        case 'p':
            httpsqs_settings_port = atoi(optarg);
            break;
        case 'x':
            httpsqs_settings_datapath = _strdup(optarg); /* 数据库文件存放路径 */
            break;
        case 't':
            httpsqs_settings_timeout = atoi(optarg);
            break;
        case 's':
            _httpsqs_settings_syncinterval = atoi(optarg);
            break;
        case 'c':
            httpsqs_settings_cachenonleaf = atoi(optarg);
			httpsqs_settings_cacheleaf = httpsqs_settings_cachenonleaf * 2;
            break;
        case 'a':
            _httpsqs_settings_auth = _strdup(optarg);
            break;			
		case 'h':
        default:
            show_help();
            return 1;
        }
    }
    
    if (!dir_exist(httpsqs_settings_datapath))
    	create_dir(httpsqs_settings_datapath);
            	
	/* 数据表路径 */
	int httpsqs_settings_dataname_len = 1024;
	char *httpsqs_settings_dataname = (char *)calloc(1, httpsqs_settings_dataname_len);
	sprintf(httpsqs_settings_dataname, "%s\\httpsqswin.kct", httpsqs_settings_datapath);

	/* 打开数据表 */
	_httpsqs_db_tcbdb = kcdbnew();
	// (KCDB不支持以下操作) 
	// tcbdbsetmutex(_httpsqs_db_tcbdb); /* 开启线程互斥锁  */
	// tcbdbtune(httpsqs_db_tcbdb, 1024, 2048, 50000000, 8, 10, BDBTLARGE);
	// tcbdbsetcache(httpsqs_db_tcbdb, httpsqs_settings_cacheleaf, httpsqs_settings_cachenonleaf);
	// tcbdbsetxmsiz(httpsqs_db_tcbdb, httpsqs_settings_mappedmemory); /* 内存缓存大小 */
					
	/* 判断表是否能打开 */
	if(!kcdbopen(_httpsqs_db_tcbdb, httpsqs_settings_dataname, KCOWRITER |KCOCREATE)){
		fprintf(stderr, "Error: Unable to open database.\n\n");	
		fprintf(stderr, kcdbemsg(_httpsqs_db_tcbdb));
		exit(1);
	}
	
	free(httpsqs_settings_dataname);
	
	DWORD pid = GetCurrentProcessId();

	_httpsqs_settings_pidfile = (char *)calloc(1, 1024);
	sprintf(_httpsqs_settings_pidfile, "%s\\httpsqswin.pid", httpsqs_settings_datapath);
	FILE *fp_pidfile;
	fp_pidfile = fopen(_httpsqs_settings_pidfile, "w");
	fprintf(fp_pidfile, "%d\n", pid);
	fclose(fp_pidfile);
	free(_httpsqs_settings_pidfile);

	DWORD dwThreadIdArray[1];

	/* 创建定时同步线程，定时将内存中的内容写入磁盘 */
	HANDLE sync_worker_tid = CreateThread(NULL, 0, sync_worker, NULL, 0, &dwThreadIdArray[0]);
	
	struct event_base *base;
	struct evhttp *httpd;  
	struct evhttp_bound_socket *handle;
	struct event *signal_event;

	WSADATA wsaData;  
	DWORD Ret;  
	if ((Ret = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)  {  
		printf("WSAStartup failed with error %d\n", Ret);  
		return -1;  
	}  

	base = event_base_new();
	if (!base) {
		fprintf(stderr, "Couldn't create an event_base: Exiting\n");
		return 1;
	}

	/* Create a new evhttp object to handle requests. */
	httpd = evhttp_new(base);
	if (!httpd) {
		fprintf(stderr, "couldn't create evhttp. Exiting.\n");
		return 1;
	}

	handle = evhttp_bind_socket_with_handle(httpd, httpsqs_settings_listen, httpsqs_settings_port);
	if (!handle) {
		fprintf(stderr, "couldn't bind to port %d. Exiting.\n", httpsqs_settings_port);
		return 1;
	}
	signal_event = evsignal_new(base, SIGINT, signal_cb, (void *)base);

	if (!signal_event || event_add(signal_event, NULL)<0) {
		fprintf(stderr, "Could not create/add a signal event!\n");
		return 1;
	}

	evhttp_set_timeout(httpd, httpsqs_settings_timeout);
	// evhttp_set_cb(httpd, "/", root_handler, NULL);  
	evhttp_set_gencb(httpd, httpsqs_handler, NULL);  

	printf("--- HttpsqsWin Service Started ---\n Press Ctrl+C to exit.\n !!! DO NOT CLOSE THIS CONSOLE WINDOW !!!\n");  

	event_base_dispatch(base);

	evhttp_free(httpd);  
	event_free(signal_event);
	event_base_free(base);

	WSACleanup();  
	return 0;  
}

