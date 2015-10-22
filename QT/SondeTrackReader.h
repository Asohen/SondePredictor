#ifndef SondeTrackReader_H
#define SondeTrackReader_H

#include <QTextStream>
#include <QFile>
#include <QFileInfo>
#include <QDateTime>
#include <QVector>

#include "Position.h"
#include "XShiftFilter.h"
#include "Parameter.h"


class SondeTrackReader
{
	// constants ----------------------------------------------------------------------------------
	private:
		static const QString		SEPARATOR;
		static const long			INVALID_TIME		= -1;
		static const unsigned long	SECONDS_PER_LINE	= 1;

		enum Format
		{
			FORMAT_NOT_DETECTED,
			FORMAT_UNKNOWN,
			FORMAT_DFM06,				// date, time
			FORMAT_RS92,				// seconds
			FORMAT_M10					// day time
		};

	// types --------------------------------------------------------------------------------------
	private:
		class AltitudeVelocity
		{
			public:
				unsigned long	StartIndex;
				unsigned long	EndIndex;
				unsigned long	Count;
				double			Sum;
			public:
				AltitudeVelocity()
				{
					StartIndex	= 0;
					EndIndex	= 0;
					Count		= 0;
					Sum			= 0;
				}
		};

	// variables ----------------------------------------------------------------------------------
	public:
		Position::Phase	CurrentPhase;
		double			ActualBurstAltitude;				// actual burst height [m], valid if PHASE_DESCENT
		long			WaitingTime;						// duration [s] waiting for next position
		bool			DescentEvent;						// descent starts
		bool			EndOfData;							// input file was read completely

	private:
		QString			Text;
		QFile&			InputFile;							// input file from SondeMonitor
		QFileInfo		InputFileInfo;						// file system informations of InputFile
		bool			PhaseChange;						// phase may change from ASCENT to DESCENT
		double			PhaseChangeAltitude;				// altitude when descent seems to start
		double			NewAltitude;						// new altitude   [m], for message
		double			Altitude;							// last valid altitude [m]

		double			LastTime;							// last time      [s]
		double			LastAltitude;						// last/current altitude [m]
		double			LastLatitude;						// last latitude  [°]
		double			LastLongitude;						// last longitude [°]
		double			LastAltVelo;						// last altitude velocity [m/s]
		double			CompareTime;						// last time      [s]
		double			CompareAlt;							// last altitude  [m]
		double			CompareLat;							// last latitude  [°]
		double			CompareLon;							// last longitude [°]
		double			CompareAltVelo;						// last alt. velo [s]
		unsigned long	StartUpCompareCount;				// number of compare values for start up average

		qint64			InputFilePosition;					// file position when closing intermediate
		unsigned long	StartTime;							// time [s] (relative to file start) when to start prediction
		Format			LineFormat;
		long			TimeOfFirstLine;					// time [s] of file start, since 1970-01-01T00:00:00
		QString			DayOfFirstLine;
		QTextStream&	ResultStream;
		unsigned long	LineTime;							// time [s] since file start
		XShiftFilter	FilteredAltVelo;
		Parameter		Param;

	// functions ----------------------------------------------------------------------------------
	public:
		SondeTrackReader
			(
				QFile&			inputFile,					// input file
				QTextStream&	resultStream

			);
		bool	NewCurrentPosition							// get new current position
			(												// return: new current position available
				Position&	position						// out:	new current position
			);

	private:
		bool GetPosition									// test if position can be used
			(												// return:	position OK
				Position&	rawPosition,
				Position&	newPosition
			);
		bool CalculateAltitudeVelocity						// calculate filtered altitude velocity
			(												// result: filteredAltVelo is OK
				Position&	currentPosition,				// current position
				double		rawAltVelo,						// in:	raw altitude velocity
				double&		filteredAltVelo					// out:	filtered altitude velocity
			);
		bool EvaluateLine									// evaluate one line of new current position
			(
				Position&	currentPosition
			);
		bool StartReached();
		bool GetDataFields
			(
				Position&		position,					// out:	data fields
				QStringList&	group1,						// in:	line elements, left of comma
				QStringList&	group2						// in:	line elements, right of comma
			);
		void DetectFormat
			(
				QStringList&	group1,						// in:	line elements, left of comma
				QStringList&	group2						// in:	line elements, right of comma
			);
		long GetTimeFromDateTime
			(
				QString&	date,
				QString&	time
			);
		long GetTimeFromDayTime
			(
				QString&	day,
				QString&	time
			);
		void ShowGroundTrackMode();
		bool DeltaInRange
			(
				double			delta,
				double			value,
				double&			compareValue,
				const QString&	text,
				const double	absoluteTolerance
			);
		bool DataFieldsOk
			(
				Position&	position
			);
};

#endif
