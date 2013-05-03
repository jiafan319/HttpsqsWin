// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"
  
#define WIN32_LEAN_AND_MEAN  

#include <windows.h>  
#include <winsock2.h>  
  
#include <event2/event.h>  
#include <event2/buffer.h>
#include <event2/http.h>  
#include <event2/keyvalq_struct.h>
#include <event2/http_struct.h>

#include <stdio.h>
#include <tchar.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include <kcwin32/kclangc.h>

#include "getopt.h"
#include "tool.h"

// TODO: 在此处引用程序需要的其他头文件
