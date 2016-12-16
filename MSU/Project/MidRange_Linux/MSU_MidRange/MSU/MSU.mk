##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Release
ProjectName            :=MSU
ConfigurationName      :=Release
WorkspacePath          := "D:\Project\MSU\Project\MidRange_Linux"
ProjectPath            := "D:\Project\MSU\Project\MidRange_Linux\MSU_MidRange\MSU"
IntermediateDirectory  :=./Release
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=SESA58149
Date                   :=05/06/15
CodeLitePath           :="C:\Program Files\CodeLite"
LinkerName             :=gcc
SharedObjectLinkerName :=gcc -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.o.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=$(PreprocessorSwitch)NDEBUG 
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E 
ObjectsFileList        :="MSU.txt"
PCHCompileFlags        :=
MakeDirCommand         :=makedir
RcCmpOptions           := 
RcCompilerName         :=windres
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := ar rcus
CXX      := gcc
CC       := gcc
CXXFLAGS := -ansi -pedantic -W -O2 -Wall $(Preprocessors)
CFLAGS   := -Wfatal-errors -w -ansi -W -Wall -O2 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := as


##
## User defined environment variables
##
CodeLiteDir:=C:\Program Files\CodeLite
Objects0=$(IntermediateDirectory)/Core_MsuComplainMode.c$(ObjectSuffix) $(IntermediateDirectory)/Core_MsuCRC32.c$(ObjectSuffix) $(IntermediateDirectory)/Core_MsuLeech.c$(ObjectSuffix) $(IntermediateDirectory)/Core_MsuPdu.c$(ObjectSuffix) $(IntermediateDirectory)/Core_MsuSocket.c$(ObjectSuffix) $(IntermediateDirectory)/Interface_MsuFileInterface.c$(ObjectSuffix) $(IntermediateDirectory)/Interface_MsuPlatformInterface.c$(ObjectSuffix) $(IntermediateDirectory)/Interface_MsuSocketInterface.c$(ObjectSuffix) $(IntermediateDirectory)/MSU_MidRange_MsuMain.c$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "./Release"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/Core_MsuComplainMode.c$(ObjectSuffix): /cygwin/home/MSU_MidRange/Core/MsuComplainMode.c $(IntermediateDirectory)/Core_MsuComplainMode.c$(DependSuffix)
	$(CC) $(SourceSwitch) "D:/cygwin/home/MSU_MidRange/Core/MsuComplainMode.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Core_MsuComplainMode.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Core_MsuComplainMode.c$(DependSuffix): /cygwin/home/MSU_MidRange/Core/MsuComplainMode.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Core_MsuComplainMode.c$(ObjectSuffix) -MF$(IntermediateDirectory)/Core_MsuComplainMode.c$(DependSuffix) -MM "/cygwin/home/MSU_MidRange/Core/MsuComplainMode.c"

$(IntermediateDirectory)/Core_MsuComplainMode.c$(PreprocessSuffix): /cygwin/home/MSU_MidRange/Core/MsuComplainMode.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Core_MsuComplainMode.c$(PreprocessSuffix) "/cygwin/home/MSU_MidRange/Core/MsuComplainMode.c"

$(IntermediateDirectory)/Core_MsuCRC32.c$(ObjectSuffix): /cygwin/home/MSU_MidRange/Core/MsuCRC32.c $(IntermediateDirectory)/Core_MsuCRC32.c$(DependSuffix)
	$(CC) $(SourceSwitch) "D:/cygwin/home/MSU_MidRange/Core/MsuCRC32.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Core_MsuCRC32.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Core_MsuCRC32.c$(DependSuffix): /cygwin/home/MSU_MidRange/Core/MsuCRC32.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Core_MsuCRC32.c$(ObjectSuffix) -MF$(IntermediateDirectory)/Core_MsuCRC32.c$(DependSuffix) -MM "/cygwin/home/MSU_MidRange/Core/MsuCRC32.c"

$(IntermediateDirectory)/Core_MsuCRC32.c$(PreprocessSuffix): /cygwin/home/MSU_MidRange/Core/MsuCRC32.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Core_MsuCRC32.c$(PreprocessSuffix) "/cygwin/home/MSU_MidRange/Core/MsuCRC32.c"

$(IntermediateDirectory)/Core_MsuLeech.c$(ObjectSuffix): /cygwin/home/MSU_MidRange/Core/MsuLeech.c $(IntermediateDirectory)/Core_MsuLeech.c$(DependSuffix)
	$(CC) $(SourceSwitch) "D:/cygwin/home/MSU_MidRange/Core/MsuLeech.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Core_MsuLeech.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Core_MsuLeech.c$(DependSuffix): /cygwin/home/MSU_MidRange/Core/MsuLeech.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Core_MsuLeech.c$(ObjectSuffix) -MF$(IntermediateDirectory)/Core_MsuLeech.c$(DependSuffix) -MM "/cygwin/home/MSU_MidRange/Core/MsuLeech.c"

$(IntermediateDirectory)/Core_MsuLeech.c$(PreprocessSuffix): /cygwin/home/MSU_MidRange/Core/MsuLeech.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Core_MsuLeech.c$(PreprocessSuffix) "/cygwin/home/MSU_MidRange/Core/MsuLeech.c"

$(IntermediateDirectory)/Core_MsuPdu.c$(ObjectSuffix): /cygwin/home/MSU_MidRange/Core/MsuPdu.c $(IntermediateDirectory)/Core_MsuPdu.c$(DependSuffix)
	$(CC) $(SourceSwitch) "D:/cygwin/home/MSU_MidRange/Core/MsuPdu.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Core_MsuPdu.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Core_MsuPdu.c$(DependSuffix): /cygwin/home/MSU_MidRange/Core/MsuPdu.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Core_MsuPdu.c$(ObjectSuffix) -MF$(IntermediateDirectory)/Core_MsuPdu.c$(DependSuffix) -MM "/cygwin/home/MSU_MidRange/Core/MsuPdu.c"

$(IntermediateDirectory)/Core_MsuPdu.c$(PreprocessSuffix): /cygwin/home/MSU_MidRange/Core/MsuPdu.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Core_MsuPdu.c$(PreprocessSuffix) "/cygwin/home/MSU_MidRange/Core/MsuPdu.c"

$(IntermediateDirectory)/Core_MsuSocket.c$(ObjectSuffix): /cygwin/home/MSU_MidRange/Core/MsuSocket.c $(IntermediateDirectory)/Core_MsuSocket.c$(DependSuffix)
	$(CC) $(SourceSwitch) "D:/cygwin/home/MSU_MidRange/Core/MsuSocket.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Core_MsuSocket.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Core_MsuSocket.c$(DependSuffix): /cygwin/home/MSU_MidRange/Core/MsuSocket.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Core_MsuSocket.c$(ObjectSuffix) -MF$(IntermediateDirectory)/Core_MsuSocket.c$(DependSuffix) -MM "/cygwin/home/MSU_MidRange/Core/MsuSocket.c"

$(IntermediateDirectory)/Core_MsuSocket.c$(PreprocessSuffix): /cygwin/home/MSU_MidRange/Core/MsuSocket.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Core_MsuSocket.c$(PreprocessSuffix) "/cygwin/home/MSU_MidRange/Core/MsuSocket.c"

$(IntermediateDirectory)/Interface_MsuFileInterface.c$(ObjectSuffix): /cygwin/home/MSU_MidRange/Interface/MsuFileInterface.c $(IntermediateDirectory)/Interface_MsuFileInterface.c$(DependSuffix)
	$(CC) $(SourceSwitch) "D:/cygwin/home/MSU_MidRange/Interface/MsuFileInterface.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Interface_MsuFileInterface.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Interface_MsuFileInterface.c$(DependSuffix): /cygwin/home/MSU_MidRange/Interface/MsuFileInterface.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Interface_MsuFileInterface.c$(ObjectSuffix) -MF$(IntermediateDirectory)/Interface_MsuFileInterface.c$(DependSuffix) -MM "/cygwin/home/MSU_MidRange/Interface/MsuFileInterface.c"

$(IntermediateDirectory)/Interface_MsuFileInterface.c$(PreprocessSuffix): /cygwin/home/MSU_MidRange/Interface/MsuFileInterface.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Interface_MsuFileInterface.c$(PreprocessSuffix) "/cygwin/home/MSU_MidRange/Interface/MsuFileInterface.c"

$(IntermediateDirectory)/Interface_MsuPlatformInterface.c$(ObjectSuffix): /cygwin/home/MSU_MidRange/Interface/MsuPlatformInterface.c $(IntermediateDirectory)/Interface_MsuPlatformInterface.c$(DependSuffix)
	$(CC) $(SourceSwitch) "D:/cygwin/home/MSU_MidRange/Interface/MsuPlatformInterface.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Interface_MsuPlatformInterface.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Interface_MsuPlatformInterface.c$(DependSuffix): /cygwin/home/MSU_MidRange/Interface/MsuPlatformInterface.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Interface_MsuPlatformInterface.c$(ObjectSuffix) -MF$(IntermediateDirectory)/Interface_MsuPlatformInterface.c$(DependSuffix) -MM "/cygwin/home/MSU_MidRange/Interface/MsuPlatformInterface.c"

$(IntermediateDirectory)/Interface_MsuPlatformInterface.c$(PreprocessSuffix): /cygwin/home/MSU_MidRange/Interface/MsuPlatformInterface.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Interface_MsuPlatformInterface.c$(PreprocessSuffix) "/cygwin/home/MSU_MidRange/Interface/MsuPlatformInterface.c"

$(IntermediateDirectory)/Interface_MsuSocketInterface.c$(ObjectSuffix): /cygwin/home/MSU_MidRange/Interface/MsuSocketInterface.c $(IntermediateDirectory)/Interface_MsuSocketInterface.c$(DependSuffix)
	$(CC) $(SourceSwitch) "D:/cygwin/home/MSU_MidRange/Interface/MsuSocketInterface.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Interface_MsuSocketInterface.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Interface_MsuSocketInterface.c$(DependSuffix): /cygwin/home/MSU_MidRange/Interface/MsuSocketInterface.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Interface_MsuSocketInterface.c$(ObjectSuffix) -MF$(IntermediateDirectory)/Interface_MsuSocketInterface.c$(DependSuffix) -MM "/cygwin/home/MSU_MidRange/Interface/MsuSocketInterface.c"

$(IntermediateDirectory)/Interface_MsuSocketInterface.c$(PreprocessSuffix): /cygwin/home/MSU_MidRange/Interface/MsuSocketInterface.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Interface_MsuSocketInterface.c$(PreprocessSuffix) "/cygwin/home/MSU_MidRange/Interface/MsuSocketInterface.c"

$(IntermediateDirectory)/MSU_MidRange_MsuMain.c$(ObjectSuffix): /cygwin/home/MSU_MidRange/MsuMain.c $(IntermediateDirectory)/MSU_MidRange_MsuMain.c$(DependSuffix)
	$(CC) $(SourceSwitch) "D:/cygwin/home/MSU_MidRange/MsuMain.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/MSU_MidRange_MsuMain.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/MSU_MidRange_MsuMain.c$(DependSuffix): /cygwin/home/MSU_MidRange/MsuMain.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/MSU_MidRange_MsuMain.c$(ObjectSuffix) -MF$(IntermediateDirectory)/MSU_MidRange_MsuMain.c$(DependSuffix) -MM "/cygwin/home/MSU_MidRange/MsuMain.c"

$(IntermediateDirectory)/MSU_MidRange_MsuMain.c$(PreprocessSuffix): /cygwin/home/MSU_MidRange/MsuMain.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/MSU_MidRange_MsuMain.c$(PreprocessSuffix) "/cygwin/home/MSU_MidRange/MsuMain.c"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Release/


