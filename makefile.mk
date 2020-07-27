LINKFLAGS=/LIBPATH:"." /LIBPATH:"C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Lib\x64" /WX
LIBS=mfplat.lib mfreadwrite.lib mfuuid.lib Ole32.lib mf.lib strmiids.lib Evr.lib user32.lib

INC= -I.

CC=cl.exe
AR=lib.exe
LINK=link.exe


FLAGS=/D_WINDOWS /DENABLE_MEDIA_ROUTER=1 /D__STD_C /D_CRT_RAND_S /D_CRT_SECURE_NO_DEPRECATE /D_HAS_EXCEPTIONS=0 /DNOMINMAX /DWINVER=0x0A00 /D__STDC_CONSTANT_MACROS /D__STDC_FORMAT_MACROS /D_AMD64_ /DRTSP_LOG /EHsc /W3


default:   h264decoder.lib

test: test.exe

test.exe:	h264decoder.lib main.o
	$(LINK) $(LINKFLAGS) /OUT:test.exe main.o h264decoder.lib $(LIBS)

h264decoder.lib: h264decoder.o
	$(AR) $(LINKFLAGS) /OUT:h264decoder.lib h264decoder.o
	
h264decoder.o: h264decoder.cpp
	$(CC) $(FLAGS) $(INC) /c h264decoder.cpp /Foh264decoder.o
	
main.o: main.cpp
	$(CC) $(FLAGS) $(INC) /c main.cpp /Fomain.o
		
clean:
	del *.o
	del *.lib
	del *.exe

