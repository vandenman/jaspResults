TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

RHOME = $$system(R-4.1.0 RHOME)
message("R_HOME is" $${RHOME})
RPackagesDir = renv/library/R-4.1/x86_64-pc-linux-gnu/
message("RPackagesDir is" $${RPackagesDir})

INCLUDEPATH += $${RHOME}/include/
INCLUDEPATH += $${RPackagesDir}Rcpp/include/
INCLUDEPATH += src/jaspColumnEncoder/

SOURCES += \
        centertext.cpp \
        main.cpp \
        src/RcppExports.cpp \
        src/centertext.cpp \
        src/columnencoder.cpp \
        src/columntype.cpp \
        src/jaspColumn.cpp \
        src/jaspColumnEncoder/columnencoder.cpp \
        src/jaspColumnEncoder/columntype.cpp \
        src/jaspContainer.cpp \
        src/jaspHtml.cpp \
        src/jaspJson.cpp \
        src/jaspObject.cpp \
        src/jaspPlot.cpp \
        src/jaspPrintOptions.cpp \
        src/jaspQmlSource.cpp \
        src/jaspResults.cpp \
        src/jaspState.cpp \
        src/jaspTable.cpp

DISTFILES += \
	R/zzaLoadModule.R \
	R/zzzWrappers.R \
	src/Makevars \
	src/jaspColumnEncoder/README \
	src/jaspResults.so

HEADERS += \
	R/writeImage.R \
	centertext.h \
	src/centertext.h \
	src/jaspColumn.h \
	src/jaspColumnEncoder/columnencoder.h \
	src/jaspColumnEncoder/columntype.h \
	src/jaspColumnEncoder/enumutilities.h \
	src/jaspColumnEncoder/stringutils.h \
	src/jaspContainer.h \
	src/jaspHtml.h \
	src/jaspJson.h \
	src/jaspList.h \
	src/jaspModuleRegistration.h \
	src/jaspObject.h \
	src/jaspPlot.h \
	src/jaspPrintOptions.h \
	src/jaspQmlSource.h \
	src/jaspResults.h \
	src/jaspState.h \
	src/jaspTable.h
