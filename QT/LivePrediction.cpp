#include <QDebug>
#include <QtCore/qmath.h>
#include <limits>
#include <iostream>
using std::endl;
#include <cmath>				// lround

#include "LivePrediction.h"
#include "Parameter.h"
#include "Execution.h"


const QString	LivePrediction::NORTH	= "N";
const QString	LivePrediction::SOUTH	= "S";
const QString	LivePrediction::EAST	= "E";
const QString	LivePrediction::WEST	= "W";


//-------------------------------------------------------------------------------------------------
LivePrediction::LivePrediction
	(
		Execution*		execution,
		QFile&			predictionFile,
		QFile&			sondeTrackFile,
		QFile&			resultFile,
		QFile&			resultGpxFile,
		QFile&			resultKmlFile,
		QSerialPort&	nmeaPort
	)
	:	SondeTrack			(sondeTrackFile, ResultStream),
		Executer			(execution),
		PredictionTrack		(predictionFile),
		Gpx					(resultGpxFile),
		Kml					(resultKmlFile),
		NmeaOutput			(nmeaPort),
		FilteredAltVeloRatio(Parameter::ALT_VELO_RATIO_X_FILTER[0])
{
	StartUp						= true;
	ResultStream.setDevice(&resultFile);
	NextCurrentTime				= 0;
	NextWaitingTime				= 0;
	AltitudeVelocityRatio		= 1;
	AltitudeVelocityRatioValid	= false;
	AltVeloRatioState			= 0;
	StartAltitude				= 0;
}

//-------------------------------------------------------------------------------------------------
void LivePrediction::WriteAltitude(const char* text, float value)
{
	ResultStream		<< text
						<< lround(value)	<< "m"
						<< endl;
	ResultStream.flush();
}

//-------------------------------------------------------------------------------------------------
void LivePrediction::WritePosition(const char* text, Position position)
{
	ResultStream		<< text
						<< "lat "		<< position.Latitude
						<< ", lon "		<< position.Longitude
						<< " after "	<< lround(position.Time / 60.) << " minutes"
						<< endl;
	ResultStream.flush();
}

//-------------------------------------------------------------------------------------------------
bool LivePrediction::Prepare()
{
	bool result = false;

	if(PredictionTrack.ReadAll())
	{
		if(PredictionTrack.GetPredictedBurstPosition(Prediction.BurstPosition))
		{
			WriteAltitude("Predicted burst altitude   by CUSF (predict.habhub.org): ", Prediction.BurstPosition.Altitude);

			if(PredictionTrack.GetPredictedLandingPosition(Prediction.LandingPosition))
			{
				WritePosition("Predicted landing position by CUSF (predict.habhub.org): ", Prediction.LandingPosition);
				result = true;
			}
			else
			{
				emit Executer->ErrorText1("No landing position in CUSF prediction file");
			}
		}
		else
		{
			emit Executer->ErrorText1("No burst in CUSF prediction file");
		}
	}
	else
	{
		emit Executer->ErrorText1("Improper CUSF prediction file");
	}

	return result;
}

//-------------------------------------------------------------------------------------------------
bool LivePrediction::Work()
{
	if(SondeTrack.NewCurrentPosition(CurrentPosition))
	{
		if(StartUp)
		{
			StartUp = false;
		}
		StartAltitude	= Prediction.BurstPosition.Altitude < SondeTrack.ActualBurstAltitude
						? Prediction.BurstPosition.Altitude : SondeTrack.ActualBurstAltitude;
		StartAltitude -= Parameter::ALTITUDE_ADAPTION;

		if(PredictionTrack.GetPredictedCurrentPosition
				(PredictedPosition, SondeTrack.CurrentPhase, CurrentPosition.Altitude)
			)
		{
			CalculateLandingPosition();
		}
		else													// current height above highest height of prediction
		{
			QTextStream(stdout)	<< "    At altitude " << lround(CurrentPosition.Altitude) << "m: "
								<< "no usable prediction available" << endl;
			QTextStream(stdout).flush();
		}
	} //else status output is done by SondeTrack.NewCurrentPosition

	emit Executer->Phase(SondeTrack.CurrentPhase);

	if(SondeTrack.CurrentPhase == Position::PHASE_ASCENT)
	{
		emit Executer->SondeLanding("Sonde");
		emit Executer->Altitude		(QString::number(lround(CurrentPosition.Altitude)));
		emit Executer->LatitudeSign	(CurrentPosition.Latitude >= 0 ? NORTH : SOUTH);
		emit Executer->Latitude		(QString::number(CurrentPosition.Latitude));
		emit Executer->LongitudeSign(CurrentPosition.Longitude >= 0 ? EAST : WEST);
		emit Executer->Longitude	(QString::number(CurrentPosition.Longitude));
	}

	return !SondeTrack.EndOfData;
}

//-------------------------------------------------------------------------------------------------
// LandingPosition = PredictedLandingPosition + (CurrentPosition - PredictedCurrentPosition)
void LivePrediction::CalculateLandingPosition()
{
	// deviation of current position
	double latitudeMeter;
	double longitudeMeter;
	double averageLatitude = Position::DegreeToDeltaMeter
		(	CurrentPosition.Latitude,	PredictedPosition.Latitude,
			CurrentPosition.Longitude,	PredictedPosition.Longitude,
			latitudeMeter, longitudeMeter
		);

	if(SondeTrack.CurrentPhase == Position::PHASE_DESCENT)
	{
		double correctionFactor = 1. + CurrentPosition.Altitude / SondeTrack.ActualBurstAltitude;

		longitudeMeter	= longitudeMeter * correctionFactor;
		latitudeMeter	= latitudeMeter  * correctionFactor;
//		double distance	= qSqrt(latitudeMeter*latitudeMeter + longitudeMeter*longitudeMeter);
		double latitudeDegree;
		double longitudeDegree;
		Position::MeterToDegree(latitudeMeter, longitudeMeter, averageLatitude, latitudeDegree, longitudeDegree);

		LandingPosition.Latitude	= Prediction.LandingPosition.Latitude  + latitudeDegree;
		LandingPosition.Longitude	= Prediction.LandingPosition.Longitude + longitudeDegree;

		double timeFactor;

		if(		TimeCorrection(timeFactor)
			&&	lround(CurrentPosition.AltVelo) < 0
			)
		{
			long timeToLanding = (Prediction.LandingPosition.Time - PredictedPosition.Time + 30.)/60.;
			timeToLanding = timeToLanding * timeFactor;

	//		double	angle = qAtan2(longitudeMeter, latitudeMeter) * 57.2957795;
	//		if(angle < 0)
	//		{
	//			angle = angle + 360;
	//		}

			ResultStream << lround(CurrentPosition.Altitude)	<< "m";
			emit Executer->Altitude(QString::number(lround(CurrentPosition.Altitude)));

			if(CurrentPosition.AltVeloValid)
			{
				ResultStream << ", " << lround(CurrentPosition.AltVelo)	<< "m/s";
				emit Executer->Rate(QString::number(lround(CurrentPosition.AltVelo)));
			}

			if(AltitudeVelocityRatioValid)
			{
				ResultStream << " = " << lround(AltitudeVelocityRatio * 100.)	<< "%";
				emit Executer->RatePercent(QString::number(lround(AltitudeVelocityRatio * 100.)));
			}

			ResultStream << ", ";
			ResultStream
	//			<< "correction " << lround(distance/1000)		<< "km " << lround(angle) << "°, "
				<< "landing in " << timeToLanding				<< " minutes "
				<< "at lat/lon " << LandingPosition.Latitude	<< " " << LandingPosition.Longitude
				<< endl;

			emit Executer->SondeLanding("Landing");
			emit Executer->Duration		(QString::number(lround(timeToLanding)));
			emit Executer->LatitudeSign	(LandingPosition.Latitude >= 0 ? NORTH : SOUTH);
			emit Executer->Latitude		(QString::number(LandingPosition.Latitude));
			emit Executer->LongitudeSign(LandingPosition.Longitude >= 0 ? EAST : WEST);
			emit Executer->Longitude	(QString::number(LandingPosition.Longitude));
			ResultStream.flush();

			Gpx.AddGpxWaypoint
				(	LandingPosition.Latitude, LandingPosition.Longitude,
					CurrentPosition.Altitude, CurrentPosition.AltVeloValid, CurrentPosition.AltVelo,
					timeToLanding
				);
			Kml.AddKmlWaypoint
				(	LandingPosition.Latitude, LandingPosition.Longitude,
					CurrentPosition.Altitude, CurrentPosition.AltVeloValid, CurrentPosition.AltVelo,
					timeToLanding
				);
			NmeaOutput.SendPosition(LandingPosition);

//			QTextStream(stdout) << "*** At altitude " << lround(CurrentPosition.Altitude) << "m: "
//								<< "new live prediction added to file" << endl;
//			QTextStream(stdout).flush();
		}
	}
}

//-------------------------------------------------------------------------------------------------
bool LivePrediction::TimeCorrection(double& factor)
{
	bool	result = false;
	double	predictedZVelocity = PredictedPosition.AltVelo;

	AltitudeVelocityRatioValid	= false;
	factor = 1;

	if(CurrentPosition.Altitude <= StartAltitude)
	{
		if(		predictedZVelocity		< 0.
			&&	CurrentPosition.AltVelo	< 0.
			&&	CurrentPosition.AltVeloValid
			)
		{
			double altitudeVelocityRatio = CurrentPosition.AltVelo / predictedZVelocity;
			result = FilteredAltVeloRatio.GetNewFilteredValue(CurrentPosition.Time, altitudeVelocityRatio);
			AltitudeVelocityRatio		= FilteredAltVeloRatio.FilteredValue();
			AltitudeVelocityRatioValid	= result;

			if(AltitudeVelocityRatioValid)
			{
				if(AltitudeVelocityRatio == 1.)									// matching
				{
					factor = 1;
				}
				else
				if(AltitudeVelocityRatio < 1.)									// slower
				{
					if(AltitudeVelocityRatio < Parameter::MIN_V_ALT_RATIO_SLOW)
					{
						AltitudeVelocityRatio = Parameter::MIN_V_ALT_RATIO_SLOW;
					}

					if(AltitudeVelocityRatio <= Parameter::V_ALT_RATIO_SLOW)	// full compensation
					{
						factor = 1. / sqrt(AltitudeVelocityRatio);
					}
					else														// reduced compensation
					{
						factor = 1. / sqrt(ReducedCompensationFactor(Parameter::V_ALT_RATIO_SLOW));
					}
				}
				else															// faster
				{
					if(AltitudeVelocityRatio >= Parameter::V_ALT_RATIO_FAST)	// full compensation
					{
						factor = 1. / AltitudeVelocityRatio;
					}
					else														// reduced compensation
					{
						factor = 1. / ReducedCompensationFactor(Parameter::V_ALT_RATIO_FAST);
					}
				}

				double latitudeDistance		= LandingPosition.Latitude  - CurrentPosition.Latitude;
				double longitudeDistance	= LandingPosition.Longitude - CurrentPosition.Longitude;
				LandingPosition.Latitude	= LandingPosition.Latitude  + latitudeDistance  * (factor - 1.);
				LandingPosition.Longitude	= LandingPosition.Longitude + longitudeDistance * (factor - 1.);
				LandingPosition.Valid		= true;

				switch(AltVeloRatioState)
				{
					case 0:
						AltVeloRatioState = 1;
						FilteredAltVeloRatio.Configure(Parameter::ALT_VELO_RATIO_X_FILTER[AltVeloRatioState]);
						break;
					case 1:
						AltVeloRatioState = 2;
						FilteredAltVeloRatio.Configure(Parameter::ALT_VELO_RATIO_X_FILTER[AltVeloRatioState]);
						break;
					case 2:
						AltVeloRatioState = 3;
						FilteredAltVeloRatio.Configure(Parameter::ALT_VELO_RATIO_X_FILTER[AltVeloRatioState]);
						break;
					default:									// leave it as is
						break;
				}
			}
			else
			{
				QTextStream(stdout) << "    At altitude " << lround(CurrentPosition.Altitude) << "m: "
									<< "altitude velocity ratio discarded"
									<< " (nominal count = " << FilteredAltVeloRatio.ValueCount()
									<< "/"					<< FilteredAltVeloRatio.NominalCount()
									<< ", minimum count = " << FilteredAltVeloRatio.SumCount()
									<< "/"					<< FilteredAltVeloRatio.MinimumCount()
									<< ")" << endl;
				QTextStream(stdout).flush();
			}
		}
		else
		{
			QTextStream(stdout)	<< "    At altitude " << lround(CurrentPosition.Altitude) << "m: "
								<< "no usable altitude velocity ratio available";
			if(CurrentPosition.AltVeloValid)
			{
				QTextStream(stdout)	<< " (alt. velo = " << CurrentPosition.AltVelo << ")" << endl;
			}
			else
			{
				QTextStream(stdout)	<< " (alt. velo invalid)" << endl;
			}
			QTextStream(stdout).flush();
		}
	}
//	else
//	{
//		QTextStream(stdout)	<< "    At altitude " << lround(CurrentPosition.Altitude) << "m: "
//							<< "waiting for start altitude " << StartAltitude << "m" << endl;
//		QTextStream(stdout).flush();
//	}

	return result;
}

//-------------------------------------------------------------------------------------------------
double LivePrediction::ReducedCompensationFactor(double altitudeRatio)
{
	double deviation = AltitudeVelocityRatio - 1.;
	double factor1 = deviation / (altitudeRatio - 1.);
	return 1. + factor1 * deviation;
}
