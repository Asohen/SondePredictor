#ifndef PredictionTrackReader_H
#define PredictionTrackReader_H

#include <QTextStream>
#include <QFile>
#include <QList>

#include "Position.h"


class PredictionTrackReader
{
	// constants ----------------------------------------------------------------------------------
	private:
		static const QString		SEPARATOR;

	// types --------------------------------------------------------------------------------------
	private:

	// variables ----------------------------------------------------------------------------------
	public:
		QList		<Position>	Prediction;					// input file content

	private:
		QFile&		InputFile;								// input file from http://predict.habhub.org
		long		BurstIndex;								// prediction index of burst position
		long long	StartTime;								// sonde start time, lift off

	// functions ----------------------------------------------------------------------------------
	public:
		PredictionTrackReader
			(
				QFile&	inputFile							// input file
			);

		bool	ReadAll();
		bool	GetPredictedBurstPosition					// get burst height from http://predict.habhub.org
			(												// return: burst height available
				Position&	burstPosition					// out:	predicted burst position
			);
		bool	GetPredictedLandingPosition					// get landing position from http://predict.habhub.org
			(												// return: landing position available
				Position&	landingPosition					// out:	predicted landing position
			);
		bool	GetPredictedCurrentPosition					// get predicted position at current phase and height
			(												// return: predictionIndex available
				Position&		PredictedPosition,			// out:	predicted position (interpolated), valid if result = true
				Position::Phase	currentPhase,				// in:	current phase
				double			currentHeight				// in:	current height
			);

	private:
		bool	GetNextPosition
			(
				Position&	position,						// out:	position
				bool		start							// 1st call
			);
		void	AddZVelocity(Position& position);
		double	Interpolation
			(
				double	start,
				double	end,
				double	part
			);
};

#endif
