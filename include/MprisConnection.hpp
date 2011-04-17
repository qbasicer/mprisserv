/*
 * This file is part of MPRISServ
 * Copyright (C) 2011  Cody Harris
 * 
 * MPRISServ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _MPRIS_CONNECTION_HPP_
#define _MPRIS_CONNECTION_HPP_

#include <QtCore>
#include <QtDBus/QtDBus>

class MprisConnection : public QObject{
	Q_OBJECT;
	public:
		MprisConnection(QString path);
		QString trackTitle();
		QString trackArtist();
		QString getPlayerName();
		int getPlaybackState();
		static void init();
	public slots:
		void next();
		void prev();
		void play();
		void pause();
		void stop();
	protected slots:
		void tick();
	signals:
		void trackChanged();
	protected:
		QDBusMessage call(QString path, QString method);
		QDBusMessage call(QString path, QString method, QVariant arg1);
		QMap<QString, QVariant> getMetadata();
		QString getMetadataEntry(QString entry);
		
		QString location;
		
		QString connectionPath;
		QTimer *interval;
};

#endif
