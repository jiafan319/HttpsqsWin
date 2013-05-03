
# VC++ directory
VCPATH = C:\Program Files\Microsoft Visual Studio 10.0\VC
SDKPATH = C:\Program Files\Microsoft SDKs\Windows\v7.0A

OBJFILES = tools.obj getopt.obj HttpsqsWin.obj
LIBFILES = kyotocabinet.lib libevent.lib libevent_core.lib libevent_extras.lib ws2_32.lib advapi32.lib
EXEFILES = HttpsqsWin.exe

# Building configuration
CL = cl
LIB = lib
LINK = link
CLFLAGS = /nologo \
  /I "$(VCPATH)\Include" /I "$(VCPATH)\PlatformSDK\Include" /I "$(SDKPATH)\Include" \
  /I "." /I "include" \
  /DNDEBUG /D_CRT_SECURE_NO_WARNINGS \
  /O2 /EHsc /W3 /wd4244 /wd4351 /wd4800 /MT

LINKFLAGS = /nologo \
  /libpath:"$(VCPATH)\lib" /libpath:"$(VCPATH)\PlatformSDK\Lib" /libpath:"$(SDKPATH)\Lib" \
  /libpath:"lib"
  

.SUFFIXES :
.SUFFIXES : .cpp .c .obj .exe

.c.obj :
	$(CL) /c $(CLFLAGS) $<

.cpp.obj :
	$(CL) /c $(CLFLAGS) $<



#================================================================
# Actions
#================================================================


all : $(EXEFILES)
	@echo ================================================================
	@echo Complete!                       
	@echo ================================================================




clean :
	-del *.obj *.exe /F /Q > NUL: 2>&1
	


HttpsqsWin.exe : $(OBJFILES) 
	$(LINK) $(LINKFLAGS) /OUT:$@ $(OBJFILES) $(LIBFILES)

tools.obj :
HttpsqsWin.obj : 
getopt.obj :