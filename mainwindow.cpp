#include <QtCore/QDebug>
#include <QtWidgets/QTreeWidget>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCalendarWidget>
#include <QComboBox>
#include <QTimer>
#include <QDateTime>
#include <QDate>
#include <QTime>
#include <QHostInfo>
#include <QUdpSocket>

#include <iostream>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	// preset the click time to app start time
	this->ui->timeEdit->setTime(QTime::currentTime());

	// initialize time sync method
	this->syncMethodChanged();

	// udp socket
	m_udpsocket = new QUdpSocket(this);

	// signal slots
	connect(this->ui->pushButtonClick, &QPushButton::clicked, this, &MainWindow::click);
	connect(this->ui->comboBoxSync, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::syncMethodChanged);
	connect(m_udpsocket, SIGNAL(connected()), this, SLOT(connectsucess()));
	connect(m_udpsocket, SIGNAL(readyRead()), this, SLOT(readingDataGrams()));


	// clock
	m_timer = new QTimer(this);
	m_timer->setInterval(1);
	connect(m_timer, &QTimer::timeout, [&]() {
		QString timeString = this->getCurrentDateTime().toString("yyyy-MM-dd hh:mm:ss");

		this->ui->labelCurrentTime->setText(timeString);
	});
	m_timer->start();
}

MainWindow::~MainWindow()
{
	delete m_udpsocket;
	delete ui;
}



QDateTime MainWindow::getCurrentDateTime()
{
	QDateTime newDateTime = m_dateTime.addMSecs(m_timeDiff.elapsed());

	return newDateTime;
}

void MainWindow::click()
{
	this->ui->plainTextEdit->appendPlainText(m_dateTime.toString("yyyy-MM-dd hh:mm:ss"));
	this->ui->plainTextEdit->appendPlainText(QString::number(m_timeDiff.elapsed()));

	// check setted datetime is after current time
	if (ui->calendarWidget->selectedDate().daysTo(this->getCurrentDateTime().date()) > 0)
	{
		this->ui->plainTextEdit->appendPlainText(this->getCurrentDateTime().toString("yyyy-MM-dd hh:mm:ss") + ":");
		this->ui->plainTextEdit->appendPlainText("Setted date has passed, auto click abort");
		return;
	}

	if (ui->calendarWidget->selectedDate().daysTo(this->getCurrentDateTime().date()) == 0)
	{
		if (this->ui->timeEdit->time().secsTo(this->getCurrentDateTime().time()) >= 0)
		{
			this->ui->plainTextEdit->appendPlainText(this->getCurrentDateTime().toString("yyyy-MM-dd hh:mm:ss") + ":");
			this->ui->plainTextEdit->appendPlainText("Setted time has passed, auto click abort");
			return;
		}
	}

	// 


}

void MainWindow::connectSuccess()
{
	//ui->pushButton->setEnabled(false);
	//qint8 LI = 0;  //无预告
	//qint8 VN = 3;  //版本号
	//qint8 MODE = 3; //客户端几
	//qint8 STRATUM = 0;//表示本地时钟层次水平
	//qint8 POLL = 4; //测试间隔
	//qint8 PREC = -6; //表示本地时钟精度
	//QDateTime Epoch(QDate(1900, 1, 1));
	//qint32 second = quint32(Epoch.secsTo(QDateTime::currentDateTime()));
	//ui->label->setText("Connected");
	//qint32 temp = 0;
	//QByteArray timeRequest(48, 0);
	//timeRequest[0] = (LI << 6) | (VN << 3) | (MODE);
	//timeRequest[1] = STRATUM;
	//timeRequest[2] = POLL;
	//timeRequest[3] = PREC & 0xff;
	//timeRequest[5] = 1;
	//timeRequest[9] = 1;
	////40到43字节保存原始时间戳，即客户端发送的时间
	//timeRequest[40] = (temp = (second & 0xff000000) >> 24);
	//temp = 0;
	//timeRequest[41] = (temp = (second & 0x00ff0000) >> 16);
	//temp = 0;
	//timeRequest[42] = (temp = (second & 0x0000ff00) >> 8);
	//temp = 0;
	//timeRequest[43] = ((second & 0x000000ff));
	//udpsocket->flush();
	//udpsocket->write(timeRequest);
	//udpsocket->flush();
}

void MainWindow::syncMethodChanged()
{
	switch (this->ui->comboBoxSync->currentIndex())
	{
	case (system):
		{
			m_dateTime = QDateTime::currentDateTime();
			m_timeDiff.start();
			this->ui->plainTextEdit->appendPlainText(this->getCurrentDateTime().toString("yyyy-MM-dd hh:mm:ss") + ":");
			this->ui->plainTextEdit->appendPlainText("Time synchronization set to system");
			
			break;
		}
	case (hko):
		{
			m_dateTime = QDateTime::currentDateTime();

			QHostInfo info = QHostInfo::fromName("time.windows.com");
			QString ipAddress = info.addresses().first().toString();
			m_udpsocket->connectToHost("time.windows.com", 123); //ntp service use port 123

			m_timeDiff.start();
			this->ui->plainTextEdit->appendPlainText(this->getCurrentDateTime().toString("yyyy-MM-dd hh:mm:ss") + ":");
			this->ui->plainTextEdit->appendPlainText("Time synchronization set to Hong Kong Observatory");

			break;
		}
	}

	return;
}