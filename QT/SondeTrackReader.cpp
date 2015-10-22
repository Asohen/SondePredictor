#include <QDebug>
#include <windows.h>										// Sleep
#include <limits>
#include <cmath>

#include "SondeTrackReader.h"


const QString	SondeTrackReader::SEPARATOR	= " ";


//-------------------------------------------------------------------------------------------------
SondeTrackReader::SondeTrackReader
	(
		QFile&			inputFile,
		QTextStream&	resultStream
	)
	:	InputFile(inputFile),
		InputFileInfo(inputFile.fileName()),
		ResultStream(resultStream),
		FilteredAltVelo(Parameter::ALT_VELO_FILTER)
{
	CurrentPhase			= Position::PHASE_STARTUP;
	ActualBurstAltitude		= -std::numeric_limits<double>::max();		// initialize for maximum calculation
	WaitingTime				= 0;
	DescentEvent			= false;
	EndOfData				= false;

	PhaseChange				= false;
	PhaseChangeAltitude		= 0;

	NewAltitude				= 0;
	Altitude				= 0;
	LastTime				= 0;
	LastAltitude			= 0;
	LastLatitude			= 0;
	LastLongitude			= 0;
	LastAltVelo				= 0;
	CompareTime				= 0;
	CompareAlt				= 0;
	CompareLat				= 0;
	CompareLon				= 0;
	CompareAltVelo			= 0;
	StartUpCompareCount		= 0;

	InputFilePosition		= 0;
	StartTime				= Parameter::MIN_TIME;
	LineFormat				= FORMAT_NOT_DETECTED;
	TimeOfFirstLine			= 0;
	LineTime				= 0;

	InputFileInfo.setCaching(false);
}

//-------------------------------------------------------------------------------------------------
bool SondeTrackReader::NewCurrentPosition(Position& position)
{
	bool result = false;

	InputFile.open(QIODevice::ReadOnly);
	InputFile.seek(InputFilePosition);

	bool available = true;
	while(available)
	{
		char	character;									// one character to be read
		qint64	count = InputFile.read(&character, sizeof(char));

		switch(count)
		{
			case 0:											// currently no new position available
			{
//				QTextStream(stdout) << "    Waiting for next ground track position" << endl;
//				QTextStream(stdout).flush();
				available = false;
				EndOfData = true;
				break;
			}

			case -1:										// error
				available = false;
				EndOfData = true;
				break;

			default:										// more text available
			{
				EndOfData = false;
				if(character == '\n')						// end of line
				{
					result = EvaluateLine(position);
					Text.clear();
					LineTime += SECONDS_PER_LINE;
					available = false;						// return with completed line
				}
				else										// line nor completed yet
				{
					Text.append(character);
				}
				InputFilePosition ++;
				break;
			}
		};
	}

	InputFile.close();										// nothing to do now any more
	return result;
}

//-------------------------------------------------------------------------------------------------
void SondeTrackReader::DetectFormat(QStringList& group1, QStringList& group2)
{
	LineFormat = FORMAT_UNKNOWN;							// init.

	if(		group1.size() >= 2								// date & time
		&&	group2.size() >= 3
		)
	{
		QString date = group1[0];
		QString time = group1[1];
		QStringList dateElements = date.split("-", QString::SkipEmptyParts);
		QStringList timeElements = time.split(":", QString::SkipEmptyParts);
		if(		dateElements.size() == 3
			&&	timeElements.size() == 3
			)
		{
			TimeOfFirstLine = GetTimeFromDateTime(date, time);
			if(TimeOfFirstLine == INVALID_TIME)
			{
				LineFormat = FORMAT_NOT_DETECTED;
			}
			else
			{
				LineFormat = FORMAT_DFM06;
			}
		}
	}
	else
	if(		group1.size() >= 8
		&&	group2.size() == 0
		&&	group1[2] == "lat"
		)
	{
		QString day		= group1[0];
		QString time	= group1[1];
		LineFormat = FORMAT_M10;
		DayOfFirstLine	= day;
		TimeOfFirstLine = GetTimeFromDayTime(day, time);
	}
	else
	if(		group1.size() >= 4
		&&	group2.size() == 0
		)
	{
		LineFormat = FORMAT_RS92;
		TimeOfFirstLine = 0;
	}

	ShowGroundTrackMode();
}

//-------------------------------------------------------------------------------------------------
bool SondeTrackReader::GetDataFields(Position& position, QStringList& group1, QStringList& group2)
{
	bool result = false;

	switch(LineFormat)
	{
		case FORMAT_DFM06:
			if(		group1.size() >= 2
				&&	group2.size() >= 3
				)
			{
				long time = GetTimeFromDateTime(group1[0], group1[1]);
				if(time != INVALID_TIME)
				{
					position.Time		= time - TimeOfFirstLine;
					position.Latitude	= group2[0].toDouble();
					position.Longitude	= group2[1].toDouble();
					position.Altitude	= group2[2].toDouble();
					position.AltVeloValid = false;
					result = true;
				}
			}
			break;

		case FORMAT_RS92:
			if(group1.size() >= 4)
			{
				position.Time		= group1[0].toLongLong();
				position.Latitude	= group1[1].toDouble();
				position.Longitude	= group1[2].toDouble();
				position.Altitude	= group1[3].toDouble();
				position.AltVeloValid = false;
				result = true;
			}
			break;

		case FORMAT_M10:
			{
				long time = GetTimeFromDayTime(group1[0], group1[1]);
				if(		time != INVALID_TIME
					&&	group1[2] == "lat"
					&&	group1[4] == "lon"
					&&	group1[6] == "alt"
					&&	group1[14] == "vert"
					)
				{
					position.Time		= time - TimeOfFirstLine;
					position.Latitude	= group1[3].toDouble();
					position.Longitude	= group1[5].toDouble();
					position.Altitude	= group1[7].toDouble();
					QString vert = group1[15];
					vert = vert.left(vert.indexOf("m/s"));
					position.AltVelo	= vert.toDouble();
					position.AltVeloValid = true;
					result = true;
				}
				break;
			}

		default:
			break;
	}

	return result;
}

//-------------------------------------------------------------------------------------------------
bool SondeTrackReader::DataFieldsOk(Position& position)
{
	bool result =	position.Time		>= 0		&&	position.Time		<= 36000
				&&	position.Latitude	>= -90		&&	position.Latitude	<= 90
				&&	position.Longitude	>= -180		&&	position.Longitude	<= 180
				&&	position.Altitude	>= -1000	&&	position.Altitude	<= 50000;
	position.Valid = result;

	if(position.AltVeloValid)
	{
		result =	result
				||	(	position.AltVelo	>=	-100	&&	position.AltVelo <= 100	);
	}

	return result;
}

//-------------------------------------------------------------------------------------------------
long SondeTrackReader::GetTimeFromDateTime(QString& date, QString& time)
{
	long timeValue;
	QString format = "yyyy-MM-dd hh:mm:ss.zzz";
	QString text = date + " " + time;
	QDateTime dateTime = QDateTime::fromString(text, format);
	if(dateTime.isValid())
	{
		timeValue = dateTime.toTime_t();
	}
	else
	{
		timeValue = INVALID_TIME;
	}

	return timeValue;
}

//-------------------------------------------------------------------------------------------------
long SondeTrackReader::GetTimeFromDayTime(QString& day, QString& time)
{
	long timeValue;
	QString format = "hh:mm:ss.zzz";
	QDateTime dateTime = QDateTime::fromString(time, format);
	if(dateTime.isValid())
	{
		QDate date;
		date.setDate(2000,1,1);			// must be greater than 1970 (UNIX)
		dateTime.setDate(date);
		if(day == DayOfFirstLine)
		{
			timeValue = dateTime.toTime_t();				// [s]
		}
		else
		{
			timeValue = dateTime.toTime_t() + 3600*24;		// [s]
		}
	}
	else
	{
		timeValue = INVALID_TIME;
	}

	return timeValue;
}

//-------------------------------------------------------------------------------------------------
bool SondeTrackReader::EvaluateLine(Position& currentPosition)
{
	bool result = false;

	QStringList groups = Text.split(",", QString::SkipEmptyParts);
	QStringList group1;
	QStringList group2;
	if(groups.size() >= 1)
	{
		group1	= groups[0].split(SEPARATOR, QString::SkipEmptyParts);
	}
	if(groups.size() >= 2)
	{
		group2	= groups[1].split(SEPARATOR, QString::SkipEmptyParts);
	}

	if(LineFormat == FORMAT_NOT_DETECTED)
	{
		DetectFormat(group1, group2);
	}

	if(		LineFormat != FORMAT_NOT_DETECTED
		&&	LineFormat != FORMAT_UNKNOWN
		)
	{
		Position rawPosition;
		if(GetDataFields(rawPosition, group1, group2))
		{
			if(DataFieldsOk(rawPosition))
			{
				if(GetPosition(rawPosition, currentPosition))
				{
					if(Altitude >= ActualBurstAltitude)						// ascending
					{
						ActualBurstAltitude = Altitude;
						PhaseChange = false;

						if(CurrentPhase == Position::PHASE_DESCENT)
						{
							QTextStream(stdout) << "    At altitude " << lround(Altitude) << "m: "
												<< "ascent after descent" << endl;
							QTextStream(stdout).flush();
						}

						CurrentPhase = Position::PHASE_ASCENT;
						DescentEvent = false;
					}
					else													// descending
					{
						if(!PhaseChange)									// phase change starts
						{
							PhaseChange = true;
							PhaseChangeAltitude = Altitude;
						}
						else												// phase change runs
						if(PhaseChangeAltitude - Altitude >= Parameter::PHASE_CHANGE_ALT_DELTA)
						{
							if(CurrentPhase != Position::PHASE_DESCENT)
							{
								CurrentPhase = Position::PHASE_DESCENT;
								DescentEvent = true;

								QTextStream(stdout) << "!!! Begin of descent, altitude " << Altitude << "m" << endl;
								QTextStream(stdout).flush();
							}
							PhaseChange = false;
						}
					}

					result = StartReached();								// delay after burst
				} //else messages by GetPosition
			}
			else
			{
				QTextStream(stdout) << "    Invalid raw ground track data discarded" << endl;
				QTextStream(stdout).flush();
			}
		}
		else
		{
			QTextStream(stdout) << "    Invalid raw ground track time discarded" << endl;
			QTextStream(stdout).flush();
		}
	}
	else
	{
		QTextStream(stdout) << "    Invalid raw ground track position discarded" << endl;
		QTextStream(stdout).flush();
	}

	return result;
}

//-------------------------------------------------------------------------------------------------
bool SondeTrackReader::DeltaInRange
	(
		double			delta,
		double			value,
		double&			compareValue,
		const QString&	text,
		const double	absoluteTolerance
	)
{
	bool result = false;

	double shall = absoluteTolerance;
	result = fabs(delta) <= shall;

	if(result)
	{
		double tolerance = absoluteTolerance / Parameter::COMPARE_PT1_FACTOR;
		double shall1 = compareValue - tolerance;
		double shall2 = compareValue + tolerance;

		result = value >= shall1 && value <= shall2;

//		QTextStream(stdout)	<< " delta=" << delta
//							<< " value=" << value
//							<< " compareValue=" << compareValue
//							<< " compareDiff=" << compareDiff
//							<< " tolerance=" << tolerance
//							<< " shall1=" << shall1
//							<< " shall2=" << shall2
//							<< " ToleranceFactor=" << ToleranceFactor
//							<< endl;

		if(!result)
		{
			QTextStream(stdout)	<< "    At altitude " << lround(NewAltitude) << "m: "
								<< "inexact ground track " << text << " discarded"
								<< " (" << value << " shall ";
			if(shall1 <= shall2)
			{
				QTextStream(stdout)	<< shall1 << "..." << shall2;
			}
			else
			{
				QTextStream(stdout)	<< shall2 << "..." << shall1;
			}
			QTextStream(stdout)	<< ")" << endl;
			QTextStream(stdout).flush();
		}

		compareValue = compareValue	+ (value - compareValue) * Parameter::COMPARE_PT1_FACTOR;
	}
	else
	{
		QTextStream(stdout)	<< "    At altitude " << lround(NewAltitude) << "m: "
							<< "inexact ground track " << text << " delta discarded"
							<< " (" << fabs(delta) << " shall <= " << shall << ")" << endl;
		QTextStream(stdout).flush();
	}

	return result;
}

//-------------------------------------------------------------------------------------------------
bool SondeTrackReader::GetPosition(Position& rawPosition, Position& newPosition)
{
	bool result = false;

	NewAltitude = rawPosition.Altitude;

	double timeDelta	= rawPosition.Time      - LastTime;
	double altDelta		= rawPosition.Altitude  - LastAltitude;
	double latDelta		= rawPosition.Latitude  - LastLatitude;
	double lonDelta		= rawPosition.Longitude - LastLongitude;
	double altVelo;											// altitude velocity [m/s]

	if(rawPosition.AltVeloValid)							// directly from sonde
	{
		altVelo = rawPosition.AltVelo;
	}
	else
	{
		altVelo = timeDelta != 0 ? altDelta / timeDelta : LastAltVelo;
	}

	double altVeloDelta	= altVelo - LastAltVelo;

	if(StartUpCompareCount < Parameter::START_UP_IGNORE_COUNT)
	{
		StartUpCompareCount ++;
	}
	else
	if(StartUpCompareCount < Parameter::START_UP_AVERAGE_COUNT)
	{
		CompareTime		+= rawPosition.Time;
		CompareAlt		+= rawPosition.Altitude;
		CompareLat		+= rawPosition.Latitude;
		CompareLon		+= rawPosition.Longitude;
		CompareAltVelo	+= altVelo;

		StartUpCompareCount ++;
	}

	if(StartUpCompareCount == Parameter::START_UP_AVERAGE_COUNT)
	{
		double count = static_cast<double>(Parameter::START_UP_AVERAGE_COUNT - Parameter::START_UP_IGNORE_COUNT);
		CompareTime		= CompareTime		/ count;
		CompareAlt		= CompareAlt		/ count;
		CompareLat		= CompareLat		/ count;
		CompareLon		= CompareLon		/ count;
		CompareAltVelo	= CompareAltVelo	/ count;

		StartUpCompareCount ++;								// last incrementation
	}

	if(StartUpCompareCount > Parameter::START_UP_AVERAGE_COUNT)
	{
		if(DeltaInRange(timeDelta,		rawPosition.Time,		CompareTime,"time",		Parameter::TIME_TOLERANCE))
		if(DeltaInRange(altDelta,		rawPosition.Altitude,	CompareAlt,	"altitude",	Parameter::ALTITUDE_TOLERANCE))
		if(DeltaInRange(latDelta,		rawPosition.Latitude,	CompareLat,	"latitude",	Param.LatTolerance))
		if(DeltaInRange(lonDelta,		rawPosition.Longitude,	CompareLon,	"longitude",Param.LonTolerance))
		if(DeltaInRange(altVeloDelta,	altVelo,			CompareAltVelo,	"altVelo",	Parameter::ALT_VELO_TOLERANCE))
		{
			double filteredAltVelo;
			if(CalculateAltitudeVelocity(rawPosition, altVelo, filteredAltVelo))
			{
				newPosition = rawPosition;
				newPosition.AltVelo			= filteredAltVelo;
				newPosition.AltVeloValid	= true;

				Altitude = rawPosition.Altitude;
				result = true;

			}
		} //else message by DeltaInRange
	} //else start up

	LastTime			= rawPosition.Time;
	LastAltitude		= rawPosition.Altitude;
	LastLatitude		= rawPosition.Latitude;
	LastLongitude		= rawPosition.Longitude;
	LastAltVelo			= altVelo;

	return result;
}

//-------------------------------------------------------------------------------------------------
bool SondeTrackReader::CalculateAltitudeVelocity(Position& currentPosition, double rawAltVelo, double& filteredAltVelo)
{
	bool result = true;

	result = FilteredAltVelo.GetNewFilteredValue(currentPosition.Time, rawAltVelo);
	filteredAltVelo = FilteredAltVelo.FilteredValue();

	if(!result)
	{
		QTextStream(stdout) << "    At altitude " << lround(currentPosition.Altitude) << "m: ";

		if(CurrentPhase == Position::PHASE_DESCENT)
		{
			QTextStream(stdout) << "waiting for ground track position completion"
								<< " (nominal count = " << FilteredAltVelo.ValueCount()
								<< "/"					<< FilteredAltVelo.NominalCount()
								<< ", minimum count = " << FilteredAltVelo.SumCount()
								<< "/"					<< FilteredAltVelo.MinimumCount()
								<< ")" << endl;
		}
		else
		{
			QTextStream(stdout) << "waiting for descent (ground track position completion)" << endl;
		}
		QTextStream(stdout).flush();
	}

	return result;
}

//-------------------------------------------------------------------------------------------------
bool SondeTrackReader::StartReached()
{
	bool result = false;

	if(CurrentPhase == Position::PHASE_DESCENT)
	{
		if(DescentEvent)
		{
			StartTime = LineTime + Parameter::MIN_TIME;
			DescentEvent = false;
		}

		if(LineTime >= StartTime)
		{
			result = true;
		}
		else
		{
//			qint64 remainingTime = StartTime - LineTime;
//			QTextStream(stdout)	<< "    At altitude " << lround(Altitude) << "m: "
//								<< "waiting for start, " << remainingTime << "s remaining" << endl;
//			QTextStream(stdout).flush();
		}
	}
	else
	{
//		QTextStream(stdout)	<< "    At altitude " << lround(Altitude) << "m: "
//							<< "waiting for descent" << endl;
//		QTextStream(stdout).flush();
	}

	return result;
}

//-------------------------------------------------------------------------------------------------
void SondeTrackReader::ShowGroundTrackMode()
{
	QString lineFormat;
	switch(LineFormat)
	{
		case FORMAT_DFM06:	lineFormat = "DFM06";	break;
		case FORMAT_RS92:	lineFormat = "RS92";	break;
		case FORMAT_M10:	lineFormat = "M10";		break;

		case FORMAT_NOT_DETECTED:
		case FORMAT_UNKNOWN:
		default:			lineFormat = "";		break;
	}

	if(!lineFormat.isEmpty())
	{
		ResultStream << "Detected ground track file sonde type: " << lineFormat << endl;
		ResultStream << "--------------------------------------------------------" << endl;
		ResultStream << endl;
		ResultStream << "LivePrediction:" << endl;
		ResultStream.flush();
	}
}
