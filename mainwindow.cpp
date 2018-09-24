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

#include <iostream>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	// clock
	m_timer = new QTimer(this);
	m_timer->setInterval(1);
	connect(m_timer, &QTimer::timeout, [&]() {
		QString timeString = QDateTime::currentDateTime().toString("h:mm:ss AP");

		this->ui->labelCurrentTime->setText(timeString);
	});
	m_timer->start();

	// signal slots
	connect(this->ui->pushButtonClick, &QPushButton::clicked, this, &MainWindow::click);

}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::click()
{
	QString dateTimeString;
	dateTimeString = dateTimeString + this->ui->calendarWidget->selectedDate().toString("yyyy-MM-dd") + " ";
	dateTimeString = dateTimeString + this->ui->labelCurrentTime->text() + ":";

	this->ui->plainTextEdit->appendPlainText(dateTimeString);

}

void MainWindow::syncMethodChanged()
{
	return;
}