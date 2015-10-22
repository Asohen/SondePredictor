#include <QDebug>

#include "Parameter.h"
#include "Position.h"


const XShiftFilter::ConfigurationT	Parameter::ALT_VELO_FILTER =
	{
		20, 20, 10
	};

const XShiftFilter::ConfigurationT	Parameter::ALT_VELO_RATIO_X_FILTER [4] =
	{
		10,		10,		5,				// 1st prediction
		20,		20,		10,				// 2nd prediction
		50,		50,		25,				// 3rd prediction
		100,	100,	50				// 4th and all following predictions
	};


//-------------------------------------------------------------------------------------------------
Parameter::Parameter()
{
	Position::MeterToDegree
	(
		LAT_LON_TOLERANCE,
		LAT_LON_TOLERANCE,
		ROUGHLY_LATITUDE,
		LatTolerance,										// indepentent from latitude
		LonTolerance										// depends on ROUGHLY_LATITUDE
	);
}
