#include <cmath>				// lround
#include <iostream>

#include "GpxWriter.h"
#include "Version.h"

const QString	GpxWriter::FILE_CSV_EXT = ".csv";
const QString	GpxWriter::FILE_GPX_EXT = ".gpx";

const QString	GpxWriter::SYM_POINT	= "point";

// Garmin eTrex 30 symbols
const QString	GpxWriter::SYM_GARMIN_ETREX30_CYCLE_WHITE	= "Navaid, White";	// Navaid = Seezeichen
const QString	GpxWriter::SYM_GARMIN_ETREX30_CYCLE_BLUE	= "Navaid, Blue";
const QString	GpxWriter::SYM_GARMIN_ETREX30_CYCLE_GREEN	= "Navaid, Green";
const QString	GpxWriter::SYM_GARMIN_ETREX30_CYCLE_RED		= "Navaid, Red";
const QString	GpxWriter::SYM_GARMIN_ETREX30_CYCLE_BLACK	= "Navaid, Black";
const QString	GpxWriter::SYM_GARMIN_ETREX30_BLOCK_GREEN	= "Block, Green";
const QString	GpxWriter::SYM_GARMIN_ETREX30_BLOCK_RED		= "Block, Red";
const QString	GpxWriter::SYM_GARMIN_ETREX30_RESIDENCE		= "Residence";
const QString	GpxWriter::SYM_GARMIN_ETREX30_DOT_GREEN		= "City (Large)";
const QString	GpxWriter::SYM_GARMIN_ETREX30_BRIDGE		= "Bridge";

// used symbols
const QString&	GpxWriter::SYM_GPX_POINT	= GpxWriter::SYM_GARMIN_ETREX30_CYCLE_RED;


//-------------------------------------------------------------------------------------------------
GpxWriter::GpxWriter(QFile& gpxFile)
{
	LastEndPosition = 0;
	GpxStream.setDevice(&gpxFile);
	AddGpxStart();
}

//-------------------------------------------------------------------------------------------------
GpxWriter::~GpxWriter()
{
//	AddGpxEnd();
}

//-------------------------------------------------------------------------------------------------
void GpxWriter::AddGpxStart()
{
	GpxStream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
	GpxStream << "<gpx";
	GpxStream << " xmlns=\"http://www.topografix.com/GPX/1/1\" xmlns:gpxx=\"http://www.garmin.com/xmlschemas/GpxExtensions/v3\" xmlns:wptx1=\"http://www.garmin.com/xmlschemas/WaypointExtension/v1\" xmlns:gpxtpx=\"http://www.garmin.com/xmlschemas/TrackPointExtension/v1\"";
	GpxStream << " creator=\"SondePredictor\" version=\"" << Version::VERSION << "\"" << ">" << endl;
//	GpxStream << "	<metadata>";
//	GpxStream << "		<link href=\"http://AsphaltOhneEnde.de\">");
//	GpxStream << "			<text>AsphaltOhneEnde</text>");
//	GpxStream << "		</link>");
//	WriteStringN("		<time>" + FormatDate("%YYYY-%mm-%ddT%hh:%ii:%ssZ", Date()) + "</time>");
//	WriteStringN("		<bounds minlat=\"51.184889000\" minlon=\"6.880059000\" maxlat=\"54.796286000\" maxlon=\"13.259436000\"/>");
//	WriteStringN("	</metadata>");

	LastEndPosition = GpxStream.pos();
	GpxStream.flush();
}

//-------------------------------------------------------------------------------------------------
void GpxWriter::AddGpxWaypoint
	(
		double		latitude,
		double		longitude,
		double		height,
		bool		zVelocityValid,
		double		zVelocity,
		long		timeToLanding
	)
{
	GpxStream.seek(LastEndPosition);						// remove last </gpx>

	GpxStream.setRealNumberPrecision(7);
	GpxStream << "	<wpt lat=\"" << latitude << "\" lon=\"" << longitude << "\">" << endl;
//	GpxStream << "		<ele>" << height << "</ele>" << endl;

	GpxStream << "		<name>";
	GpxStream.setFieldWidth(COORD_DECIMAL_COUNT);
	GpxStream.setPadChar('0');
	GpxStream << lround(height);
	GpxStream.setFieldWidth(0);
	GpxStream << "</name>" << endl;

	if(zVelocityValid)
	{
		GpxStream << "		<cmt>" << lround(zVelocity) << "m/s" << ", " << timeToLanding << " minutes left</cmt>" << endl;
	}
	else
	{
		GpxStream << "		<cmt>" << timeToLanding << " minutes left</cmt>" << endl;
	}
//	GpxStream << "		<desc>" << heigth << "</desc>" << endl;
	GpxStream << "		<sym>" << SYM_GPX_POINT << "</sym>" << endl;
	GpxStream << "	</wpt>" << endl;

	AddGpxEnd();
	GpxStream.flush();
}

//-------------------------------------------------------------------------------------------------
void GpxWriter::AddGpxEnd()
{
	LastEndPosition = GpxStream.pos();

	GpxStream << "</gpx>" << endl;
//	GpxStream.flush();
}
