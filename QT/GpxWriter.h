#include <QTextStream>
#include <QFile>

class GpxWriter
{
	// constants ----------------------------------------------------------------------------------
	public:
		static const QString	FILE_CSV_EXT;
		static const QString	FILE_GPX_EXT;

	private:
		static const QString	SYM_POINT;

		// Garmin eTrex 30 symbols
		static const QString	SYM_GARMIN_ETREX30_CYCLE_WHITE;	// "Navaid, White"		Navaid = Seezeichen
		static const QString	SYM_GARMIN_ETREX30_CYCLE_BLUE;	// "Navaid, Blue"
		static const QString	SYM_GARMIN_ETREX30_CYCLE_GREEN;	// "Navaid, Green"
		static const QString	SYM_GARMIN_ETREX30_CYCLE_RED;	// "Navaid, Red"
		static const QString	SYM_GARMIN_ETREX30_CYCLE_BLACK;	// "Navaid, Black"
		static const QString	SYM_GARMIN_ETREX30_BLOCK_GREEN;	// "Block, Green"
		static const QString	SYM_GARMIN_ETREX30_BLOCK_RED;	// "Block, Red"
		static const QString	SYM_GARMIN_ETREX30_RESIDENCE;	// "Residence"
		static const QString	SYM_GARMIN_ETREX30_DOT_GREEN;	// "City (Large)"
		static const QString	SYM_GARMIN_ETREX30_BRIDGE;		// "Bridge"

		// used symbols
		static const QString&	SYM_GPX_POINT;

		static const unsigned long	COORD_DECIMAL_COUNT	= 5;

	// types --------------------------------------------------------------------------------------
	private:

	// variables ----------------------------------------------------------------------------------
	private:
		QTextStream		GpxStream;
		qint64			LastEndPosition;

	// functions ----------------------------------------------------------------------------------
	public:
		GpxWriter(QFile& gpxFile);
		virtual ~GpxWriter();

		void	AddGpxStart();
		void	AddGpxWaypoint
			(
				double		latitude,
				double		longitude,
				double		height,
				bool		zVelocityValid,
				double		zVelocity,
				long		timeToLanding
			);
		void	AddGpxEnd();
};
