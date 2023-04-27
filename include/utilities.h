#ifndef UTILITIES_H
#define UTILITIES_H

#include "headers.h"
#include "shaders_handler.h"

void draw_pixel_at(int x, int y, Glib::RefPtr<Gdk::Pixbuf> pixbuf, int color[]);

int* get_pixel_at(int x, int y, Glib::RefPtr<Gdk::Pixbuf> pixbuf);

void invertir_pixbuf(Glib::RefPtr<Gdk::Pixbuf> pixbuf);

void draw_line(int x, int y_start, int y_end, Glib::RefPtr<Gdk::Pixbuf> pixbuf, int color[]);



#endif