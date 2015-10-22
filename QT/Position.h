#ifndef Position_H
#define Position_H

class Position
{
	// constants ----------------------------------------------------------------------------------
	public:
		static const double	DEGREE_TO_RADIANS	= 0.0174532925;
		enum Phase
		{
			PHASE_STARTUP,
			PHASE_ASCENT,
			PHASE_DESCENT
		};

	// variables ----------------------------------------------------------------------------------
	public:
		bool	Valid;
		double	Time;										// duration since start [s]
		double	Latitude;									// WGS 84
		double	Longitude;									// WGS 84
		double	Altitude;									// altitude [m]
		double	AltVelo;									// altitude velocity [m/s]
		bool	AltVeloValid;								// AltVelo is valid
		bool	AltVeloInAverage;							// altitude velocity added to average

	// functions ----------------------------------------------------------------------------------
	public:
		Position();

		static double DegreeToDeltaMeter					// calculate difference [m] from latitude/longitude [°]
			(
				double	latitude1Degree,					// in:	latitude #1 [°]
				double	latitude2Degree,					// in:	latitude #2 [°]
				double	longitude1Degree,					// in:	longitude #1 [°]
				double	longitude2Degree,					// in:	longitude #2 [°]
				double&	latitudeMeter,						// out:	latitude difference #1 - #2 [m]
				double&	longitudeMeter						// out:	longitude difference #1 - #2 [m]
			);
		static void MeterToDegree							// calculate latitude/longitude [°] from distance [m]
			(
				double	latitudeMeter,						// in:	latitude [m]
				double	longitudeMeter,						// in:	longitude [m]
				double	roughlyLatitude,					// in:	roughly latitude
				double&	latitudeDegree,						// out:	latitude [°]
				double&	longitudeDegree						// out:	longitude [°]
			);
};

#endif
