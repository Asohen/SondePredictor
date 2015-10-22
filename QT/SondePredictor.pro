#-------------------------------------------------
#
# Project created by QtCreator 2015-06-13T18:25:38
#
#-------------------------------------------------

QT	+= core gui
QT	+= serialport
#QT	+= positioning

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET		= SondePredictor
TEMPLATE	= app
#CONFIG		+= mobility
#MOBILITY	+= systeminfo


SOURCES +=	main.cpp\
			mainwindow.cpp \
	Execution.cpp \
	LivePrediction.cpp \
	GpsReader.cpp \
	GpxWriter.cpp \
	KmlWriter.cpp \
	Nmea.cpp \
	Position.cpp \
	PredictionTrackReader.cpp \
	SondeTrackReader.cpp \
	XShiftFilter.cpp \
	Parameter.cpp \
	Version.cpp

HEADERS  +=	mainwindow.h \
	Execution.h \
	LivePrediction.h \
	GpsReader.h \
	GpxWriter.h \
	KmlWriter.h \
	Nmea.h \
	Position.h \
	PredictionTrackReader.h \
	SondeTrackReader.h \
	XShiftFilter.h \
	Parameter.h \
	Version.h

FORMS	+= mainwindow.ui
