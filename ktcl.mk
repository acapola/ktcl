##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=ktcl
ConfigurationName      :=Debug
WorkspacePath          := "C:\Users\sebastien\Documents\dev\ktcl\ktcl_windows"
ProjectPath            := "C:\Users\sebastien\Documents\dev\ktcl"
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=sebastien
Date                   :=11/29/14
CodeLitePath           :="C:\Program Files (x86)\CodeLite"
LinkerName             :=C:/MinGW-4.8.1/bin/g++.exe 
SharedObjectLinkerName :=C:/MinGW-4.8.1/bin/g++.exe -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=$(PreprocessorSwitch)HAVE_UNISTD_H 
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="ktcl.txt"
PCHCompileFlags        :=
MakeDirCommand         :=makedir
RcCmpOptions           := 
RcCompilerName         :=C:/MinGW-4.8.1/bin/windres.exe 
LinkOptions            :=  $(LINKER_OPT)
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)$(TCL_ROOT)/include $(IncludeSwitch)common 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)$(TCL_LIB) 
ArLibs                 :=  "$(TCL_LIB)" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)$(TCL_ROOT)/lib 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := C:/MinGW-4.8.1/bin/ar.exe rcu
CXX      := C:/MinGW-4.8.1/bin/g++.exe 
CC       := C:/MinGW-4.8.1/bin/gcc.exe 
CXXFLAGS :=  -Wmain -Wfatal-errors -g -O0 -std=c++11 -Wall $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := C:/MinGW-4.8.1/bin/as.exe 


##
## User defined environment variables
##
CodeLiteDir:=C:\Program Files (x86)\CodeLite
UNIT_TEST_PP_SRC_DIR:=C:\UnitTest++-1.3
TCL_ROOT:=c:/tcl32
TCL_LIB:=tcl86
LINKER_OPT:=-static
Objects0=$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IntermediateDirectory)/dat_analysis_raw_to_text.cpp$(ObjectSuffix) $(IntermediateDirectory)/tclchannelstreambuf.cpp$(ObjectSuffix) $(IntermediateDirectory)/common_archive.cpp$(ObjectSuffix) $(IntermediateDirectory)/common_error_msg.cpp$(ObjectSuffix) $(IntermediateDirectory)/common_numconv_stringstream.cpp$(ObjectSuffix) $(IntermediateDirectory)/common_string_ext.cpp$(ObjectSuffix) 



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
	@$(MakeDirCommand) "./Debug"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/main.cpp$(ObjectSuffix): main.cpp $(IntermediateDirectory)/main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/sebastien/Documents/dev/ktcl/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main.cpp$(DependSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/main.cpp$(DependSuffix) -MM "main.cpp"

$(IntermediateDirectory)/main.cpp$(PreprocessSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main.cpp$(PreprocessSuffix) "main.cpp"

$(IntermediateDirectory)/dat_analysis_raw_to_text.cpp$(ObjectSuffix): dat_analysis/raw_to_text.cpp $(IntermediateDirectory)/dat_analysis_raw_to_text.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/sebastien/Documents/dev/ktcl/dat_analysis/raw_to_text.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/dat_analysis_raw_to_text.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/dat_analysis_raw_to_text.cpp$(DependSuffix): dat_analysis/raw_to_text.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/dat_analysis_raw_to_text.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/dat_analysis_raw_to_text.cpp$(DependSuffix) -MM "dat_analysis/raw_to_text.cpp"

$(IntermediateDirectory)/dat_analysis_raw_to_text.cpp$(PreprocessSuffix): dat_analysis/raw_to_text.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/dat_analysis_raw_to_text.cpp$(PreprocessSuffix) "dat_analysis/raw_to_text.cpp"

$(IntermediateDirectory)/tclchannelstreambuf.cpp$(ObjectSuffix): tclchannelstreambuf.cpp $(IntermediateDirectory)/tclchannelstreambuf.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/sebastien/Documents/dev/ktcl/tclchannelstreambuf.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/tclchannelstreambuf.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/tclchannelstreambuf.cpp$(DependSuffix): tclchannelstreambuf.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/tclchannelstreambuf.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/tclchannelstreambuf.cpp$(DependSuffix) -MM "tclchannelstreambuf.cpp"

$(IntermediateDirectory)/tclchannelstreambuf.cpp$(PreprocessSuffix): tclchannelstreambuf.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/tclchannelstreambuf.cpp$(PreprocessSuffix) "tclchannelstreambuf.cpp"

$(IntermediateDirectory)/common_archive.cpp$(ObjectSuffix): common/archive.cpp $(IntermediateDirectory)/common_archive.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/sebastien/Documents/dev/ktcl/common/archive.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/common_archive.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/common_archive.cpp$(DependSuffix): common/archive.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/common_archive.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/common_archive.cpp$(DependSuffix) -MM "common/archive.cpp"

$(IntermediateDirectory)/common_archive.cpp$(PreprocessSuffix): common/archive.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/common_archive.cpp$(PreprocessSuffix) "common/archive.cpp"

$(IntermediateDirectory)/common_error_msg.cpp$(ObjectSuffix): common/error_msg.cpp $(IntermediateDirectory)/common_error_msg.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/sebastien/Documents/dev/ktcl/common/error_msg.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/common_error_msg.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/common_error_msg.cpp$(DependSuffix): common/error_msg.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/common_error_msg.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/common_error_msg.cpp$(DependSuffix) -MM "common/error_msg.cpp"

$(IntermediateDirectory)/common_error_msg.cpp$(PreprocessSuffix): common/error_msg.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/common_error_msg.cpp$(PreprocessSuffix) "common/error_msg.cpp"

$(IntermediateDirectory)/common_numconv_stringstream.cpp$(ObjectSuffix): common/numconv_stringstream.cpp $(IntermediateDirectory)/common_numconv_stringstream.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/sebastien/Documents/dev/ktcl/common/numconv_stringstream.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/common_numconv_stringstream.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/common_numconv_stringstream.cpp$(DependSuffix): common/numconv_stringstream.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/common_numconv_stringstream.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/common_numconv_stringstream.cpp$(DependSuffix) -MM "common/numconv_stringstream.cpp"

$(IntermediateDirectory)/common_numconv_stringstream.cpp$(PreprocessSuffix): common/numconv_stringstream.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/common_numconv_stringstream.cpp$(PreprocessSuffix) "common/numconv_stringstream.cpp"

$(IntermediateDirectory)/common_string_ext.cpp$(ObjectSuffix): common/string_ext.cpp $(IntermediateDirectory)/common_string_ext.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/sebastien/Documents/dev/ktcl/common/string_ext.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/common_string_ext.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/common_string_ext.cpp$(DependSuffix): common/string_ext.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/common_string_ext.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/common_string_ext.cpp$(DependSuffix) -MM "common/string_ext.cpp"

$(IntermediateDirectory)/common_string_ext.cpp$(PreprocessSuffix): common/string_ext.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/common_string_ext.cpp$(PreprocessSuffix) "common/string_ext.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) ./Debug/*$(ObjectSuffix)
	$(RM) ./Debug/*$(DependSuffix)
	$(RM) $(OutputFile)
	$(RM) $(OutputFile).exe
	$(RM) "ktcl_windows/.build-debug/ktcl"


