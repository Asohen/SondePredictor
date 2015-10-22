#ifndef LivePrediction_H
#define LivePrediction_H

#include <QTextStream>
#include <QFile>
#include <QSerialPort>

#include "Position.h"
#include "PredictionTrackReader.h"
#include "SondeTrackReader.h"
#include "GpxWriter.h"
#include "KmlWriter.h"
#include "Nmea.h"
#include "XShiftFilter.h"


class Execution;


class LivePrediction
{
	// constants ----------------------------------------------------------------------------------
	public:
		static const QString	NORTH;
		static const QString	SOUTH;
		static const QString	EAST;
		static const QString	WEST;

	// types --------------------------------------------------------------------------------------
	private:

	// variables ----------------------------------------------------------------------------------
	public:
		QTextStream				ResultStream;
		Position				LandingPosition;			// own prediction
		unsigned long			AltVeloRatioState;
		SondeTrackReader		SondeTrack;
		Position				CurrentPosition;			// current position from SondeMonitor

	private:
		Execution*				Executer;
		PredictionTrackReader	PredictionTrack;
		GpxWriter				Gpx;
		KmlWriter				Kml;
		bool					StartUp;

		struct PredictionT									// prediction from http://predict.habhub.org
		{
			Position	BurstPosition;						// burst position
			Position	LandingPosition;					// touch down position
		};

		PredictionT	Prediction;								// prediction from http://predict.habhub.org
		Position	PredictedPosition;						// predicted position from SondeMonitor, corresponding to current position
		long		NextCurrentTime;						// next time of CurrentPosition when new collection is complete
		long		NextWaitingTime;						// next waiting time of SondeTrack
		double		AltitudeVelocityRatio;					// height velocity ratio
		bool		AltitudeVelocityRatioValid;
		Nmea		NmeaOutput;
		XShiftFilter	FilteredAltVeloRatio;
		double			StartAltitude;

	// functions ----------------------------------------------------------------------------------
	public:
		LivePrediction
			(
				Execution*		execution,
				QFile&			predictionFile,
				QFile&			sondeTrackFile,
				QFile&			resultFile,
				QFile&			resultGpxFile,
				QFile&			resultKmlFile,
				QSerialPort&	nmeaPort
			);
		bool	Prepare();									// preparation of Work()
		bool	Work();										// one prediction step, must be repeated endless

	private:
		void	WriteAltitude
			(
				const char*	text,
				float		value
			);
		void	WritePosition
			(
				const char*	text,
				Position	position
			);
		void	CalculateLandingPosition();					// calculate new prediction of landing position
		bool	TimeCorrection								// applies vertival velocity
			(
				double&	factor								// out:	altitude velocity ratio
			);
		double	ReducedCompensationFactor					// TimeCorrection: factor for reduced compensation
			(
				double	altitudeRatio
			);
};

#endif
