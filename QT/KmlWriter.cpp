#include <cmath>						// lround
#include <iostream>
#include <windows.h>					// Sleep

#include "KmlWriter.h"
#include "Version.h"

//const QString	KmlWriter::SYM_POINT	= "point";


//-------------------------------------------------------------------------------------------------
KmlWriter::KmlWriter(QFile& kmlFile)
{
	KmlStream.setDevice(&kmlFile);
}

//-------------------------------------------------------------------------------------------------
KmlWriter::~KmlWriter()
{
}

//-------------------------------------------------------------------------------------------------
void KmlWriter::AddKmlStart()
{
	KmlStream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
	KmlStream << "<kml xmlns=\"http://earth.google.com/kml/2.2\">" << endl;
	KmlStream << "	<Document>" << endl;
	KmlStream << "		<name>SondePredictor " << Version::VERSION << "</name>" << endl;
//	KmlStream << "		<description><![CDATA[]]></description>" << endl;

//	AddKmlStyle(SYM_POINT, "")
}

//-------------------------------------------------------------------------------------------------
void KmlWriter::AddKmlStyle(QString& name, QString& symbol)
{
	KmlStream << "		<Style id=\"" << name << "\">";
	KmlStream << "			<IconStyle>";
	KmlStream << "				<Icon>";
	KmlStream << "					<href>" << symbol << "</href>";
	KmlStream << "				</Icon>";
	KmlStream << "			</IconStyle>";
	KmlStream << "		</Style>";
	KmlStream << "" << endl;
}

//-------------------------------------------------------------------------------------------------
void KmlWriter::AddKmlWaypoint
	(
		double		latitude,
		double		longitude,
		double		height,
		bool		zVelocityValid,
		double		zVelocity,
		long		timeToLanding
	)
{
	KmlStream.seek(0);										// remove last content
	AddKmlStart();

	KmlStream.setRealNumberPrecision(7);
	KmlStream << "		<Placemark>" << endl;
	KmlStream.setFieldWidth(COORD_DECIMAL_COUNT);
	KmlStream.setPadChar('0');
	KmlStream << "			<name>" << lround(height) << "</name>";
	KmlStream.setFieldWidth(0);
	KmlStream << endl;
	if(zVelocityValid)
	{
		KmlStream << "			<Snippet>"		<< lround(zVelocity) << "m/s, " << timeToLanding << " minutes left" << "</Snippet>" << endl;
		KmlStream << "			<description>"	<< lround(zVelocity) << "m/s, " << timeToLanding << " minutes left" << "</description>" << endl;
	}
	else
	{
		KmlStream << "			<Snippet>"		<< timeToLanding << " minutes left" << "</Snippet>" << endl;
		KmlStream << "			<description>"	<< timeToLanding << " minutes left" << "</description>" << endl;
	}
//	KmlStream << "			<styleUrl></styleUrl>" << endl;
	KmlStream << "			<Point>" << endl;
	KmlStream << "				<coordinates>" << longitude << "," << latitude << "," << height << "</coordinates>" << endl;
	KmlStream << "			</Point>" << endl;
	KmlStream << "		</Placemark>" << endl;

	AddKmlEnd();
	KmlStream.flush();
}

//-------------------------------------------------------------------------------------------------
void KmlWriter::AddKmlEnd()
{
	KmlStream << "	</Document>" << endl;
	KmlStream << "</kml>" << endl;
}
