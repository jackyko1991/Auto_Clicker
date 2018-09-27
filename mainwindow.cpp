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
	this->ui->timeEdit->setDateTime(QDateTime::currentDateTime());

	// initialize time sync method
	this->syncMethodChanged();

	// udp socket
	m_udpsocket = new QUdpSocket(this);

	// signal slots
	connect(this->ui->pushButtonClick, &QPushButton::clicked, this, &MainWindow::click);
	connect(this->ui->comboBoxSync, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::syncMethodChanged);
	connect(this->m_udpsocket, &QUdpSocket::connected, this, &MainWindow::connectSuccess);
	connect(this->m_udpsocket, &QUdpSocket::readyRead, this, &MainWindow::readinigDataGrams);

	// click status
	m_clickerStatus = false;

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

void MainWindow::enableUi(bool enable)
{
	this->ui->timeEdit->setEnabled(enable);
	this->ui->comboBoxSync->setEnabled(enable);
	
	if (enable)
		this->ui->pushButtonClick->setText("Auto Click!!!");
	else
		this->ui->pushButtonClick->setText("Cancel");
}

void MainWindow::click()
{
	if (m_clickerStatus)
	{
		this->ui->plainTextEdit->appendPlainText(this->getCurrentDateTime().toString("yyyy-MM-dd hh:mm:ss") + ":");
		this->ui->plainTextEdit->appendPlainText("Auto click cancelled");

		this->enableUi(true);
		m_clickerStatus = false;

		return;
	}

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

	// lock ui
	this->enableUi(false);
	m_clickerStatus = true;

	this->ui->plainTextEdit->appendPlainText(this->getCurrentDateTime().toString("yyyy-MM-dd hh:mm:ss") + ":");
	this->ui->plainTextEdit->appendPlainText("Auto click counting down, hover the cursor on the place to click");

	m_clickerConnection = connect(m_timer, &QTimer::timeout, [&]() {


		if (this->getCurrentDateTime().msecsTo(this->ui->timeEdit->dateTime()) < 0)
		{
			/*QString timeString = this->getCurrentDateTime().toString("yyyy-MM-dd hh:mm:ss");*/
			this->ui->plainTextEdit->appendPlainText(this->getCurrentDateTime().toString("yyyy-MM-dd hh:mm:ss") + ":");
			this->ui->plainTextEdit->appendPlainText("Click!!!");
			// unlock ui
			this->enableUi(true);
			m_clickerStatus = false;

			disconnect(m_clickerConnection);
			return;
			
		}


	});


}

void MainWindow::connectSuccess()
{
	qint8 LI = 0; //display level
	qint8 VN = 3; //version number
	qint8 MODE = 3; //client side
	qint8 STRATUM = 0; //local clock startum
	qint8 POLL = 4; //test interval
	qint8 PREC = -6; //local clock precision
	QDateTime Epoch(QDate(1900, 1, 1));
	qint32 second = quint32(Epoch.secsTo(QDateTime::currentDateTime()));

	//ui->label->setText("Connected");
	qint32 temp = 0;
	QByteArray timeRequest(48, 0);
	timeRequest[0] = (LI << 6) | (VN << 3) | (MODE);
	timeRequest[1] = STRATUM;
	timeRequest[2] = POLL;
	timeRequest[3] = PREC & 0xff;
	timeRequest[5] = 1;
	timeRequest[9] = 1;
	//40-43 client side time
	timeRequest[40] = (temp = (second & 0xff000000) >> 24);
	temp = 0;
	timeRequest[41] = (temp = (second & 0x00ff0000) >> 16);
	temp = 0;
	timeRequest[42] = (temp = (second & 0x0000ff00) >> 8);
	temp = 0;
	timeRequest[43] = ((second & 0x000000ff));
	m_udpsocket->flush();
	m_udpsocket->write(timeRequest);
	m_udpsocket->flush();
}

void MainWindow::readinigDataGrams()
{
	//ui->label->setText("Reading...");
	QByteArray newTime;
	QDateTime Epoch(QDate(1900, 1, 1));
	QDateTime unixStart(QDate(1970, 1, 1));
	do
	{
		newTime.resize(m_udpsocket->pendingDatagramSize());
		m_udpsocket->read(newTime.data(), newTime.size());
	} while (m_udpsocket->hasPendingDatagrams());
	QByteArray TransmitTimeStamp;
	TransmitTimeStamp = newTime.right(8);
	quint32 seconds = TransmitTimeStamp[0];
	quint8 temp = 0;
	for (int j = 1; j <= 3; j++)
	{
		seconds = seconds << 8;
		temp = TransmitTimeStamp[j];
		seconds = seconds + temp;
	}
	m_dateTime.setTime_t(seconds - Epoch.secsTo(unixStart));
	m_timeDiff.restart();
	m_udpsocket->disconnectFromHost();
	m_udpsocket->close();
}

void MainWindow::syncMethodChanged()
{
	switch (this->ui->comboBoxSync->currentIndex())
	{
	case (system):
		{
			m_dateTime = QDateTime::currentDateTime();
			m_timeDiff.restart();
			this->ui->plainTextEdit->appendPlainText(this->getCurrentDateTime().toString("yyyy-MM-dd hh:mm:ss") + ":");
			this->ui->plainTextEdit->appendPlainText("Time synchronization set to system");
			
			break;
		}
	case (hko):
		{
			//m_dateTime = QDateTime::currentDateTime();

			//QHostInfo info = QHostInfo::fromName("time.windows.com");
			QHostInfo info = QHostInfo::fromName("time.hko.hk");

			switch (info.error())
			{
			case(QHostInfo::NoError):
			{
				break;
			}
			case(QHostInfo::HostNotFound):
			case(QHostInfo::UnknownError):
			{
				this->ui->plainTextEdit->appendPlainText(this->getCurrentDateTime().toString("yyyy-MM-dd hh:mm:ss") + ":");
				this->ui->plainTextEdit->appendPlainText("Time synchronization server connection failed");

				this->ui->comboBoxSync->setCurrentIndex(0);
				return;
				break;
			}
			}


			std::cout << info.error() << std::endl;
			QString ipAddress = info.addresses().first().toString();
			//m_udpsocket->connectToHost("time.windows.com", 123); //ntp service use port 123
			m_udpsocket->connectToHost("time.hko.hk", 123); //ntp service use port 123

			//m_timeDiff.restart();
			this->ui->plainTextEdit->appendPlainText(this->getCurrentDateTime().toString("yyyy-MM-dd hh:mm:ss") + ":");
			this->ui->plainTextEdit->appendPlainText("Time synchronization set to Hong Kong Observatory");

			break;
		}
	}

	return;
}