#include <QApplication>
#include <QLabel>
#include <QTextBrowser>
#include "mainwindow.h"

#include "Version.h"
#include "Execution.h"


Execution	Executer;


//-------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	QApplication	a(argc, argv);

	MainWindow		w;
	QString			title	= "SondePredictor " + Version::VERSION;
	w.setWindowTitle(title);

	QLabel*		sondeDistance	= w.centralWidget()->findChild<QLabel*>	("SondeDistance");
	QLabel*		sondeDirection	= w.centralWidget()->findChild<QLabel*>	("SondeDirection");
	QLabel*		distance		= w.centralWidget()->findChild<QLabel*>	("Distance");
	QLabel*		direction		= w.centralWidget()->findChild<QLabel*>	("Direction");
	QWidget*	normalArea		= w.centralWidget()->findChild<QWidget*>("NormalArea");
	QWidget*	errorArea		= w.centralWidget()->findChild<QWidget*>("ErrorArea");

	if(sondeDistance	!= NULL)	{ QObject::connect(&Executer,	SIGNAL(SondeDistance	(QString)),	sondeDistance,	SLOT(setText(QString))); }
	if(sondeDirection	!= NULL)	{ QObject::connect(&Executer,	SIGNAL(SondeDirection	(QString)),	sondeDirection,	SLOT(setText(QString))); }
	if(distance			!= NULL)	{ QObject::connect(&Executer,	SIGNAL(Distance			(QString)),	distance,		SLOT(setText(QString))); }
	if(direction		!= NULL)	{ QObject::connect(&Executer,	SIGNAL(Direction		(QString)),	direction,		SLOT(setText(QString))); }

	if(		normalArea	!= NULL
		&&	errorArea	!= NULL
		)
	{
		normalArea->setHidden(true);
		errorArea->setHidden (true);

		QLabel* sondeLanding	= normalArea->findChild<QLabel*>("SondeLanding");
		QLabel* altitude		= normalArea->findChild<QLabel*>("Altitude");
		QLabel* ascentDescent	= normalArea->findChild<QLabel*>("AscentDescent");
		QLabel* duration		= normalArea->findChild<QLabel*>("Duration");
		QLabel* latitude		= normalArea->findChild<QLabel*>("Latitude");
		QLabel* latitudeSign	= normalArea->findChild<QLabel*>("LatitudeSign");
		QLabel* longitude		= normalArea->findChild<QLabel*>("Longitude");
		QLabel* longitudeSign	= normalArea->findChild<QLabel*>("LongitudeSign");
		QLabel* rate			= normalArea->findChild<QLabel*>("Rate");
		QLabel* ratePercent		= normalArea->findChild<QLabel*>("RatePercent");

		QLabel* errorAreaText1	= errorArea->findChild<QLabel*>("Error_1");
		QLabel* errorAreaText2	= errorArea->findChild<QLabel*>("Error_2");
		QLabel* errorAreaText3	= errorArea->findChild<QLabel*>("Error_3");
		QLabel* errorAreaText4	= errorArea->findChild<QLabel*>("Error_4");
		QLabel* errorAreaText5	= errorArea->findChild<QLabel*>("Error_5");
		QLabel* errorAreaText6	= errorArea->findChild<QLabel*>("Error_6");
		QLabel* errorAreaText7	= errorArea->findChild<QLabel*>("Error_7");
		QLabel* errorAreaText8	= errorArea->findChild<QLabel*>("Error_8");
		QLabel* errorAreaText9	= errorArea->findChild<QLabel*>("Error_9");

		if(normalArea		!= NULL)	{ QObject::connect(&Executer,	SIGNAL(HideNormal	(bool)),	normalArea,		SLOT(setHidden(bool))); }
		if(errorArea		!= NULL)	{ QObject::connect(&Executer,	SIGNAL(HideError	(bool)),	errorArea,		SLOT(setHidden(bool))); }

		if(sondeLanding		!= NULL)	{ QObject::connect(&Executer,	SIGNAL(SondeLanding	(QString)),	sondeLanding,	SLOT(setText(QString))); }
		if(altitude			!= NULL)	{ QObject::connect(&Executer,	SIGNAL(Altitude		(QString)),	altitude,		SLOT(setText(QString))); }
		if(ascentDescent	!= NULL)	{ QObject::connect(&Executer,	SIGNAL(AscentDescent(QString)),	ascentDescent,	SLOT(setText(QString))); }
		if(duration			!= NULL)	{ QObject::connect(&Executer,	SIGNAL(Duration		(QString)),	duration,		SLOT(setText(QString))); }
		if(latitude			!= NULL)	{ QObject::connect(&Executer,	SIGNAL(Latitude		(QString)),	latitude,		SLOT(setText(QString))); }
		if(latitudeSign		!= NULL)	{ QObject::connect(&Executer,	SIGNAL(LatitudeSign	(QString)),	latitudeSign,	SLOT(setText(QString))); }
		if(longitude		!= NULL)	{ QObject::connect(&Executer,	SIGNAL(Longitude	(QString)),	longitude,		SLOT(setText(QString))); }
		if(longitudeSign	!= NULL)	{ QObject::connect(&Executer,	SIGNAL(LongitudeSign(QString)),	longitudeSign,	SLOT(setText(QString))); }
		if(rate				!= NULL)	{ QObject::connect(&Executer,	SIGNAL(Rate			(QString)),	rate,			SLOT(setText(QString))); }
		if(ratePercent		!= NULL)	{ QObject::connect(&Executer,	SIGNAL(RatePercent	(QString)),	ratePercent,	SLOT(setText(QString))); }

		if(errorAreaText1	!= NULL)	{ QObject::connect(&Executer,	SIGNAL(ErrorText1(QString)),	errorAreaText1,	SLOT(setText(QString))); }
		if(errorAreaText2	!= NULL)	{ QObject::connect(&Executer,	SIGNAL(ErrorText2(QString)),	errorAreaText2,	SLOT(setText(QString))); }
		if(errorAreaText3	!= NULL)	{ QObject::connect(&Executer,	SIGNAL(ErrorText3(QString)),	errorAreaText3,	SLOT(setText(QString))); }
		if(errorAreaText4	!= NULL)	{ QObject::connect(&Executer,	SIGNAL(ErrorText4(QString)),	errorAreaText4,	SLOT(setText(QString))); }
		if(errorAreaText5	!= NULL)	{ QObject::connect(&Executer,	SIGNAL(ErrorText5(QString)),	errorAreaText5,	SLOT(setText(QString))); }
		if(errorAreaText6	!= NULL)	{ QObject::connect(&Executer,	SIGNAL(ErrorText6(QString)),	errorAreaText6,	SLOT(setText(QString))); }
		if(errorAreaText7	!= NULL)	{ QObject::connect(&Executer,	SIGNAL(ErrorText7(QString)),	errorAreaText7,	SLOT(setText(QString))); }
		if(errorAreaText8	!= NULL)	{ QObject::connect(&Executer,	SIGNAL(ErrorText8(QString)),	errorAreaText8,	SLOT(setText(QString))); }
		if(errorAreaText9	!= NULL)	{ QObject::connect(&Executer,	SIGNAL(ErrorText9(QString)),	errorAreaText9,	SLOT(setText(QString))); }

		QObject::connect(&Executer, SIGNAL(Phase(Position::Phase)),		&w, SLOT(Phase(Position::Phase)));
		QObject::connect(&Executer, SIGNAL(GpsOk(GpsReader::GPS_OK)),	&w, SLOT(GpsOk(GpsReader::GPS_OK)));

		Executer.Start(argc, argv);
	}

	w.show();

	return a.exec();
}
