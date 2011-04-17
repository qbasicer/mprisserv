#include <QHttpServer.hpp>
#include <MprisConnection.hpp>
#include <MprisDetector.hpp>

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

/* Modified from  http://doc.qt.nokia.com/solutions/4/qtservice/qtservice-example-server.html
 */

QHttpServer::QHttpServer(quint16 port, QObject* parent) : QTcpServer(parent), disabled(false){
	listen(QHostAddress::Any, port);
}

void QHttpServer::incomingConnection(int socket){
	if (disabled)
		return;

	// When a new client connects, the server constructs a QTcpSocket and all
	// communication with the client is done over this QTcpSocket. QTcpSocket
	// works asynchronously, this means that all the communication is done
	// in the two slots readClient() and discardClient().
	QTcpSocket* s = new QTcpSocket(this);
	connect(s, SIGNAL(readyRead()), this, SLOT(readClient()));
	connect(s, SIGNAL(disconnected()), this, SLOT(discardClient()));
	s->setSocketDescriptor(socket);

}

void QHttpServer::pause(){
	disabled = true;
}

void QHttpServer::resume()
{
	disabled = false;
}


void QHttpServer::insertCommand(QString dbus, QString name, QString cmd, QTextStream& os){
	os << "<a href=\"/?path=" << dbus << "&cmd=" << cmd << "\">" << name << "</a>";
}

void QHttpServer::readClient(){
	if (disabled)
		return;

	// This slot is called when the client sent data to the server. The
	// server looks if it was a get request and sends a very simple HTML
	// document back.
	QTcpSocket* socket = (QTcpSocket*)sender();
	if (socket->canReadLine()) {
		QStringList tokens = QString(socket->readLine()).split(QRegExp("[ \r\n][ \r\n]*"));
		if (tokens[0] == "GET") {
			
			QTextStream os(socket);
			os.setAutoDetectUnicode(true);
			os << "HTTP/1.0 200 Ok\r\n"
				"Content-Type: text/html; charset=\"utf-8\"\r\n"
				"\r\n";
			
			if(tokens[1].startsWith("/?")){
				QString query = tokens[1].right(tokens[1].length() - 2);
				QMap<QString,QString> map;
				QStringList queries = query.split("&");
				foreach(QString q, queries){
					QStringList p = q.split("=");
					map.insert(p[0],p[1]);
				}
				if(map.contains("path") && map.contains("cmd")){
					MprisDetector md;
					QStringList names = md.getNames();
					foreach (QString name, names){
						if(name == map.value("path")){
							MprisConnection con(name);
							QString cmd = map.value("cmd");
							if(cmd == "next"){
								con.next();
							}else if(cmd == "prev"){
								con.prev();
							}else if(cmd == "play"){
								con.play();
							}else if(cmd == "pause"){
								con.pause();
							}else if(cmd == "stop"){
								con.stop();
							}
						}
					}
				}
				usleep(500000);
			}
			MprisDetector md;
			QStringList names = md.getNames();
			foreach (QString name, names){
				MprisConnection con(name);
				int playbackState = con.getPlaybackState();
				//new MprisConnection(name
				os << "<head>\r\n<title>MPRISServ</title>\r\n";
				os << "<meta http-equiv=\"refresh\" content=\"30; url=/\">\r\n";
				os << "<STYLE type=\"text/css\">\r\n";
				os << "body{size: 12pt;}\r\n";
				os << "</STYLE>\r\n";
				os << "<meta name=\"viewport\" content=\"width=device-width\" />\r\n";
				os << "</head>\r\n<body>\r\n";
				os << "<h1>" << con.getPlayerName() << "</h1>\r\n";
				os << "<h3>" << con.trackArtist() << "&nbsp;-&nbsp;" << con.trackTitle() << "</h3>\r\n";
				insertCommand(name,"Prev","prev", os);
				os << "&nbsp;&nbsp;/&nbsp;&nbsp;";
				if(playbackState == 0){
					insertCommand(name,"Pause","pause", os);
					os << "&nbsp;&nbsp;/&nbsp;&nbsp;";
					insertCommand(name,"Stop","stop", os);
					os << "&nbsp;&nbsp;/&nbsp;&nbsp;";
				}else if(playbackState == 1){
					insertCommand(name,"Play","play", os);
					os << "&nbsp;&nbsp;/&nbsp;&nbsp;";
					insertCommand(name,"Stop","stop", os);
					os << "&nbsp;&nbsp;/&nbsp;&nbsp;";
				}else if(playbackState == 2){
					insertCommand(name,"Play","play", os);
					os << "&nbsp;&nbsp;/&nbsp;&nbsp;";
				}
				insertCommand(name,"Next","next", os);
				os << "&nbsp;&nbsp;/&nbsp;&nbsp;";
				insertCommand(name,"Refresh","", os);
				
			}
			os << "</body>\r\n";
			socket->close();
			if (socket->state() == QTcpSocket::UnconnectedState) {
				delete socket;
			}
		}
	}
}

void QHttpServer::discardClient(){
	QTcpSocket* socket = (QTcpSocket*)sender();
	socket->deleteLater();

}