#include <QDebug>
#include <QTimer>
#include <windows.h>										// Sleep
#include <QtCore/qmath.h>

#include "Execution.h"
#include "Version.h"
#include "Parameter.h"
#include "LivePrediction.h"


const QString	Execution::SEPARATOR	= "\\";
const QString	Execution::GROUND_TRACK_FILE_NAME_PREFIX				= "groundtrack";
const QString	Execution::GROUND_TRACK_FILE_NAME_PREFIX_M10			= "digitalsonde";
const QString	Execution::GROUND_TRACK_FILE_NAME_SUFFIX				= "txt";
const QString	Execution::GROUND_TRACK_FILE_NAME_SUFFIX_M10			= "log";
const QString	Execution::LIVE_PREDICTION_RESULT_FILE_NAME_EXTENSION	= ".txt";
const QString	Execution::LIVE_PREDICTION_GPX_FILE_NAME_EXTENSION		= ".gpx";
//const QString	Execution::LIVE_PREDICTION_KML_FILE_NAME_EXTENSION		= ".kml";
const QString	Execution::LIVE_PREDICTION_KML_FILE_NAME				= "LivePrediction_Current.kml";
const QString	Execution::LIVE_PREDICTION_FILE_NAME_PREFIX				= "LivePrediction_";


//-------------------------------------------------------------------------------------------------
Execution::Execution(QObject *parent)
:	QObject(parent)
,	GpsRead(NmeaInPort)
{
	Live				= NULL;
	StatusOk			= true;
	NmeaOutputApplied	= false;
}

//-------------------------------------------------------------------------------------------------
Execution::~Execution()
{
}

//-------------------------------------------------------------------------------------------------
void Execution::WriteLine()
{
	Live->ResultStream << "--------------------------------------------------------" << endl;
}

//-------------------------------------------------------------------------------------------------
void Execution::WriteParameters()
{
	Live->ResultStream << "Optional start parameters (no mandatory parameters):" << endl;
	Live->ResultStream << "    Name of NMEA output COM interface (e.g. COM5)" << endl;
	Live->ResultStream << "    Name of directory for input and output files" << endl;
	Live->ResultStream << "    Name of CUSF prediction file (e.g. output.csv)" << endl;
	Live->ResultStream << "    Name of Sondemonitor ground track file" << endl;
}

//-------------------------------------------------------------------------------------------------
void Execution::OpenFile
	(
		const char*				errorText,
		const QString&			fileName,
		QIODevice::OpenModeFlag	fileMode,
		QFile&					file
	)
{
	if(!file.isOpen())
	{
		file.setFileName(fileName);
		file.open(fileMode | QIODevice::Text);
		if(!file.isOpen())
		{
			emit HideError(false);
			emit ErrorText2(errorText + fileName);
		}
	}
}

//-------------------------------------------------------------------------------------------------
void Execution::CloseFile(QFile& file)
{
	if(file.isOpen())
	{
		file.close();
	}
}

//-------------------------------------------------------------------------------------------------
void Execution::CreateNmeaInput(char* nmeaInputName)
{
	NmeaInPort.setPortName		(nmeaInputName);
	NmeaInPort.setBaudRate		(QSerialPort::Baud4800);
	NmeaInPort.setDataBits		(QSerialPort::Data8);
	NmeaInPort.setParity		(QSerialPort::NoParity);
	NmeaInPort.setStopBits		(QSerialPort::OneStop);
	NmeaInPort.setFlowControl	(QSerialPort::NoFlowControl);

	NmeaInPort.open(QIODevice::ReadWrite);
	if(!NmeaInPort.isOpen())
	{
		emit HideError(false);
		emit ErrorText5("NMEA input not accessible: " + NmeaOutputName);
	}
}

//-------------------------------------------------------------------------------------------------
void Execution::CreateNmeaOutput(char* nmeaOutputName)
{
	NmeaOutputApplied = false;

	if(nmeaOutputName != NULL)
	{
		if(nmeaOutputName[0] != 0)
		{
			NmeaOutputApplied = true;
		}
	}

	if(NmeaOutputApplied)
	{
		NmeaOutputName = nmeaOutputName;

		NmeaOutPort.setPortName		(NmeaOutputName);
		NmeaOutPort.setBaudRate		(QSerialPort::Baud4800);
		NmeaOutPort.setDataBits		(QSerialPort::Data8);
		NmeaOutPort.setParity		(QSerialPort::NoParity);
		NmeaOutPort.setStopBits		(QSerialPort::OneStop);
		NmeaOutPort.setFlowControl	(QSerialPort::NoFlowControl);

		NmeaOutPort.open(QIODevice::ReadWrite);
		if(!NmeaOutPort.isOpen())
		{
			emit HideError(false);
			emit ErrorText3("NMEA output not accessible: " + NmeaOutputName);
		}
	}
}

//-------------------------------------------------------------------------------------------------
void Execution::CreatePredictionFileName (QString& directory, QString& prediction)
{
	directory.remove("\"");

	QDirIterator	dirIt(directory, QDirIterator::NoIteratorFlags);
	QDateTime		dateTime;
	QString			fileName;

	dateTime.setTime_t(0);									// initialization for comparision

	while(dirIt.hasNext())
	{
		dirIt.next();
		QFileInfo fileInfo(dirIt.filePath());

		if(		fileInfo.isFile()
			&&	(	fileInfo.fileName().contains("output.csv")
				||	fileInfo.fileName().contains("flight_path.csv")
				)
			)
		{
			if(fileInfo.lastModified() > dateTime)
			{
				dateTime = fileInfo.lastModified();
				fileName = fileInfo.fileName();
			}
		}
	}

	prediction = fileName;
}

//-------------------------------------------------------------------------------------------------
bool Execution::CreateGroundTrackFileName (QString& directory, QString& groundTrack)
{
	QDateTime	dateTime;

	dateTime.setTime_t(0);									// initialization for comparision

	if(groundTrack.isEmpty())
	{
		QDirIterator	dirIt(directory, QDirIterator::NoIteratorFlags);

		while(dirIt.hasNext())
		{
			dirIt.next();
			QFileInfo fileInfo(dirIt.filePath());

			bool isStandard	=	fileInfo.suffix() == GROUND_TRACK_FILE_NAME_SUFFIX
							&&	fileInfo.completeBaseName().startsWith(GROUND_TRACK_FILE_NAME_PREFIX)
							&&	fileInfo.completeBaseName().size() > GROUNDTRACK_FILE_NAME_LENGTH_1;
			bool isM10		=	fileInfo.suffix() == GROUND_TRACK_FILE_NAME_SUFFIX_M10
							&&	fileInfo.completeBaseName().startsWith(GROUND_TRACK_FILE_NAME_PREFIX_M10)
							&&	fileInfo.completeBaseName().size() > GROUNDTRACK_FILE_NAME_LENGTH_1_M10;

			if(		fileInfo.isFile()
				&&	(isStandard || isM10)
				)
			{
				if(fileInfo.lastModified() > dateTime)
				{
					dateTime = fileInfo.lastModified();
					groundTrack = fileInfo.fileName();
				}

				if(isStandard)
				{
					GroundTrackPrefix = GROUND_TRACK_FILE_NAME_PREFIX;
				}
				else
				if(isM10)
				{
					GroundTrackPrefix = GROUND_TRACK_FILE_NAME_PREFIX_M10;
				}
			}
		}
	}

	return !groundTrack.isEmpty();
}

//-------------------------------------------------------------------------------------------------
void Execution::CreateLivePredictionFileName
	(
		QString&	prediction,
		QString&	groundTrack,
		QString&	livePrediction,
		const QString&	extension
	)
{
	if(!groundTrack.isEmpty())								// valid name
	{
		(void) prediction;
	//	QFileInfo fileInfoPrediction (prediction);
		QFileInfo fileInfoGroundTrack(groundTrack);

		livePrediction = fileInfoGroundTrack.baseName() + extension;
		livePrediction.replace(GroundTrackPrefix, LIVE_PREDICTION_FILE_NAME_PREFIX);
		livePrediction.replace(":", "-");
	}
}

//-------------------------------------------------------------------------------------------------
bool Execution::OpenAllFiles
	(
		unsigned long	parameterCount,
		char*	nmeaOutputName,
		char*	directoryName,
		char*	predictionFileName,
		char*	groundTrackFileName
	)
{
	bool found = false;

	QString directory;										// directory name
	QString prediction;										// prediction file name
	QString groundTrack;									// ground track file name
	QString livePrediction;									// live prediction result file name
	QString livePredictionGpx;								// live prediction GPX file name
	QString livePredictionKml;								// live prediction KML file name
	QString fullPrediction;

	char* nmeaInputName = const_cast<char*>("COM4");
	CreateNmeaInput(nmeaInputName);

	switch(parameterCount)
	{
		case 0:												// no directory specified
		{
			CreateNmeaOutput(NULL);
			directory = ".";								// working directory
			CreatePredictionFileName (directory, prediction);
			fullPrediction = OpenPrediction(directory, prediction);
			found = CreateGroundTrackFileName(directory, groundTrack);
			break;
		}

		case 1:												// NMEA output specified
			CreateNmeaOutput(nmeaOutputName);
			directory = ".";								// working directory
			CreatePredictionFileName (directory, prediction);
			fullPrediction = OpenPrediction(directory, prediction);
			found = CreateGroundTrackFileName(directory, groundTrack);
			break;

		case 2:												// directory specified
			CreateNmeaOutput(nmeaOutputName);
			directory	= directoryName;
			CreatePredictionFileName (directory, prediction);
			fullPrediction = OpenPrediction(directory, prediction);
			found = CreateGroundTrackFileName(directory, groundTrack);
			break;

		case 3:												// directory & prediction file specified
			CreateNmeaOutput(nmeaOutputName);
			directory	= directoryName;
			fullPrediction = OpenPrediction(directory, predictionFileName);
			found = CreateGroundTrackFileName(directory, groundTrack);
			break;

		case 4:												// directory & prediction file & ground track file specified
			CreateNmeaOutput(nmeaOutputName);
			directory	= directoryName;
			fullPrediction = OpenPrediction(directory, predictionFileName);
			groundTrack	= groundTrackFileName;
			break;

		default:											// ignore surplus parameters, but show message
			emit HideError(false);
			emit ErrorText4("More than 4 Parameters");
			break;
	}

	if(found)
	{
		QString fullGroundTrack;
		if(groundTrack.isEmpty())
		{
			emit HideError(false);
			emit ErrorText9("No ground track file (internal)");
		}
		else
		{
			fullGroundTrack = directory + SEPARATOR + groundTrack;
			OpenFile
				(	"Ground track file error: ",
					fullGroundTrack,	QIODevice::ReadOnly,	GroundTrackFile);

			CreateLivePredictionFileName(fullPrediction, fullGroundTrack, livePrediction,	 LIVE_PREDICTION_RESULT_FILE_NAME_EXTENSION);
			CreateLivePredictionFileName(fullPrediction, fullGroundTrack, livePredictionGpx, LIVE_PREDICTION_GPX_FILE_NAME_EXTENSION);
	//		CreateLivePredictionFileName(fullPrediction, fullGroundTrack, livePredictionKml, LIVE_PREDICTION_KML_FILE_NAME_EXTENSION);
			livePredictionKml = LIVE_PREDICTION_KML_FILE_NAME;
		}

		if(livePrediction.isEmpty() || livePredictionGpx.isEmpty() || livePredictionKml.isEmpty())
		{
			emit HideError(false);
			emit ErrorText6("No live prediction result file");
		}
		else
		{
			QString fullLivePrediction = directory + SEPARATOR + livePrediction;
			QFile::remove(fullLivePrediction);
			OpenFile
				(	"Live prediction file error: ",
					fullLivePrediction,	QIODevice::WriteOnly, LivePredictionFile);

			QString fullLivePredictionGpx = directory + SEPARATOR + livePredictionGpx;
			QFile::remove(fullLivePredictionGpx);
			OpenFile
				(	"Live prediction GPX file error: ",
					fullLivePredictionGpx,	QIODevice::WriteOnly, LivePredictionGpxFile);

			QString fullLivePredictionKml = directory + SEPARATOR + livePredictionKml;
			QFile::remove(fullLivePredictionKml);
			OpenFile
				(	"Live prediction KML file error: ",
					fullLivePredictionKml,	QIODevice::WriteOnly, LivePredictionKmlFile);
		}
	}

	return found;
}

//-------------------------------------------------------------------------------------------------
QString Execution::OpenPrediction(QString directory, QString fileName)
{
	QString fullPrediction;

	if(fileName.isEmpty())
	{
		emit HideError(false);
		emit ErrorText7("No CUSF prediction file");
	}
	else
	{
		fullPrediction = directory + SEPARATOR + fileName;
		OpenFile
			(	"CUSF prediction file error: ",
				fullPrediction,		QIODevice::ReadOnly,	PredictionFile);
	}

	StatusOk = PredictionFile.isOpen() && PredictionFile.isReadable();

	return fullPrediction;
}

//-------------------------------------------------------------------------------------------------
bool Execution::AllFilesOpen()
{
	return	PredictionFile.isOpen()
		&&	GroundTrackFile.isOpen()
		&&	LivePredictionFile.isOpen()
		&&	LivePredictionGpxFile.isOpen()
		&&	LivePredictionKmlFile.isOpen();
}

//-------------------------------------------------------------------------------------------------
void Execution::CloseAllFiles()
{
	CloseFile(PredictionFile);
	CloseFile(GroundTrackFile);
	CloseFile(LivePredictionFile);
	CloseFile(LivePredictionGpxFile);
	CloseFile(LivePredictionKmlFile);
}

//-------------------------------------------------------------------------------------------------
void Execution::WriteHeader()
{
	Live->ResultStream << "SondePrediction " << Version::VERSION << " by Asohen, http://AsphaltOhneEnde.de" << endl;
}

//-------------------------------------------------------------------------------------------------
void Execution::WriteFilenames()
{
	if(LivePredictionFile.isOpen())
	{
		if(PredictionFile.isOpen())
		{
			Live->ResultStream	<< "File from CUSF (predict.habhub.org): " << PredictionFile.fileName() << endl;
		}

		if(GroundTrackFile.isOpen())
		{
			Live->ResultStream	<< "Sondemonitor ground track file:      " << GroundTrackFile.fileName() << endl;
		}

		Live->ResultStream		<< "Live prediction result file (this):  " << LivePredictionFile.fileName() << endl;

		if(LivePredictionGpxFile.isOpen())
		{
			Live->ResultStream	<< "Live prediction GPX file:            " << LivePredictionGpxFile.fileName() << endl;
		}

		if(LivePredictionKmlFile.isOpen())
		{
			Live->ResultStream	<< "Live prediction KML file:            " << LivePredictionKmlFile.fileName() << endl;
		}
	}

	if(NmeaOutputApplied)
	{
		if(NmeaOutPort.isOpen())
		{
			Live->ResultStream << "NMEA output: " << NmeaOutputName << endl;
		}
		else
		{
			Live->ResultStream << "NMEA output not accessible: " << NmeaOutputName << endl;
		}
	}
	else
	{
		Live->ResultStream << "NMEA output not applied" << endl;
	}
}

//-------------------------------------------------------------------------------------------------
void Execution::PrepareUi()
{
	emit HideNormal(true);
	emit HideError (true);

	emit GpsOk(GpsReader::GPS_NONE);

	emit SondeDistance	("---");
	emit SondeDirection	("---");
	emit Distance		("---");
	emit Direction		("---");

	emit SondeLanding	("Sonde");
	emit Altitude		("---");
	emit AscentDescent	("Waiting");
	emit Duration		("---");
	emit Latitude		("");
	emit LatitudeSign	("");
	emit Longitude		("");
	emit LongitudeSign	("");
	emit Rate			("---");
	emit RatePercent	("---");

	emit ErrorText1("Error");
	emit ErrorText2("");
	emit ErrorText3("");
	emit ErrorText4("");
	emit ErrorText5("");
	emit ErrorText6("");
	emit ErrorText7("");
	emit ErrorText8("");
	emit ErrorText9("");
}

//-------------------------------------------------------------------------------------------------
void Execution::Start(int argc, char *argv[])
{
	PrepareUi();

	if(argc >= 1 && argc <= MAX_PARAMETER_COUNT + 1)
	{
		ArgC = argc;
		ArgV = argv;

		Start1();
	}
	else
	{
		if(argc == 0)
		{
			emit HideError(false);
			emit ErrorText9("No Parameters (internal)");
		}
		else												// too many parameters
		{
			emit HideError(false);
			emit ErrorText8("Too many parameters, more than 4");
		}
	}
}

//-------------------------------------------------------------------------------------------------
void Execution::Start1()
{
	if(OpenAllFiles(ArgC - 1, ArgV[1], ArgV[2], ArgV[3], ArgV[4]))
	{
		Start2();
	}
	else
	{
		emit HideError(false);
		emit ErrorText1("Waiting for ground track file...");
		QTimer::singleShot(1000 * Parameter::PAUSE_DURATION, this, SLOT(Start1()));
	}
}

//-------------------------------------------------------------------------------------------------
void Execution::Start2()
{
	if(AllFilesOpen())
	{
		GroundTrackFile.close();							// normaly closed, will be opened for reading

		Live = new LivePrediction
			(
				this,
				PredictionFile,
				GroundTrackFile,
				LivePredictionFile,
				LivePredictionGpxFile,
				LivePredictionKmlFile,
				NmeaOutPort
			);

		if(Live != NULL)
		{
			WriteHeader();
			WriteLine();
			WriteParameters();
			WriteLine();
			WriteFilenames();
			WriteLine();

			emit HideNormal(false);
			if(Live->Prepare())
			{
				emit HideError(true);
				Work();
				QObject::connect(&NmeaInPort, SIGNAL(readyRead()), &GpsRead, SLOT(PositionUpdated()));
				QObject::connect(&GpsRead, SIGNAL(GpsUpdate(GpsReader::GPS_OK, double, double)), this, SLOT(GpsUpdate(GpsReader::GPS_OK, double, double)));
			}
			else
			{
				emit HideNormal(true);
				emit HideError (false);
			}
		}
		else
		{
			emit HideError(false);
			emit ErrorText9("Out of memory (internal)");
		}
	}
	else
	{
		emit HideError(false);
		emit ErrorText8("Not all files available");
	}
}

//-------------------------------------------------------------------------------------------------
void Execution::Work()
{
	if(Live->Work())
	{
		QTimer::singleShot(0, this, SLOT(Work()));
	}
	else
	{
		QTimer::singleShot(1000 * Parameter::PAUSE_DURATION, this, SLOT(Work()));
	}
}

//-------------------------------------------------------------------------------------------------
void Execution::GpsUpdate(GpsReader::GPS_OK gpsOk, double latitude, double longitude)
{
	bool landing =		Live->SondeTrack.CurrentPhase == Position::PHASE_DESCENT
					&&	Live->LandingPosition.Valid;
	double distance;
	double direction;

	if(gpsOk == GpsReader::GPS_VALID)
	{
		if(Live->LandingPosition.Valid)
		{
			CalculateVector										// direction to landing position
					(	latitude,
						longitude,
						Live->LandingPosition.Latitude,
						Live->LandingPosition.Longitude,
						distance, direction
					);

			emit Distance	(QString::number(distance / 1000, 'g', 3));
			emit Direction	(QString::number(lround(direction)));
		}

		if(gpsOk && Live->CurrentPosition.Valid)
		{
			CalculateVector										// direction to sonde
					(	latitude,
						longitude,
						Live->CurrentPosition.Latitude,
						Live->CurrentPosition.Longitude,
						distance, direction
					);

			emit SondeDistance	(QString::number(distance / 1000, 'g', 3));
			emit SondeDirection	(QString::number(lround(direction)));
		}

		if(landing)
		{
			emit GpsOk(gpsOk);
		}
		else
		{
			emit GpsOk(GpsReader::GPS_INVALID);
		}
	}
	else													// GPS position not available
	{
		emit GpsOk(gpsOk);
	}
}

//-------------------------------------------------------------------------------------------------
void Execution::CalculateVector
	(	double lat1, double lon1,
		double lat2, double lon2,
		double& distance, double& direction
	)
{
	double latitudeMeter;
	double longitudeMeter;
	Position::DegreeToDeltaMeter
		(	lat2, lat1,
			lon2, lon1,
			latitudeMeter, longitudeMeter
		);
	distance = qSqrt(latitudeMeter*latitudeMeter + longitudeMeter*longitudeMeter);

	direction = qAtan2(longitudeMeter, latitudeMeter) * 57.2957795;
	if(direction < 0)
	{
		direction = direction + 360.;
	}
}
