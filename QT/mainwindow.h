#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>

#include "Execution.h"
#include "Position.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();

	private:
		Ui::MainWindow*	ui;

		QLabel*			SondeLabel;
		QLabel*			SondeDirection;
		QLabel*			SondeDirectionUnit;
		QLabel*			SondeDistance;
		QLabel*			SondeDistanceUnit;

		QLabel*			LandingLabel;
		QLabel*			Direction;
		QLabel*			DirectionUnit;
		QLabel*			Distance;
		QLabel*			DistanceUnit;

		QWidget*		NormalArea;
		QLabel*			Latitude;
		QLabel*			Longitude;
		QLabel*			AscentDescent;
		QPushButton*	CopyButton;
		Position::Phase	CurrentPhase;

	public slots:
		void Phase(Position::Phase		phase);
		void GpsOk(GpsReader::GPS_OK	gpsOk);
		void CopyToClipboard();
};

#endif // MAINWINDOW_H
