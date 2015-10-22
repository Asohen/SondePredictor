#ifndef GpsReader_H
#define GpsReader_H

#include <QObject>
//#include <QGeoPositionInfo>
//#include <QGeoPositionInfoSource>
//#include <QSystemInfo>

//using namespace QtMobility;

// Neccessary for Qt Mobility API usage
//QTM_USE_NAMESPACE

#include "Nmea.h"


class GpsReader : public QObject
{
	Q_OBJECT

	// constants ----------------------------------------------------------------------------------
	public:
		enum GPS_OK
		{
			GPS_NONE,
			GPS_INVALID,
			GPS_VALID
		};

	// variables ----------------------------------------------------------------------------------
	public:
		QSerialPort&	NmeaPort;

	private:
// 		QGeoPositionInfoSource*	PositionInfoSource;
		Nmea			NmeaInput;

	// functions ----------------------------------------------------------------------------------
	public:
		GpsReader(QSerialPort& NmeaPort, QObject* parent = 0);
		virtual ~GpsReader();

	signals:
		void GpsUpdate(GpsReader::GPS_OK gpsOk, double latitude, double longitude);

	public slots:
		void PositionUpdated();	//(QGeoPositionInfo geoPositionInfo);
//		void GpsError(QSerialPort::SerialPortError error);
//		void UpdateTimeout();
};

#endif

