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

#include <iostream>
#include <QtGui>
#include <MprisDetector.hpp>
#include <MprisConnection.hpp>
#include <QHttpServer.hpp>

using namespace std;

int main(int argc, char* argv[]){
	//QApplication a( argc, argv );
	QCoreApplication a(argc, argv);
	QEventLoop *qel = new QEventLoop();
	
	MprisConnection::init();
	
	QHttpServer srv(3834);
	
	
	a.exec();
}
