#include <math.h>
#include <QDebug>
#include <limits>
#include "PredictionTrackReader.h"
#include "Parameter.h"


const QString	PredictionTrackReader::SEPARATOR	= ",";


//-------------------------------------------------------------------------------------------------
PredictionTrackReader::PredictionTrackReader
	(
		QFile&	inputFile
	)
	:	InputFile(inputFile)
{
	BurstIndex	= 0;
	StartTime	= 0;
}

//-------------------------------------------------------------------------------------------------
bool PredictionTrackReader::ReadAll()
{
	bool		result	= true;
	bool		start	= true;
	Position	position;

	while(GetNextPosition(position, start))					// read all positions
	{
		start = false;
		AddZVelocity(position);
		Prediction.append(position);
	}

	if(Prediction.size() < 3)
	{
		result = false;
	}

	return result;
}

//-------------------------------------------------------------------------------------------------
void PredictionTrackReader::AddZVelocity(Position& position)
{
	if(Prediction.size() == 0)								// no prediction yet
	{
		position.AltVeloValid = false;						// not able to calculate velocity
	}
	else													// at least one entry present
	{
		unsigned long index = Prediction.size() - 1;
		long	time1	= Prediction.at(index).Time;
		long	time2	= position.Time;
		double	height1	= Prediction.at(index).Altitude;
		double	height2	= position.Altitude;

		long dTime = time2 - time1;
		if(dTime >= 1)										// avoid division by zero
		{
			position.AltVelo = (height2 - height1) / dTime;	// v [m/s]
			position.AltVeloValid = true;
		}
		else												// end of file
		{
			position.AltVeloValid = false;					// not able to calculate velocity
		}
	}
}

//-------------------------------------------------------------------------------------------------
bool PredictionTrackReader::GetNextPosition(Position& position, bool start)
{
	bool result = false;
	QString	text = InputFile.readLine();					// try to read one more line

	if(!text.isEmpty())										// data available
	{
		QStringList elements = text.split(SEPARATOR);

		if(elements.size() >= 4)
		{
			long long time		= elements[0].toLongLong();
			if(start)
			{
				StartTime = time;
			}
			position.Time		= time - StartTime;
			position.Latitude	= elements[1].toDouble();
			position.Longitude	= elements[2].toDouble();
			position.Altitude	= elements[3].toDouble();

			result = true;
		} //else lack of data
	} //else end of file

	return result;
}

//-------------------------------------------------------------------------------------------------
bool PredictionTrackReader::GetPredictedBurstPosition
	(
		Position&	burstPosition
	)
{
	if(Prediction.size() >= 1)
	{
		burstPosition.Altitude = -std::numeric_limits<double>::max();	// initialize for maximum calculation

		for(int i = 0; i < Prediction.size(); i++)			// all entries
		{
			double height = Prediction.at(i).Altitude;
			if(height > burstPosition.Altitude)				// new maximum
			{
				burstPosition = Prediction.at(i);
				BurstIndex = i;
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

//-------------------------------------------------------------------------------------------------
bool PredictionTrackReader::GetPredictedLandingPosition
	(
		Position&	landingPosition
	)
{
	if(Prediction.size() >= 1)
	{
		landingPosition = Prediction.at(Prediction.size() - 1);
		return true;
	}
	else
	{
		return false;
	}
}

//-------------------------------------------------------------------------------------------------
bool PredictionTrackReader::GetPredictedCurrentPosition
	(
		Position&		PredictedPosition,
		Position::Phase	currentPhase,
		double			currentAltitude
	)
{
	bool result = false;

	if(currentAltitude <= Prediction.at(BurstIndex).Altitude)			// current altitude in predicted range
	{
		if(currentPhase == Position::PHASE_DESCENT)
		{
			for(int i = BurstIndex; i < Prediction.size(); i++)			// from burst to end
			{
				if(Prediction.at(i).Altitude <= currentAltitude)		// found it
				{
					double part = (currentAltitude - Prediction.at(i-1).Altitude) / (Prediction.at(i).Altitude - Prediction.at(i-1).Altitude);

					PredictedPosition.Altitude		= currentAltitude;
					PredictedPosition.Latitude		= Interpolation(Prediction.at(i-1).Latitude,	Prediction.at(i).Latitude,	part);
					PredictedPosition.Longitude		= Interpolation(Prediction.at(i-1).Longitude,	Prediction.at(i).Longitude,	part);
					PredictedPosition.Time			= Interpolation(Prediction.at(i-1).Time,		Prediction.at(i).Time,		part);
					PredictedPosition.AltVelo		= Interpolation(Prediction.at(i-1).AltVelo,		Prediction.at(i).AltVelo,	part);
					PredictedPosition.AltVeloValid	= Prediction.at(i-1).AltVeloValid && Prediction.at(i).AltVeloValid;
					result = true;

					break;												// completed
				}
			}
		}
	}

	return result;
}

//-------------------------------------------------------------------------------------------------
double PredictionTrackReader::Interpolation
	(
		double	start,
		double	end,
		double	part
	)
{
	return start + part * (end - start);
}
