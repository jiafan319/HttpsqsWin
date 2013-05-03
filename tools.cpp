#include "stdafx.h"

char *urldecode(char *input_str) 
{
		int len = strlen(input_str);
		char *str = _strdup(input_str);
		
        char *dest = str; 
        char *data = str; 

        int value; 
        int c; 

        while (len--) { 
                if (*data == '+') { 
                        *dest = ' '; 
                } 
                else if (*data == '%' && len >= 2 && isxdigit((int) *(data + 1)) 
  && isxdigit((int) *(data + 2))) 
                { 

                        c = ((unsigned char *)(data+1))[0]; 
                        if (isupper(c)) 
                                c = tolower(c); 
                        value = (c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10) * 16; 
                        c = ((unsigned char *)(data+1))[1]; 
                        if (isupper(c)) 
                                c = tolower(c); 
                                value += c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10; 

                        *dest = (char)value ; 
                        data += 2; 
                        len -= 2; 
                } else { 
                        *dest = *data; 
                } 
                data++; 
                dest++; 
        } 
        *dest = '\0'; 
        return str; 
}

void create_dir( const char *dir ) 
{
    int    i,len;
    char    str[512];
    
    strncpy( str, dir, 512 );
    len=strlen(str);
    for( i=0; i<len; i++ )
    {
        if( str[i]=='/' )
        {
            str[i] = '\0';
            //判断此目录是否存在,不存在则创建
            if(!dir_exist(str))
            {
                CreateDirectory(str, NULL);
            }
            str[i]='/';
        }
    }
    if( len>0 && !dir_exist(str) )
    {
        CreateDirectory( str, NULL );
    }

    return;
}

int dir_exist(const char* strPath)
{
    WIN32_FIND_DATA  wfd;
    int rValue = 0;
    HANDLE hFind = FindFirstFile(strPath, &wfd);
    if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
    {
        rValue = 1;   
    }
    FindClose(hFind);
    return rValue;
}