#include "QDebug"

#include <QFont>
#include <QClipboard>

#include "mainwindow.h"
#include "ui_mainwindow.h"


//-------------------------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	SondeLabel			= centralWidget()->findChild<QLabel*>("SondeLabel");
	SondeDirection		= centralWidget()->findChild<QLabel*>("SondeDirection");
	SondeDirectionUnit	= centralWidget()->findChild<QLabel*>("SondeDirectionUnit");
	SondeDistance		= centralWidget()->findChild<QLabel*>("SondeDistance");
	SondeDistanceUnit	= centralWidget()->findChild<QLabel*>("SondeDistanceUnit");

	LandingLabel		= centralWidget()->findChild<QLabel*>("LandingLabel");
	Direction			= centralWidget()->findChild<QLabel*>("Direction");
	DirectionUnit		= centralWidget()->findChild<QLabel*>("DirectionUnit");
	Distance			= centralWidget()->findChild<QLabel*>("Distance");
	DistanceUnit		= centralWidget()->findChild<QLabel*>("DistanceUnit");

	NormalArea			= centralWidget()->findChild<QWidget*>("NormalArea");

	if(NormalArea != NULL)
	{
		Latitude		= NormalArea->findChild<QLabel*>("Latitude");
		Longitude		= NormalArea->findChild<QLabel*>("Longitude");
		AscentDescent	= NormalArea->findChild<QLabel*>("AscentDescent");

		CopyButton		= NormalArea->findChild<QPushButton*>("CopyButton");
		if(CopyButton != NULL)
		{
			QString styleSheet = "QPushButton { background-color: rgba(0, 0, 0, 0%); }";
			CopyButton->setStyleSheet(styleSheet);
			QObject::connect(CopyButton, SIGNAL (clicked()), this, SLOT (CopyToClipboard()));
		}
	}
	else
	{
		AscentDescent	= NULL;
		CopyButton		= NULL;
	}

	CurrentPhase= Position::PHASE_STARTUP;
}

//-------------------------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
	delete ui;
}

//-------------------------------------------------------------------------------------------------
void MainWindow::Phase(Position::Phase phase)
{
	if(phase != CurrentPhase)
	{
		if(		NormalArea		!= NULL
			&&	AscentDescent	!= NULL
			)
		{
			if(phase == Position::PHASE_ASCENT)
			{
				NormalArea->setStyleSheet("QLabel { color : yellow; }");
				AscentDescent->setText("Ascent");
			}
			else
			{
				NormalArea->setStyleSheet("QLabel { color : white; }");
				AscentDescent->setText("Descent");
			}
		}

		CurrentPhase = phase;
	}
}

//-------------------------------------------------------------------------------------------------
void MainWindow::GpsOk(GpsReader::GPS_OK gpsOk)
{
	if(		SondeLabel			!= NULL
		&&	SondeDistance		!= NULL
		&&	SondeDistanceUnit	!= NULL
		&&	SondeDirection		!= NULL
		&&	SondeDirectionUnit	!= NULL
		&&	LandingLabel		!= NULL
		&&	Distance			!= NULL
		&&	DistanceUnit		!= NULL
		&&	Direction			!= NULL
		&&	DirectionUnit		!= NULL
		)
	{
		QString styleSheet;

		if(gpsOk == GpsReader::GPS_VALID)
		{
			styleSheet = "QLabel { color : white; }";
		}
		else
		if(gpsOk == GpsReader::GPS_INVALID)
		{
			styleSheet = "QLabel { color : yellow; }";
		}
		else
		{
			styleSheet = "QLabel { color : red; }";
		}

		SondeLabel			->setStyleSheet(styleSheet);
		SondeDistance		->setStyleSheet(styleSheet);
		SondeDistanceUnit	->setStyleSheet(styleSheet);
		SondeDirection		->setStyleSheet(styleSheet);
		SondeDirectionUnit	->setStyleSheet(styleSheet);

		LandingLabel		->setStyleSheet(styleSheet);
		Distance			->setStyleSheet(styleSheet);
		DistanceUnit		->setStyleSheet(styleSheet);
		Direction			->setStyleSheet(styleSheet);
		DirectionUnit		->setStyleSheet(styleSheet);
	}
}

//-------------------------------------------------------------------------------------------------
void MainWindow::CopyToClipboard()
{
	QClipboard*	clipBoard = QApplication::clipboard();
	if(clipBoard != NULL)
	{
		QString text = Latitude->text() + " " + Longitude->text();
		clipBoard->setText(text);
	}
}
