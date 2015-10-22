
#include "XShiftFilter.h"


//-------------------------------------------------------------------------------------------------
XShiftFilter::XShiftFilter(ConfigurationT xFilter)
	: Duration	(xFilter.Duration)
	, NominalValueCount	(xFilter.NominalValueCount)
	, MinimumValueCount	(xFilter.MinimumValueCount)
{
	StartIndex			= 0;
	EndIndex			= 0;
	Count				= 0;
	Sum					= 0;
	LastFilteredValue	= 0;
}

//-------------------------------------------------------------------------------------------------
void XShiftFilter::Configure(ConfigurationT xFilter)
{
	Duration			= xFilter.Duration;
	NominalValueCount	= xFilter.NominalValueCount;
	MinimumValueCount	= xFilter.MinimumValueCount;
}

//-------------------------------------------------------------------------------------------------
bool XShiftFilter::GetNewFilteredValue
	(
		unsigned long	currentTime,
		double			newValue
	)
{
	bool	result = false;									// initilization

	Append(currentTime, newValue);

	if(Values.size() >= static_cast<int>(NominalValueCount))
	{
		long startTime				= currentTime	- Duration;
		unsigned long startIndex	= Values.size()	- NominalValueCount;
		unsigned long endIndex		= Values.size();

		for(unsigned long i=StartIndex; i<startIndex; i++)
		{
			if(Values.at(i)->ValueInAverage)
			{
				Sum -= Values.at(i)->Value;
				Count --;
				Values.at(i)->ValueInAverage = false;
			}
		}
		StartIndex = startIndex;

		for(unsigned long i=startIndex; i<EndIndex; i++)
		{
			if(Values.at(i)->ValueInAverage)
			{
				if(Values.at(i)->Time < startTime)
				{
					Sum -= Values.at(i)->Value;
					Count --;
					Values.at(i)->ValueInAverage = false;
				}
				else
				{
					break;
				}
			}
		}

		for(unsigned long i=EndIndex; i<endIndex; i++)
		{
			if(Values.at(i)->Time >= startTime)
			{
				Sum += Values.at(i)->Value;
				Count ++;
				Values.at(i)->ValueInAverage = true;
			}
		}
		EndIndex = endIndex;

		if(Count >= MinimumValueCount)
		{
			LastFilteredValue = Sum / static_cast<double>(Count);
			result = true;
		}
	}

	return result;
}

//-------------------------------------------------------------------------------------------------
void XShiftFilter::Append
	(
		unsigned long	time,
		double			value
	)
{
	ValueT* element = new ValueT;

	element->Time			= time;
	element->Value			= value;
	element->ValueInAverage	= false;

	Values.append(element);
}
