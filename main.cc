#include <QApplication>

#include "Hangman.h"

#ifdef _WIN32
#include <windows.h>
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE prev, LPSTR cmd, int show){
	int argc = 0;
	char **argv = NULL;
#else
int main(int argc, char **argv){
#endif // _WIN32
	QApplication app(argc, argv);

	Hangman hangman;

	return app.exec();
}
