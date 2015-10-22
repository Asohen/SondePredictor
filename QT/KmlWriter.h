#include <QTextStream>
#include <QFile>

class KmlWriter
{
	// constants ----------------------------------------------------------------------------------
	public:
	private:
		static const QString		SYM_POINT;
		static const unsigned long	COORD_DECIMAL_COUNT	= 5;

	// types --------------------------------------------------------------------------------------
	private:

	// variables ----------------------------------------------------------------------------------
	private:
		QTextStream		KmlStream;

	// functions ----------------------------------------------------------------------------------
	public:
		KmlWriter(QFile& kmlFile);
		virtual ~KmlWriter();

		void	AddKmlWaypoint
			(
				double		latitude,
				double		longitude,
				double		height,
				bool		zVelocityValid,
				double		zVelocity,
				long		timeToLanding
			);

	private:
		void	AddKmlStart();
		void	AddKmlStyle(QString& name, QString& symbol);
		void	AddKmlEnd();
};
