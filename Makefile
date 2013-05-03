VCPATH = $(VCINSTALLDIR) 

OBJFILES = tools.obj getopt.obj HttpsqsWin.obj
LIBFILES = kyotocabinet.lib libevent.lib libevent_core.lib libevent_extras.lib ws2_32.lib advapi32.lib
EXEFILES = HttpsqsWin.exe

# Building configuration
CL = cl
LINK = link
CLFLAGS = /nologo \
  /I "$(INCLUDE)"  /I "." /I "include" \
  /DNDEBUG /D_CRT_SECURE_NO_WARNINGS \
  /O2 /EHsc /W3 /wd4244 /wd4351 /wd4800 /MT

LINKFLAGS = /nologo \
  /libpath:"$(LIB)" /libpath:"lib"
  

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