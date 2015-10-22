#ifndef Parameter_H
#define Parameter_H

#include "XShiftFilter.h"


class Parameter
{
	// types --------------------------------------------------------------------------------------
	public:

	// constants ----------------------------------------------------------------------------------
	public:
		static const long			MIN_TIME						= 0;		// min. time [s] of groundtrack file before processing starts
		static const unsigned long	PAUSE_DURATION					= 1;		// loop pause duration [s]
		static const double			MIN_V_ALT_RATIO_SLOW			= 0.1;		// minimum of altitude velocity ratio
		static const double			V_ALT_RATIO_SLOW				= 0.7;		// altitude velocity ratio for TimeCorrection, slow descent
		static const double			V_ALT_RATIO_FAST				= 1.4;		// altitude velocity ratio for TimeCorrection, fast descent
		static const double			PHASE_CHANGE_ALT_DELTA			= 200;		// max. expected difference [m] of lat/lon/height
		static const double			COMPARE_PT1_FACTOR				= 0.1;		// filter for compare value to discard inexact values
		static const double			ALTITUDE_ADAPTION				= 2000;		// [m] altitude delta before prediction start

		static const double			TIME_TOLERANCE					= 5.1;		// [s]
		static const double			ALTITUDE_TOLERANCE				= 100;		// [m]
		static const double			ALT_VELO_TOLERANCE				= 100;		// [m/s]
		static const double			LAT_LON_TOLERANCE				= 100;		// [m]
		static const double			ROUGHLY_LATITUDE				= 51.5;		// [°]
		static const unsigned long	START_UP_IGNORE_COUNT			= 30;		// number of ignored values during start up
		static const unsigned long	START_UP_AVERAGE_COUNT			= 33;		// number of compare values for start up average, including START_UP_IGNORE_COUNT

		static const XShiftFilter::ConfigurationT	ALT_VELO_FILTER;
		static const XShiftFilter::ConfigurationT	ALT_VELO_RATIO_X_FILTER [4];

	// variables ----------------------------------------------------------------------------------
	public:
		double LatTolerance;													// [°]
		double LonTolerance;													// [°]

	// functions ----------------------------------------------------------------------------------
	public:
		Parameter();
};

#endif
