#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "QtWidgets/QMainWindow"

class QTimer;

namespace Ui
{
	class MainWindow;
}

class MainWindow: public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private:
	Ui::MainWindow* ui;
	QTimer* m_timer;

private slots:
	void syncMethodChanged();
	void click();
};

#endif