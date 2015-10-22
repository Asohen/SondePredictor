#ifndef XShiftFilter_H
#define XShiftFilter_H

#include <QVector>

#include "Position.h"


class XShiftFilter
{
	// constants ----------------------------------------------------------------------------------
	private:

	// types --------------------------------------------------------------------------------------
	public:
		struct ConfigurationT
		{
			unsigned long	Duration;						// nominal duration [s]
			unsigned long	NominalValueCount;				// number of values at all
			unsigned long	MinimumValueCount;				// number of values within duration
		};

	private:
		struct ValueT
		{
			long	Time;
			double	Value;
			bool	ValueInAverage;
		};

	// variables ----------------------------------------------------------------------------------
	private:
		QVector<ValueT*>	Values;
		unsigned long	Duration;
		unsigned long	NominalValueCount;					// requested number of elements in Values
		unsigned long	MinimumValueCount;					// minimum number of elements in Values
		unsigned long	StartIndex;
		unsigned long	EndIndex;
		unsigned long	Count;
		double			Sum;
		double			LastFilteredValue;

	// functions ----------------------------------------------------------------------------------
	public:
		XShiftFilter(ConfigurationT xFilter);				// for filter off: 0, 1, 1

		void Configure(ConfigurationT xFilter);

		bool GetNewFilteredValue
			(												// result: filteredValue was calculated
				unsigned long	currentTime,				// current time [ms]
				double			newValue					// value to append to average
			);

		inline unsigned long	ValueCount()	{ return Values.size(); }
		inline unsigned long	SumCount()		{ return Count; }
		inline unsigned long	NominalCount()	{ return NominalValueCount; }
		inline unsigned long	MinimumCount()	{ return MinimumValueCount; }
		inline double			FilteredValue()	{ return LastFilteredValue; }

	private:
		void Append
			(
				unsigned long	time,
				double			value
			);
};

#endif
