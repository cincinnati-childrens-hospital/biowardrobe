######################################################################
# Automatically generated by qmake (2.01a) Thu Jul 26 12:07:34 2012
######################################################################

TEMPLATE = lib
TARGET = bamtools
DEPENDPATH += . \
              api \
              jsoncpp \
              utils \
              Win \
              api/algorithms \
              api/internal/bam \
              api/internal/index \
              api/internal/io \
              api/internal/sam \
              api/internal/utils

#              toolkit \


INCLUDEPATH += . \
              api \
              jsoncpp \
              utils \
              Win 

#              toolkit 

CONFIG   += staticlib
QT       -= gui

unix{
OBJECTS_DIR = GeneratedFiles
UI_DIR      = GeneratedFiles
MOC_DIR     = GeneratedFiles
RCC_DIR     = GeneratedFiles
}

# Input
HEADERS += api/api_global.h \
           api/BamAlgorithms.h \
           api/BamAlignment.h \
           api/BamAux.h \
           api/BamConstants.h \
           api/BamIndex.h \
           api/BamMultiReader.h \
           api/BamReader.h \
           api/BamWriter.h \
           api/IBamIODevice.h \
           api/SamConstants.h \
           api/SamHeader.h \
           api/SamProgram.h \
           api/SamProgramChain.h \
           api/SamReadGroup.h \
           api/SamReadGroupDictionary.h \
           api/SamSequence.h \
           api/SamSequenceDictionary.h \
           jsoncpp/json.h \
           jsoncpp/json_batchallocator.h \
           jsoncpp/json_config.h \
           jsoncpp/json_features.h \
           jsoncpp/json_forwards.h \
           jsoncpp/json_reader.h \
           jsoncpp/json_tool.h \
           jsoncpp/json_value.h \
           jsoncpp/json_writer.h \
           utils/bamtools_fasta.h \
           utils/bamtools_filter_engine.h \
           utils/bamtools_filter_properties.h \
           utils/bamtools_filter_ruleparser.h \
           utils/bamtools_options.h \
           utils/bamtools_pileup_engine.h \
           utils/bamtools_utilities.h \
           utils/bamtools_variant.h \
           utils/utils_global.h \
           api/algorithms/Sort.h \
           api/internal/bam/BamHeader_p.h \
           api/internal/bam/BamMultiMerger_p.h \
           api/internal/bam/BamMultiReader_p.h \
           api/internal/bam/BamRandomAccessController_p.h \
           api/internal/bam/BamReader_p.h \
           api/internal/bam/BamWriter_p.h \
           api/internal/index/BamIndexFactory_p.h \
           api/internal/index/BamStandardIndex_p.h \
           api/internal/index/BamToolsIndex_p.h \
           api/internal/io/BamDeviceFactory_p.h \
           api/internal/io/BamFile_p.h \
           api/internal/io/BamFtp_p.h \
           api/internal/io/BamHttp_p.h \
           api/internal/io/BamPipe_p.h \
           api/internal/io/BgzfStream_p.h \
           api/internal/io/ByteArray_p.h \
           api/internal/io/HostAddress_p.h \
           api/internal/io/HostInfo_p.h \
           api/internal/io/HttpHeader_p.h \
           api/internal/io/ILocalIODevice_p.h \
           api/internal/io/RollingBuffer_p.h \
           api/internal/io/TcpSocket_p.h \
           api/internal/io/TcpSocketEngine_p.h \
           api/internal/sam/SamFormatParser_p.h \
           api/internal/sam/SamFormatPrinter_p.h \
           api/internal/sam/SamHeaderValidator_p.h \
           api/internal/sam/SamHeaderVersion_p.h \
           api/internal/utils/BamException_p.h \
           jsoncpp/json_internalarray.inl \
           jsoncpp/json_internalmap.inl \
           jsoncpp/json_valueiterator.inl

SOURCES += api/BamAlignment.cpp \
           api/BamMultiReader.cpp \
           api/BamReader.cpp \
           api/BamWriter.cpp \
           api/SamHeader.cpp \
           api/SamProgram.cpp \
           api/SamProgramChain.cpp \
           api/SamReadGroup.cpp \
           api/SamReadGroupDictionary.cpp \
           api/SamSequence.cpp \
           api/SamSequenceDictionary.cpp \
           jsoncpp/json_reader.cpp \
           jsoncpp/json_value.cpp \
           jsoncpp/json_writer.cpp \
           utils/bamtools_fasta.cpp \
           utils/bamtools_options.cpp \
           utils/bamtools_pileup_engine.cpp \
           utils/bamtools_utilities.cpp \
           api/internal/bam/BamHeader_p.cpp \
           api/internal/bam/BamMultiReader_p.cpp \
           api/internal/bam/BamRandomAccessController_p.cpp \
           api/internal/bam/BamReader_p.cpp \
           api/internal/bam/BamWriter_p.cpp \
           api/internal/index/BamIndexFactory_p.cpp \
           api/internal/index/BamStandardIndex_p.cpp \
           api/internal/index/BamToolsIndex_p.cpp \
           api/internal/io/BamDeviceFactory_p.cpp \
           api/internal/io/BamFile_p.cpp \
           api/internal/io/BamFtp_p.cpp \
           api/internal/io/BamHttp_p.cpp \
           api/internal/io/BamPipe_p.cpp \
           api/internal/io/BgzfStream_p.cpp \
           api/internal/io/ByteArray_p.cpp \
           api/internal/io/HostAddress_p.cpp \
           api/internal/io/HostInfo_p.cpp \
           api/internal/io/HttpHeader_p.cpp \
           api/internal/io/ILocalIODevice_p.cpp \
           api/internal/io/RollingBuffer_p.cpp \
           api/internal/io/TcpSocket_p.cpp \
           api/internal/io/TcpSocketEngine_p.cpp \
           api/internal/sam/SamFormatParser_p.cpp \
           api/internal/sam/SamFormatPrinter_p.cpp \
           api/internal/sam/SamHeaderValidator_p.cpp \
           api/internal/utils/BamException_p.cpp

win32 {

SOURCES += api/internal/io/TcpSocketEngine_win_p.cpp 
HEADERS += api/internal/io/NetWin_p.h \
           Win/zconf.h \
           Win/zlib.h 
 
}
           
!win32{

SOURCES += api/internal/io/TcpSocketEngine_unix_p.cpp
HEADERS += api/internal/io/NetUnix_p.h

}

QMAKE_CLEAN += lib$${TARGET}.* logfile.log *~  ../global/*~
