#include "../include/utilities.h"

void draw_pixel_at(int x, int y, Glib::RefPtr<Gdk::Pixbuf> pixbuf, int color[])
{
	unsigned int index = y * pixbuf->get_rowstride() + x * pixbuf->get_n_channels();
	guint8 *pixels = pixbuf->get_pixels();
		
	if (index < pixbuf->get_byte_length() && x < pixbuf->get_width() && y < pixbuf->get_height()
		&& x >= 0 && y >= 0) {
		
		if (pixels[index + 3] == 0) {
			pixels[index] = color[0];
			pixels[index + 1] = color[1];
			pixels[index + 2] = color[2];
			pixels[index + 3] = color[3];
			
		}
	
	}
}

int* get_pixel_at(int x, int y, Glib::RefPtr<Gdk::Pixbuf> pixbuf)
{
	int *returned = nullptr;
	
	if (x >= 0 && x < pixbuf->get_width() && y >= 0 && y < pixbuf->get_height()) { 
		
		unsigned int index = y * pixbuf->get_rowstride() + x * pixbuf->get_n_channels();
		guint8 *pixels = pixbuf->get_pixels();
		
		returned = new int[4]{pixels[index], pixels[index + 1], pixels[index + 2], pixels[index + 3]};
	
	}
	
	return returned;  
}

void invertir_pixbuf(Glib::RefPtr<Gdk::Pixbuf> pixbuf)
{
	guint8 *pixels = pixbuf->get_pixels();
	
	for (int y = 0; y < pixbuf->get_height() / 2; y++) {
		for(int x = 0; x < pixbuf->get_rowstride(); x++) {
			
			int index_at = y * pixbuf->get_rowstride() + x;
			int index_inverse = (pixbuf->get_height() - 1 - y) * pixbuf->get_rowstride() + x;
			
			guint8 value = pixels[index_at];
			pixels[index_at] = pixels[index_inverse];
			pixels[index_inverse] = value;

		}
	}
}

void draw_line(int x, int y_start, int y_end, Glib::RefPtr<Gdk::Pixbuf> pixbuf, int color[]) 
{
	unsigned int start_index = y_start * pixbuf->get_rowstride() + x * pixbuf->get_n_channels();
	unsigned int end_index = y_end * pixbuf->get_rowstride() + x * pixbuf->get_n_channels();
	
	guint8 *pixels = pixbuf->get_pixels();
	
	if (start_index < pixbuf->get_byte_length() && end_index < pixbuf->get_byte_length() - 4 
		&& y_start > 0 && y_end < pixbuf->get_height()) {
		for (unsigned int i = start_index; i < end_index; i += pixbuf->get_rowstride()) {
			if(pixels[i + 3] == 0) {
				for (int c = 0; c < 4; c++) pixels[i + c] = color[c];
			}
		}
	}
}




