#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "QtWidgets/QMainWindow"
#include <QElapsedTimer> // not work with forward declare

class QTimer;
class QDateTime;

namespace Ui
{
	class MainWindow;
}

class MainWindow: public QMainWindow
{
	Q_OBJECT
	enum syncMethod 
	{
	system = 0,
	hko
	};


public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private:
	Ui::MainWindow* ui;
	QTimer* m_timer;
	QDateTime m_dateTime;
	QElapsedTimer m_timeDiff;

	QDateTime getCurrentDateTime();

private slots:
	void syncMethodChanged();
	void click();
};

#endif