######################################################################
# Automatically generated by qmake (2.01a) Thu Jul 26 12:07:34 2012
######################################################################

TEMPLATE = app
TARGET = bamtools-toolkit
DEPENDPATH += . \
              toolkit 


INCLUDEPATH += . \
              toolkit \
               ../../thirdparty/bamtools

CONFIG   += console release
QT       -= gui

OBJECTS_DIR = GeneratedFiles
UI_DIR      = GeneratedFiles
MOC_DIR     = GeneratedFiles
RCC_DIR     = GeneratedFiles

LIBS           += -lm -lz ../../thirdparty/bamtools/libbamtools.a

# Input
HEADERS += toolkit/bamtools_convert.h \
           toolkit/bamtools_count.h \
           toolkit/bamtools_coverage.h \
           toolkit/bamtools_filter.h \
           toolkit/bamtools_header.h \
           toolkit/bamtools_index.h \
           toolkit/bamtools_merge.h \
           toolkit/bamtools_random.h \
           toolkit/bamtools_resolve.h \
           toolkit/bamtools_revert.h \
           toolkit/bamtools_sort.h \
           toolkit/bamtools_split.h \
           toolkit/bamtools_stats.h \
           toolkit/bamtools_tool.h \
           toolkit/bamtools_version.h 


SOURCES += toolkit/bamtools.cpp \
           toolkit/bamtools_convert.cpp \
           toolkit/bamtools_count.cpp \
           toolkit/bamtools_coverage.cpp \
           toolkit/bamtools_filter.cpp \
           toolkit/bamtools_header.cpp \
           toolkit/bamtools_index.cpp \
           toolkit/bamtools_merge.cpp \
           toolkit/bamtools_random.cpp \
           toolkit/bamtools_resolve.cpp \
           toolkit/bamtools_revert.cpp \
           toolkit/bamtools_sort.cpp \
           toolkit/bamtools_split.cpp \
           toolkit/bamtools_stats.cpp 



