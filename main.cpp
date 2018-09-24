#include <QApplication>
#include <mainwindow.h>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	QIcon icon(":/icon/click.ico");
	app.setWindowIcon(icon);
	MainWindow w;
	w.show();
	
	return app.exec();
}