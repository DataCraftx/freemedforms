#ifndef RAPIDTOFREEIO_H
#define RAPIDTOFREEIO_H

#include <QObject>


class RapidToFreeIO:public QObject
{
	public:
		RapidToFreeIO (QObject * parent = 0);
		virtual ~RapidToFreeIO ();
		bool initialiseBases();

	private:
		bool connectToRapidComptamed();
};

#endif

