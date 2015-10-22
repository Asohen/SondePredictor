#include <math.h>

#include "QDebug"
#include "Nmea.h"
#include "QDateTime"


//-------------------------------------------------------------------------------------------------
Nmea::Nmea(QSerialPort& serialPort) :
	SerialPort(serialPort)
{
}

//-------------------------------------------------------------------------------------------------
bool Nmea::ReceivePosition(Position& position, bool& gpsOk)
{
	bool result = false;

	if(		SerialPort.isOpen()
		&&	SerialPort.isWritable()
		)
	{
		QByteArray data = SerialPort.readAll();

		for(int i=0; i<data.size(); i++)					// get line
		{
			if(data.at(i) == '\n')							// line completed
			{
				result = EvaluateLine(position, gpsOk);
				Line.clear();
			}
			else											// continue line
			{
				if(data.at(i) != '\n' && data.at(i) != '\r')
				{
					Line.append(data.at(i));
				}
			}
		}
	} //else NMEA output not applied

//	qDebug() << result << " " << position.Latitude << " " << position.Longitude;
	return result;
}

//-------------------------------------------------------------------------------------------------
bool Nmea::EvaluateLine(Position& position, bool& gpsOk)
{
	bool result = false;

	if(!Line.isEmpty())										// data available
	{
//		qDebug() << "Line: " << Line;
		// "$GPRMC,HHMMSS,A,BBBB.BBBB,b,LLLLL.LLLL,l,GG.G,RR.R,DDMMYY,M.M,m,F*PP"

		unsigned char checkSum = 0;
		for(int i=1; i<Line.length() - 3; i++)				// except PP itself
		{
			checkSum = checkSum ^ Line.at(i).toLatin1();
		}

		QStringList elements = Line.split(",");

		if(elements.size() == 13)
		{
			QString type = elements[0];
			if(type == "$GPRMC")
			{
				QString lat		= elements[3];
				QString latSign	= elements[4];
				QString lon		= elements[5];
				QString lonSign	= elements[6];

				double latDegree = lat.mid(0, 2).toDouble();
				double lonDegree = lon.mid(0, 3).toDouble();
				double latMinute = lat.mid(2).toDouble();
				double lonMinute = lon.mid(3).toDouble();

				position.Latitude	= latDegree + latMinute / 60.;
				position.Longitude	= lonDegree + lonMinute / 60.;

				if(latSign == "S")
				{
					position.Latitude	= -position.Latitude;
				}
				if(lonSign == "W")
				{
					position.Longitude	= -position.Longitude;
				}

				QString f			= elements[12].mid(0, 1);
				QString pp			= elements[12].mid(2);
				unsigned long ppHex	= pp.toLong(0, 16);

				gpsOk = f != "N";										// N = not valid
				result = gpsOk && ppHex == checkSum;
			}
		} //else lack of data
	} //else end of file

	return result;
}

//-------------------------------------------------------------------------------------------------
void Nmea::SendPosition(Position position)
{
	if(		SerialPort.isOpen()
		&&	SerialPort.isWritable()
		)
	{
		QString text;
		QTextStream textStream(&text);

		QDateTime dateTime = QDateTime::currentDateTimeUtc();
		QString	time = dateTime.toString("hhmmss");
		QString	date = dateTime.toString("ddMMyy");

		double	latDegree;
		double	lonDegree;
		double	latMinute	= 60* modf(position.Latitude,  &latDegree);
		double	lonMinute	= 60* modf(position.Longitude, &lonDegree);

		QString	latDegree2	= QString::number(int(latDegree));
		QString	lonDegree2	= QString::number(int(lonDegree));

		if(latDegree2.length() < 2)		{ latDegree2 = "0" + latDegree2; }
		if(lonDegree2.length() < 2)		{ lonDegree2 = "0" + lonDegree2; }

		QString	latMinute2	= QString::number(latMinute);
		QString	lonMinute2	= QString::number(lonMinute);

		if(latMinute2.indexOf('.') < 2)	{ latMinute2 = "0" + latMinute2; }
		if(lonMinute2.indexOf('.') < 2)	{ lonMinute2 = "0" + lonMinute2; }

		QString	latitude	= latDegree2 + latMinute2;
		QString	longitude	= lonDegree2 + lonMinute2;

		QString	latSign		=	(	position.Latitude >= 0
								&&	position.Latitude <  180)
								? "N" : "S";
		QString	lonSign		=	(position.Longitude >= 0)
								? "E" : "W";

		// "$GPRMC,HHMMSS,A,BBBB.BBBB,b,LLLLL.LLLL,l,GG.G,RR.R,DDMMYY,M.M,m,F*PP"
		textStream	<< "$GPRMC,"
					<< time										// HHMMSS		Zeit (UTC)
					<< ",A,"									// A			Status, A=OK, V=Warnung
					<< latitude << "," << latSign << ","		// BBBB.BBBB,b	Breitengrad
					<< longitude << "," << lonSign << ","		// LLLLL.LLLL,l	Längengrad
					<< ","										// GG.G			Geschwindigkeit über Grund
					<< ","										// RR.R			Kurs über Grund
					<< date << ","								// DDMMYY		Datum
					<< ",,"										// M.M,m		Magnetische Abweichung, E=Ost, W=West
					<< "A";										// F			Signalintegrität: A = Autonomous mode, D = Differential Mode, E = Estimated (dead-reckoning) mode, M = Manual Input Mode, S = Simulated Mode, N = Data Not Valid

		unsigned char checkSum = 0;
		for(int i=1; i<text.length(); i++)
		{
			checkSum = checkSum ^ text.at(i).toLatin1();
		}

		QString pp;
		QTextStream ppStream(&pp);
		ppStream	<< "*";
		ppStream.setFieldWidth(2);
		ppStream.setPadChar('0');
		ppStream	<< hex << checkSum << dec;					// PP			Prüfsumme
		ppStream.setFieldWidth(0);
		ppStream	<< endl;

		text = text + pp.toUpper();

		SerialPort.write(text.toStdString().c_str());
		SerialPort.flush();
//		SerialPort.waitForBytesWritten(5000);					// blocking, needed for console application (has no event loop)
	} //else NMEA output not applied
}
