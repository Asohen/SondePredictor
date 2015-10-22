#ifndef Nmea_H
#define Nmea_H

#include <iostream>
using std::endl;
#include <QString>
#include <QFile>
#include <QDateTime>

#include <Position.h>
#include <QSerialPort>
#include <QTextStream>


class Nmea
{
	// constants ----------------------------------------------------------------------------------
	private:

	// variables ----------------------------------------------------------------------------------
	public:
		QSerialPort&	SerialPort;
	private:
		QString			Line;

	// functions ----------------------------------------------------------------------------------
	public:
		Nmea(QSerialPort& serialPort);
		bool ReceivePosition
			(
				Position&	position,
				bool&		gpsOk
			);
		void SendPosition
			(
				Position	position
			);

	private:
		bool EvaluateLine(Position& position, bool& gpsOk);
};

#endif

