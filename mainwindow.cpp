#include <QtCore/QDebug>
#include <QtWidgets/QTreeWidget>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCalendarWidget>
#include <QComboBox>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::syncMethodChanged()
{
	return;
}