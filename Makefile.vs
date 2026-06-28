APP=flappy
CFG=Release
SFML_DIR=SFML-3.1.0
SRC_DIR=src
BUILD_DIR=build
OBJ_DIR=$(BUILD_DIR)\obj\$(CFG)
BIN_DIR=.

CPP=cl
LD=link

CXXFLAGS=/nologo /std:c++20 /EHsc /permissive- /W3 /DWIN32 /D_WINDOWS
INCLUDES=/I $(SFML_DIR)\include /I $(SRC_DIR)
LDFLAGS=/nologo /SUBSYSTEM:WINDOWS /INCREMENTAL:NO /LIBPATH:$(SFML_DIR)\lib

WINLIBS=user32.lib gdi32.lib winmm.lib advapi32.lib opengl32.lib

!IF "$(CFG)" == "Debug"
OUT_EXE=$(BIN_DIR)\$(APP)-d.exe
CXXFLAGS=$(CXXFLAGS) /MDd /Zi /Od /DDEBUG
LDFLAGS=$(LDFLAGS) /DEBUG
SFML_LIBS=sfml-graphics-d.lib sfml-window-d.lib sfml-system-d.lib sfml-main-d.lib
SFML_DLLS=sfml-graphics-d-3.dll sfml-window-d-3.dll sfml-system-d-3.dll
!ELSE
OUT_EXE=$(BIN_DIR)\$(APP).exe
CXXFLAGS=$(CXXFLAGS) /MD /O2 /DNDEBUG
SFML_LIBS=sfml-graphics.lib sfml-window.lib sfml-system.lib sfml-main.lib
SFML_DLLS=sfml-graphics-3.dll sfml-window-3.dll sfml-system-3.dll
!ENDIF

OBJS=$(OBJ_DIR)\main.obj $(OBJ_DIR)\Game.obj $(OBJ_DIR)\Bird.obj $(OBJ_DIR)\PipeSystem.obj $(OBJ_DIR)\HighScore.obj

all: dirs $(OUT_EXE) copydlls

dirs:
	@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"
	@if not exist "$(OBJ_DIR)" mkdir "$(OBJ_DIR)"

$(OBJ_DIR)\main.obj: $(SRC_DIR)\main.cpp
	$(CPP) $(CXXFLAGS) $(INCLUDES) /c $(SRC_DIR)\main.cpp /Fo$(OBJ_DIR)\main.obj

$(OBJ_DIR)\Game.obj: $(SRC_DIR)\Game.cpp $(SRC_DIR)\Game.hpp $(SRC_DIR)\Bird.hpp $(SRC_DIR)\PipeSystem.hpp $(SRC_DIR)\HighScore.hpp
	$(CPP) $(CXXFLAGS) $(INCLUDES) /c $(SRC_DIR)\Game.cpp /Fo$(OBJ_DIR)\Game.obj

$(OBJ_DIR)\Bird.obj: $(SRC_DIR)\Bird.cpp $(SRC_DIR)\Bird.hpp
	$(CPP) $(CXXFLAGS) $(INCLUDES) /c $(SRC_DIR)\Bird.cpp /Fo$(OBJ_DIR)\Bird.obj

$(OBJ_DIR)\PipeSystem.obj: $(SRC_DIR)\PipeSystem.cpp $(SRC_DIR)\PipeSystem.hpp
	$(CPP) $(CXXFLAGS) $(INCLUDES) /c $(SRC_DIR)\PipeSystem.cpp /Fo$(OBJ_DIR)\PipeSystem.obj

$(OBJ_DIR)\HighScore.obj: $(SRC_DIR)\HighScore.cpp $(SRC_DIR)\HighScore.hpp
	$(CPP) $(CXXFLAGS) $(INCLUDES) /c $(SRC_DIR)\HighScore.cpp /Fo$(OBJ_DIR)\HighScore.obj

$(OUT_EXE): $(OBJS)
	$(LD) $(LDFLAGS) /OUT:$(OUT_EXE) $(OBJS) $(SFML_LIBS) $(WINLIBS)

copydlls:
!IF "$(CFG)" == "Debug"
	@for %f in ($(SFML_DLLS)) do @copy /Y "$(SFML_DIR)\bin\%f" "$(BIN_DIR)\%f" >NUL
!ELSE
	@for %f in ($(SFML_DLLS)) do @copy /Y "$(SFML_DIR)\bin\%f" "$(BIN_DIR)\%f" >NUL
!ENDIF

clean:
	@if exist "$(BUILD_DIR)" rmdir /S /Q "$(BUILD_DIR)"

run: all
	"$(OUT_EXE)"

