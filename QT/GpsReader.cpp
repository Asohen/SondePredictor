
#include "QDebug"
#include "GpsReader.h"


//-------------------------------------------------------------------------------------------------
GpsReader::GpsReader(QSerialPort& nmeaPort, QObject* parent)
	: QObject	(parent)
	, NmeaPort	(nmeaPort)
	, NmeaInput	(nmeaPort)
// 	, PositionInfoSource(NULL)
{
// 	PositionInfoSource = QGeoPositionInfoSource::createDefaultSource(this);

//	if(PositionInfoSource)
//	{
//		qDebug() << "Start";
//	//	PositionInfoSource->setPreferredPositioningMethods(QGeoPositionInfoSource::SatellitePositioningMethods);
//		connect(PositionInfoSource, SIGNAL (positionUpdated(QGeoPositionInfo)), this, SLOT (PositionUpdated(QGeoPositionInfo)));
//		connect(PositionInfoSource, SIGNAL (updateTimeout ()), this, SLOT(UpdateTimeout ()));
//		PositionInfoSource->startUpdates();
//	}
}

//-------------------------------------------------------------------------------------------------
GpsReader::~GpsReader()
{
}

//-------------------------------------------------------------------------------------------------
void GpsReader::PositionUpdated()	//(QGeoPositionInfo geoPositionInfo)
{
//	if(geoPositionInfo.isValid())
	{
//		QGeoCoordinate geoCoordinate = geoPositionInfo.coordinate();
//		qreal latitude	= geoCoordinate.latitude();
//		qreal longitude	= geoCoordinate.longitude();

//		QByteArray data = NmeaPort.readAll();

		Position	position;
		bool		ok;
		if(NmeaInput.ReceivePosition(position, ok))
		{
			if(ok)
			{
				emit GpsUpdate(GPS_VALID, position.Latitude, position.Longitude);
			}
			else											// GPS position not available
			{
				emit GpsUpdate(GPS_INVALID, position.Latitude, position.Longitude);
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------
//void GpsReader::GpsError(QSerialPort::SerialPortError error)
//{
//	if (error == QSerialPort::ResourceError)
//	{
//		qDebug() << "MainWindow: Error: " + NmeaInPort.errorString();
//	}
//}

//-------------------------------------------------------------------------------------------------
//void GpsReader::UpdateTimeout()
//{
//	// Current location could not be retrieved within the specified timeout of 5 seconds.
//	qDebug() << "Failed to retrieve current position";
//}
