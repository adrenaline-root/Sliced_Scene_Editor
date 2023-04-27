#include "../include/mainwindow.h"
#include <gtkmm/application.h>

int main (int argc, char *argv[])
{
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.gtkmm.example");
	

	MainWindow *mainwindow = nullptr;
	//MainWindow main;
	
	try {
		auto builder = Gtk::Builder::create_from_file("source-files/styles/index.glade");
		builder->get_widget_derived("MainWindow", mainwindow);
	}
	catch (const Glib::Error &e) {
		std::cerr << e.what() << std::endl;
	}

	auto css = Gtk::CssProvider::create();

	try {
		css->load_from_path("source-files/styles/styles.css");

	} catch (const Glib::Error &e) {
		std::cout << e.what() << std::endl;
	}
	Gtk::StyleContext::add_provider_for_screen(Gdk::Screen::get_default(), css, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	

	//Shows the window and returns when it is closed.
	return app->run(*mainwindow);
}