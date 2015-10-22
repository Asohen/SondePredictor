#ifndef Execution_H
#define Execution_H

#include <iostream>
using std::endl;
#include <QObject>
#include <QString>
#include <QFile>
#include <QDirIterator>
#include <QDateTime>
#include <QSerialPort>

#include "Position.h"
#include "GpsReader.h"

class LivePrediction;


class Execution : public QObject
{
	Q_OBJECT

	// constants ----------------------------------------------------------------------------------
	public:
		static const int		MAX_PARAMETER_COUNT					= 3;
		static const QString	SEPARATOR;
		static const int		GROUNDTRACK_FILE_NAME_LENGTH_1		= 23;	// e.g. groundtrack2014091012Z_
		static const int		GROUNDTRACK_FILE_NAME_LENGTH_1_M10	= 24;	// e.g. digitalsonde2014120818Z_

	private:
		static const QString	GROUND_TRACK_FILE_NAME_PREFIX;
		static const QString	GROUND_TRACK_FILE_NAME_PREFIX_M10;
		static const QString	GROUND_TRACK_FILE_NAME_SUFFIX;
		static const QString	GROUND_TRACK_FILE_NAME_SUFFIX_M10;
		static const QString	LIVE_PREDICTION_RESULT_FILE_NAME_EXTENSION;
		static const QString	LIVE_PREDICTION_GPX_FILE_NAME_EXTENSION;
//		static const QString	LIVE_PREDICTION_KML_FILE_NAME_EXTENSION;
		static const QString	LIVE_PREDICTION_KML_FILE_NAME;
		static const QString	LIVE_PREDICTION_FILE_NAME_PREFIX;

	// variables ----------------------------------------------------------------------------------
	public:
		QFile	PredictionFile;
		QFile	GroundTrackFile;
		QFile	LivePredictionFile;
		QFile	LivePredictionGpxFile;
		QFile	LivePredictionKmlFile;
		QSerialPort		NmeaOutPort;						// live prediction output
		QSerialPort		NmeaInPort;							// GPS position input
		LivePrediction*	Live;
		bool			StatusOk;
		GpsReader		GpsRead;

	private:
		QString	GroundTrackPrefix;
		QString NmeaOutputName;
		bool	NmeaOutputApplied;
		int		ArgC;
		char**	ArgV;

	// functions ----------------------------------------------------------------------------------
	public:
		Execution(QObject *parent = 0);
		virtual ~Execution();

		void Start
			(
				int argc,
				char *argv[]
			);

	private:
		void Start2();
		void WriteLine();
		void WriteParameters();
		void OpenFile
			(
				const char*				errorText,
				const QString&			fileName,
				QIODevice::OpenModeFlag	fileMode,
				QFile&					file
			);
		void CloseFile
			(
				QFile& file
			);
		void CreateNmeaInput					// initiate and open NmeaPort
			(
				char*		nmeaInputName		// in:	name of NMEA output, e.g. COM4
			);
		void CreateNmeaOutput					// initiate and open NmeaPort
			(
				char*		nmeaOutputName		// in:	name of NMEA output, e.g. COM5
			);
		void CreatePredictionFileName
			(
				QString& directory,
				QString& prediction
			);
		bool CreateGroundTrackFileName
			(
				QString& directory,
				QString& groundTrack
			);
		void CreateLivePredictionFileName
			(
				QString& prediction,
				QString& groundTrack,
				QString& livePrediction,
				const QString& extension
			);
		bool OpenAllFiles
			(	unsigned long	parameterCount,
				char*	NmeaOutputName,
				char*	directoryName,
				char*	predictionFileName,
				char*	groundTrackFileName
			);
		bool AllFilesOpen();
		void CloseAllFiles();
		void WriteHeader();
		void WriteFilenames();
		QString OpenPrediction
			(
				QString	directory,
				QString	fileName
			);
		void PrepareUi();
		void CalculateVector
			(	double lat1, double lon1,
				double lat2, double lon2,
				double& distance, double& direction
			);

	// signals, slots
	signals:
		void Phase			(const Position::Phase		phase);
		void GpsOk			(const GpsReader::GPS_OK	gpsOk);

		void HideNormal		(const bool		hide);
		void HideError		(const bool		hide);

		void SondeDistance	(const QString	distance);
		void SondeDirection	(const QString	direction);
		void Distance		(const QString	distance);
		void Direction		(const QString	direction);

		void SondeLanding	(const QString	text);
		void Altitude		(const QString	text);
		void AscentDescent	(const QString	text);
		void Duration		(const QString	text);
		void Latitude		(const QString	text);
		void LatitudeSign	(const QString	text);
		void Longitude		(const QString	text);
		void LongitudeSign	(const QString	text);
		void Rate			(const QString	text);
		void RatePercent	(const QString	text);

		void ErrorText1	(const QString	text);
		void ErrorText2	(const QString	text);
		void ErrorText3	(const QString	text);
		void ErrorText4	(const QString	text);
		void ErrorText5	(const QString	text);
		void ErrorText6	(const QString	text);
		void ErrorText7	(const QString	text);
		void ErrorText8	(const QString	text);
		void ErrorText9	(const QString	text);

	public slots:
		void GpsUpdate(GpsReader::GPS_OK gpsOk, double latitude, double longitude);

	private slots:
		void Start1();
		void Work();
};

#endif

