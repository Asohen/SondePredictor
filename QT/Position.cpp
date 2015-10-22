#include <QtCore/qmath.h>

#include "Position.h"


//-------------------------------------------------------------------------------------------------
Position::Position()
{
	Valid				= false;
	Time				= 0;
	Latitude			= 0;
	Longitude			= 0;
	Altitude			= 0;
	AltVelo				= 0;
	AltVeloValid		= false;
	AltVeloInAverage	= false;
}

//-------------------------------------------------------------------------------------------------
// http://www.kompf.de/gps/distcalc.html
double Position::DegreeToDeltaMeter
	(
		double	latitude1Degree,
		double	latitude2Degree,
		double	longitude1Degree,
		double	longitude2Degree,
		double&	latitudeMeter,
		double&	longitudeMeter
	)
{
	double averageLatitude = (latitude1Degree + latitude2Degree) / 2.;
	latitudeMeter	= 111300. * (latitude1Degree - latitude2Degree);
	longitudeMeter	= 111300. * qCos(averageLatitude * DEGREE_TO_RADIANS) * (longitude1Degree - longitude2Degree);
	return averageLatitude;
}

//-------------------------------------------------------------------------------------------------
void Position::MeterToDegree
	(
		double	latitudeMeter,
		double	longitudeMeter,
		double	roughlyLatitude,
		double&	latitudeDegree,
		double&	longitudeDegree
	)
{
	latitudeDegree	= latitudeMeter  / 111300.;
	longitudeDegree	= longitudeMeter / (111300. * qCos(roughlyLatitude * DEGREE_TO_RADIANS));
}
