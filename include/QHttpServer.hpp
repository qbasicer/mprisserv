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

#ifndef _QHTTPSERVER_HPP_
#define _QHTTPSERVER_HPP_

#include <QtCore>
#include <QTcpServer>
#include <QTcpSocket>

/* Modified from  http://doc.qt.nokia.com/solutions/4/qtservice/qtservice-example-server.html
 */


class QHttpServer : public QTcpServer {
	Q_OBJECT
	public:
		QHttpServer(quint16 port, QObject* parent = 0);
		void incomingConnection(int socket);
		void resume();
		void pause();
	private slots:
		void readClient();
		void discardClient();
	private:
		void insertCommand(QString dbus, QString name, QString cmd, QTextStream& os);
		bool disabled;
};

#endif
