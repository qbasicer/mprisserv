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

#include <MprisConnection.hpp>
#include <iostream>

using namespace std;

struct PlaybackState{
	int playback;
	int a2;
	int a3;
	int a4;
};

Q_DECLARE_METATYPE(PlaybackState)

// Marshall the MyStructure data into a D-Bus argument
QDBusArgument &operator<<(QDBusArgument &argument, const PlaybackState &mystruct)
{
	argument.beginStructure();
	argument << mystruct.playback << mystruct.a2 << mystruct.a3 << mystruct.a4;
	argument.endStructure();
	return argument;
}

// Retrieve the MyStructure data from the D-Bus argument
const QDBusArgument &operator>>(const QDBusArgument &argument, PlaybackState &mystruct)
{
	argument.beginStructure();
	argument >> mystruct.playback >> mystruct.a2 >> mystruct.a3 >> mystruct.a4;
	argument.endStructure();
	return argument;
}

MprisConnection::MprisConnection(QString path):QObject(NULL){
	connectionPath = path;
	
	interval = new QTimer(this);
	connect(interval,SIGNAL(timeout()), this, SLOT(tick()));
	interval->start(1000);
}

void MprisConnection::init(){
	qDBusRegisterMetaType<PlaybackState>();
}

QMap<QString, QVariant> MprisConnection::getMetadata(){
	QDBusMessage reply2= call("/Player","GetMetadata");
	if (reply2.type() == QDBusMessage::ReplyMessage) {
		QVariantMap map =  qdbus_cast<QVariantMap>(reply2.arguments().first());

//  		QMapIterator<QString,QVariant> mitr(map);
//  		while(mitr.hasNext()){
//  			mitr.next();
//  			cout << mitr.key().toStdString() << " - " << mitr.value().toString().toStdString() << endl;
//  		}
 		return map;
		
	}else{
		cerr << "ERR invalid:" << reply2.errorMessage().toStdString() << endl;
	}
	return QVariantMap();
}

int MprisConnection::getPlaybackState(){
	QDBusMessage reply= call("/Player","GetStatus");
	if (reply.type() == QDBusMessage::ReplyMessage) {
		PlaybackState state = qdbus_cast<PlaybackState>(reply.arguments().first());
		return state.playback;
	}
	return -1;
}

QString MprisConnection::getPlayerName(){
	QDBusMessage reply= call("/","Identity");
	if (reply.type() == QDBusMessage::ReplyMessage) {
		return reply.arguments().first().toString();
	}
	return QString();
}

void MprisConnection::tick(){
	QString loc = getMetadataEntry("location");
	if(loc != location){
		location = loc;
		cout << "TrackChanged" << endl;
		emit trackChanged();
	}
}

QString MprisConnection::getMetadataEntry(QString entry){
	QMap<QString, QVariant> map = getMetadata();
	if(map.contains(entry)){
		return map.value(entry).toString();
	}
	return QString();
}

QString MprisConnection::trackTitle(){
	return getMetadataEntry("title");
}

QString MprisConnection::trackArtist(){
	return getMetadataEntry("artist");
}

void MprisConnection::next(){
	cout << "Next" << endl;
	call("/Player","Next");
}

void MprisConnection::prev(){
	cout << "Prev" << endl;
	call("/Player","Prev");
}

void MprisConnection::play(){
	cout << "Playing" << endl;
	call("/Player","Play");
}

void MprisConnection::pause(){
	cout << "Pausing" << endl;
	call("/Player","Pause");
}

void MprisConnection::stop(){
	cout << "Stopping" << endl;
	call("/Player","Stop");
}

QDBusMessage MprisConnection::call(QString path, QString method){
	QDBusInterface iface(connectionPath, path, "org.freedesktop.MediaPlayer", QDBusConnection::sessionBus());
	QDBusMessage reply=iface.call(method);
	return reply;
}

QDBusMessage MprisConnection::call(QString path, QString method, QVariant arg1){
	QDBusInterface iface(connectionPath, path, "org.freedesktop.MediaPlayer", QDBusConnection::sessionBus());
	QDBusMessage reply=iface.call(method, arg1);
	return reply;
}