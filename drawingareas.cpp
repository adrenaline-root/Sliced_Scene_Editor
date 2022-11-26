#include "drawingareas.h"


#define PI 3.14159265358979323846

//==================================
//======= METODOS DE GL_AREA =======
//==================================

glDrawingArea::glDrawingArea()
{
	set_double_buffered(false);
	add_events(Gdk::KEY_PRESS_MASK | Gdk::KEY_RELEASE_MASK | Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK);
	
	set_can_focus(true);
	set_sensitive(true);

	signal_realize().connect(sigc::mem_fun(*this, &glDrawingArea::attach_xwindow));
	signal_draw().connect(sigc::mem_fun(*this, &glDrawingArea::on_my_draw));
}


glDrawingArea::glDrawingArea(BaseObjectType* obj, Glib::RefPtr<Gtk::Builder> const& builder)
	: Gtk::DrawingArea(obj)
	, builder{builder}
{
	set_double_buffered(false);
	add_events(Gdk::KEY_PRESS_MASK | Gdk::KEY_RELEASE_MASK | Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK);
	
	set_can_focus(true);
	set_sensitive(true);
	
	signal_realize().connect(sigc::mem_fun(*this, &glDrawingArea::attach_xwindow));
	signal_draw().connect(sigc::mem_fun(*this, &glDrawingArea::on_my_draw));

}

glDrawingArea::~glDrawingArea()
{
}

bool glDrawingArea::on_my_draw(const Cairo::RefPtr<Cairo::Context> &cr)
{
	glClearColor(0.2, 0.2, 0.2, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	if (attached == true)
	{
		glXMakeCurrent(xdisplay, win, glc);
		//glEnable(GL_DEPTH_TEST); 

		XGetWindowAttributes(xdisplay, win, &gwa);
		glViewport(0, 0, gwa.width, gwa.height);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		GLdouble orto = gwa.width; //get_parent()->get_width();
		GLdouble ortov = gwa.height; //get_parent()->get_height();

		float z_aspect_correction = 1.0 / std::sin(45 * PI / 180);
		
		// Perspectiva ortogonal: 
		//glOrtho( -orto / 2, orto / 2, -ortov / 2 / z_aspect_correction, ortov / 2 / z_aspect_correction, -ortov / 2, ortov / 2);
		
		// Perspectiva normal: (estrechamiento del eje z -profundidad-);
		glOrtho( -orto / 2, orto / 2, -ortov / 2 , ortov / 2, -ortov / 2, ortov / 2);
		//glOrtho(0.0, orto, 0.0, ortov, -1.0, ortov);
		
		glRotatef(45.0, 1.0, 0.0, 0.0);
		glRotatef(rotation, 0.0, 1.0, 0.0);
		//float x_aspect_correction = 1.0 / std::cos(45 * PI / 180);
		glScalef(scale, scale, scale);
		glScalef(1.0, z_aspect_correction, 1.0);
		
				
		glTranslatef(tranlation_x, 0.0, tranlation_y);
		

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glLineWidth(1.0);
		drawRejilla();
		//DrawPoints();
		//glLineWidth(2.0);
		Draw_ejes();
		
		
		glColor3f(1.0, 1.0, 1.0);
		
		if (int(scene_handler->sliced_objects.size()) > 0) {
			
			// Renderizando la escena con el basic shader
			
			scene_handler->shader_handler.use_program();
			
			int gen_textures_size = scene_handler->texture_handler.generated_textures.size();
			GLuint texture = scene_handler->texture_handler.generated_textures[gen_textures_size - 1];
			
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texture);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			
			scene_handler->shader_handler.set_viewport_matrix({int(orto), int(ortov), int(ortov)});
			scene_handler->shader_handler.set_mat4("viewport_matrix", scene_handler->shader_handler.viewport_matrix);
			
			scene_handler->shader_handler.set_rotation_matrix(rotation);
			scene_handler->shader_handler.set_mat4("rotation_matrix", scene_handler->shader_handler.rotation_matrix);
			
			scene_handler->shader_handler.set_view_rotation_matrix(45);
			scene_handler->shader_handler.set_mat4("view_rotation_matrix", scene_handler->shader_handler.view_rotation_matrix);
			
			scene_handler->shader_handler.set_trans_scale_matrix(scale, {tranlation_x, tranlation_y});
			scene_handler->shader_handler.set_mat4("trans_scale_matrix", scene_handler->shader_handler.trans_scale_matrix);
			
			scene_handler->shader_handler.set_sampler("c_texture", 0);
			scene_handler->shader_handler.set_sampler("n_texture", 1);
			
			
			
			float cos90 = float(cos(double(90) * PI / double(180)));
			float sin90 = float(sin(double(90) * PI / double(180)));
			
			GLfloat rot_norm90[] = {cos90, 0, sin90, 0, // rotación eje y
									0, 1, 0, 0, 
									-sin90, 0, cos90, 0,
									0, 0, 0, 1};
									
			float cos180 = float(cos(double(180) * PI / double(180)));
			float sin180 = float(sin(double(180) * PI / double(180)));
			
			GLfloat rot_norm180[] = {cos180, 0, sin180, 0, // rotación eje y
									0, 1, 0, 0, 
									-sin180, 0, cos180, 0,
									0, 0, 0, 1};
									
			float cos270 = float(cos(double(270) * PI / double(180)));
			float sin270 = float(sin(double(270) * PI / double(180)));
			
			GLfloat rot_norm270[] = {cos270, 0, sin270, 0, // rotación eje y
									0, 1, 0, 0, 
									-sin270, 0, cos270, 0,
									0, 0, 0, 1};
			
			GLint mat_rot90_loc = glGetUniformLocation(scene_handler->shader_handler.program, "mat_rot90");
			GLint mat_rot180_loc = glGetUniformLocation(scene_handler->shader_handler.program, "mat_rot180");
			GLint mat_rot270_loc = glGetUniformLocation(scene_handler->shader_handler.program, "mat_rot270");
			
			glUniformMatrix4fv(mat_rot90_loc, 1, GL_FALSE, rot_norm90);
	        glUniformMatrix4fv(mat_rot180_loc, 1, GL_FALSE, rot_norm180);
			glUniformMatrix4fv(mat_rot270_loc, 1, GL_FALSE, rot_norm270);
			

			scene_handler->shader_handler.check_gl_errors();
			
			glDrawElements(GL_TRIANGLES, int(scene_handler->layered_mesh.index_pool.size()), GL_UNSIGNED_INT, nullptr);
			
			glDisable(GL_TEXTURE_2D);
				
		}
		
		scene_handler->shader_handler.unuse_program();
		
		drawBoxSelector();
		
		glXSwapBuffers(xdisplay, win);

	}

	return true;
}

void glDrawingArea::outlineSelectedModel(Model model)
{
	
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	
	if (model.vertexPool.size() > 0)
		{
			for (unsigned int i = 0; i < model.indexPool.size(); i += 3) {
				int indice1 = model.indexPool[i];
				int indice2 = model.indexPool[i + 1];
				int indice3 = model.indexPool[i + 2];
				
				glColor3f(1.0, 0.0, 0.0);
				glBegin(GL_TRIANGLES);
					glVertex3f(model.vertexPool[indice1 * 3], model.vertexPool[indice1 * 3 + 1], model.vertexPool[indice1 * 3 + 2]);
					glVertex3f(model.vertexPool[indice2 * 3], model.vertexPool[indice2 * 3 + 1], model.vertexPool[indice2 * 3 + 2]);
					glVertex3f(model.vertexPool[indice3 * 3], model.vertexPool[indice3 * 3 + 1], model.vertexPool[indice3 * 3 + 2]);
				glEnd();
			}
		}
		
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        //glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glEnable(GL_LINE_SMOOTH);
		glLineWidth(3.0);
		

		if (model.vertexPool.size() > 0)
		{
			for (unsigned int i = 0; i < model.indexPool.size(); i += 3) {
				int indice1 = model.indexPool[i];
				int indice2 = model.indexPool[i + 1];
				int indice3 = model.indexPool[i + 2];
				
				glColor3f(1.0, 0.7, 0.0);
				glBegin(GL_LINE_LOOP);
					glVertex3f(model.vertexPool[indice1 * 3], model.vertexPool[indice1 * 3 + 1], model.vertexPool[indice1 * 3 + 2]);
					glVertex3f(model.vertexPool[indice2 * 3], model.vertexPool[indice2 * 3 + 1], model.vertexPool[indice2 * 3 + 2]);
					glVertex3f(model.vertexPool[indice3 * 3], model.vertexPool[indice3 * 3 + 1], model.vertexPool[indice3 * 3 + 2]);
				glEnd();
			}
		}
		
		glDisable(GL_LINE_SMOOTH);
		glDisable(GL_STENCIL_TEST);
}

void glDrawingArea::readPixelsToPixbuff()
{
	data = new float[gwa.width * gwa.height * sizeof(float)];
	glReadPixels(0, 0, gwa.width, gwa.height, GL_RGBA, GL_FLOAT, data);
	
	Cairo::RefPtr<Cairo::ImageSurface> ImagetoBuff = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, gwa.width, gwa.height);
	bufferdata = Gdk::Pixbuf::create(ImagetoBuff, 0, 0, ImagetoBuff->get_width(), ImagetoBuff->get_height());
	
	int buffsize = bufferdata->get_width() * bufferdata->get_height() * sizeof(float);
	int bufflinesize = bufferdata->get_width() * sizeof(float);
	int datay = bufferdata->get_height() - 1;
			
	for (int i = 0; i < buffsize; i++)
	{
		int datax = i % bufflinesize;
		int indice = datax + datay * bufflinesize;
		bufferdata->get_pixels()[indice] = guint (*(data + i) * 255);
		if (datax == bufflinesize - 1) {datay -= 1;}
	}
	
	//td::cout << "glContextSave to Pixbuff" << std::endl;
	//bufferdata->save("/home/usuario/Pruebas/GLContext", "png");
	
}

bool glDrawingArea::on_event(GdkEvent *evento)
{
	if (evento->any.type == GDK_BUTTON_PRESS) {
		grab_focus();
	}
	
	if (is_focus()) {
		if (evento->any.type == GDK_KEY_PRESS) {
			on_key_press_event(&evento->key);
		}
	}

	return true;
}


bool glDrawingArea::on_key_press_event(GdkEventKey *keyevent) 
{
	int rotSpeed = 5;
	int trSpeed = 15 / scale;
	float scaleInc = 0.05;
	
	rotation = int(rotation) % 360;
	int mrotation = rotation;
	
	if (mrotation < 0) mrotation = 360 + rotation;
	
	if (keyevent->state == GDK_CONTROL_MASK) 
	{
		switch(keyevent->keyval)
		{
			case GDK_KEY_Left:
				rotation += rotSpeed;
				break;
			
			case GDK_KEY_Right:
				rotation -= rotSpeed;
				break;
				
			case GDK_KEY_Up:
				scale += scaleInc;
				break;
				
			case GDK_KEY_Down:
				scale -= scaleInc;
				break;
		}
	}
	
	else if (keyevent->state == GDK_SHIFT_MASK)
	{
		float radians = float((rotation) * PI / 180);
	
		switch(keyevent->keyval)
		{
			case GDK_KEY_Left:
				tranlation_x -= (std::cos(radians) * trSpeed);
				tranlation_y -= (std::sin(radians) * trSpeed);
				                                            
				break;                                      
			                                                
			case GDK_KEY_Right:                             
				tranlation_x += (std::cos(radians) * trSpeed);
				tranlation_y += (std::sin(radians) * trSpeed);
				break;
				
			case GDK_KEY_Up:
				tranlation_y -= (std::cos(radians) * trSpeed);
				tranlation_x += (std::sin(radians) * trSpeed);
				break;
			
			case GDK_KEY_Down:
				tranlation_y += (std::cos(radians) * trSpeed);
				tranlation_x -= (std::sin(radians) * trSpeed);
				break;
				
				
		}
	}
	
	else {
		
		int dx = rejillaSize;
		int dy = rejillaSize;
		
		switch (int(mrotation / 45)) {
			case 7:
			case 0:
				dx = dx;
				dy = 0;
				break;
			case 1:
			case 2:
				dy = -dx;
				dx = 0;
				break;
			case 3:
			case 4:
				dx = -dx;
				dy = 0;
				break;
			case 5:
			case 6:
				dy = dx;
				dx = 0;
				break;
				
		}
		
		
		if (keyevent->keyval == guint(65293)) {
			
			if (scene_handler->action == ROTATE) scene_handler->action = NO_ACTION;
			if (scene_handler->action == MOVE) scene_handler->action = NO_ACTION;
		}
		
		// SI NO HAY NINGUNA ACCION PRODUCCIENDOSE O SE ESTA PRODUCIENDO UN MOVIMIENTO
		
		if (scene_handler->action == NO_ACTION || scene_handler->action == MOVE) {
			
			instance *inst = NULL;
			
			if (scene_handler->action == MOVE) inst = scene_handler->get_seleted_instance(boxSelector.position);
			
		
			switch(keyevent->keyval)
			{
				case GDK_KEY_Left:
					boxSelector.move(-dx, 0, dy);
					break;
					
				case GDK_KEY_Right:
					boxSelector.move(dx, 0, -dy);
					break;
					
				case GDK_KEY_Up:
					if (keyevent->state == GDK_CONTROL_MASK + GDK_SHIFT_MASK) {
						boxSelector.move(0, rejillaSize, 0);
					}
					else {
						boxSelector.move(-dy, 0, -dx);
					}
					break;
					
				case GDK_KEY_Down:
					if (keyevent->state == GDK_CONTROL_MASK + GDK_SHIFT_MASK) {
						boxSelector.move(0, -rejillaSize, 0);
					}
					else {
						boxSelector.move(dy, 0, dx);
					}
					break;
					
			}
				
			if (inst != NULL) {
				inst->position = boxSelector.position;
				scene_handler->add_slices_to_layered_mesh();
				scene_handler->shader_handler.load_mesh_to_gl_buffers(scene_handler->layered_mesh);
			}
				
			
		}
		
		else if (scene_handler->action == ROTATE) {
			
			bool got_instance = false;
			
			instance *inst = scene_handler->get_seleted_instance(boxSelector.position);
			
			if (inst != NULL) got_instance = true;
			
			//int rot = 0;
			
			//if (got_instance) rot = inst->rotation;
			
			switch(keyevent->keyval)
			{
				case GDK_KEY_Left:
					if (got_instance) inst->rotation -= 1;
					break;
					
				case GDK_KEY_Right:
					if (got_instance) inst->rotation += 1;
					break;
			}
			
			
			if (got_instance) {
				inst->rotation = inst->rotation % 4;
				if (inst->rotation < 0) inst->rotation = 4 + inst->rotation;
				scene_handler->add_slices_to_layered_mesh();
				scene_handler->shader_handler.load_mesh_to_gl_buffers(scene_handler->layered_mesh);
			}
			
			else scene_handler->action = NO_ACTION;
		}
	}
	
	
	queue_draw();
	
	return true;
}


void glDrawingArea::attach_xwindow()
{
	Gtk::Allocation allocation = get_allocation();
	memset(&attributes, 0, sizeof(attributes));
	
	std::cout << "attaching window..." << std::endl;

	attributes.x = allocation.get_x();
	attributes.y = allocation.get_y();
	attributes.width = allocation.get_width();
	attributes.height = allocation.get_height();

	attributes.window_type = GDK_WINDOW_CHILD;
	attributes.wclass = GDK_INPUT_OUTPUT;

	parent = get_window()->gobj();
	display = gdk_window_get_display(parent);
	xdisplay = GDK_DISPLAY_XDISPLAY(display);
	
	root = DefaultRootWindow(xdisplay);
	vi = glXChooseVisual(xdisplay, 0, att);

	win = GDK_WINDOW_XID(parent);
	XMapWindow(xdisplay, win);

	glc = glXCreateContext(xdisplay, vi, NULL, GL_TRUE);

	
	boxSelector = Model();
	boxSelector.createCube(rejillaSize);
	
	attached = true;
	std::cout << "window attached..." << std::endl;
	
	glXMakeCurrent(xdisplay, win, glc);
	
	scene_handler->init_shaders();
	
}

void glDrawingArea::Draw_ejes()
{
	
	glColor3f(0.0, 0.0, 1.0); 
	glBegin(GL_LINES);

		glVertex3f(0.0, .0, 0.0); 
		glVertex3f(1.0 * rejillaSize * 2, 0.0, 0.0);

	glEnd();
	
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINES);

		glVertex3f(0.0, 0.0, 0.0); 
		glVertex3f(0.0, 1.0 * rejillaSize * 2, 0.0);

	glEnd();
	
	
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);

		glVertex3f(0.0, 0.0, 0.0); 
		glVertex3f(0.0, 0.0, 1.0 * rejillaSize * 2);

	glEnd();
}

void glDrawingArea::DrawPoints ()
{
	for (int z = 0; z < 1; z++)
	{
		float ratio = float (z) / 255.0;
		for (int x = 0; x < 50; x++)
		{
			for (int y = 0; y < 50; y++)
			{
				glColor3f(ratio, ratio, ratio);
				glBegin(GL_POINTS);
					glVertex3f(x, z, y);
				glEnd();
			}
		}
	}
}

void glDrawingArea::drawRejilla() {
	
	int limRej[] = {-(rejillaSize * 20), rejillaSize * 20};
	
	glColor3f(0.3, 0.3, 0.3);
	for (int x = limRej[0]; x <= limRej[1]; x += rejillaSize) {
		if (x != limRej[0] && x != limRej[1]) {
			glBegin(GL_LINES);
				glVertex3f(x, 0, limRej[0]);
				glVertex3f(x, 0, limRej[1]);
			glEnd();
			glBegin(GL_LINES);
				glVertex3f(limRej[0], 0, x);
				glVertex3f(limRej[1], 0, x);
			glEnd();
		}
		
	}
}


void glDrawingArea::drawBoxSelector() {
	
	int indexes[] = {0, 1, 2, 2, 3, 0};
	
	// Dibuja una guía para saber la posicion del selector en la rejilla;
	
	glColor3f(.7, .4, .4);
	
	for (int x = 0; x < (int(sizeof(indexes) / sizeof(indexes[0])) / 3); x++) {
		int index1 = indexes[x * 3] * 3;
		int index2 = indexes[x * 3 + 1] * 3;
		int index3 = indexes[x * 3 + 2] * 3;
		glBegin(GL_TRIANGLES);
			glVertex3f(boxSelector.vertexPool[index1], 0.0, boxSelector.vertexPool[index1 + 2]);
			glVertex3f(boxSelector.vertexPool[index2], 0.0, boxSelector.vertexPool[index2 + 2]);
			glVertex3f(boxSelector.vertexPool[index3], 0.0, boxSelector.vertexPool[index3 + 2]);
		glEnd();
	}
	
	// Dibuja el selector;
	
	glColor3f(.9, .4, .3);
	
	for (int x = 0; x < int(boxSelector.indexPool.size()); x += 2) {
		int index1 = boxSelector.indexPool[x] * 3;
		int index2 = boxSelector.indexPool[x + 1] * 3;
		
		glBegin(GL_LINES);
			glVertex3f(boxSelector.vertexPool[index1], boxSelector.vertexPool[index1 + 1], boxSelector.vertexPool[index1 + 2]);
			glVertex3f(boxSelector.vertexPool[index2], boxSelector.vertexPool[index2 + 1], boxSelector.vertexPool[index2 + 2]);
		glEnd();
	}
}




//==================================
//=== METODOS DE M_DRAWING_AREA ====
//==================================


mDrawingArea::mDrawingArea()
{
	
}

mDrawingArea::mDrawingArea(BaseObjectType* obj, Glib::RefPtr<Gtk::Builder> const& builder)
	: Gtk::DrawingArea(obj)
	, builder{builder}
{
	add_events(Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
	signal_draw().connect(sigc::mem_fun(*this, &mDrawingArea::on_my_draw));
		
}

mDrawingArea::~mDrawingArea()
{
}

bool mDrawingArea::on_my_draw(const Cairo::RefPtr<Cairo::Context> &cr)
{
	cr->set_source_rgba(.2, .2, .2, 1.0); cr->fill(); cr->paint();
	int margin = 2;
	
	if (ref_tile_set != NULL) {
		
		if (draw_type == TILE_SET_TYPE) {
		
			int cr_coord_x_init = (get_width() - ref_tile_set->dimensions.x) / 2;
			int cr_coord_y_init = (get_height() - (ref_tile_set->dimensions.z + ref_tile_set->dimensions.y)) / 2;
			
			for (int e = 0; e < int(ref_tile_set->sliced_objects.size()); e++) {
				cr->save();
				cr->translate(cr_coord_x_init + e * (ref_tile_set->elements_dimensions.x + margin), cr_coord_y_init);

				
				if (e == ref_tile_set->selected_element) {
					auto maska = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, true, 8, ref_tile_set->elements_dimensions.x, ref_tile_set->dimensions.z + ref_tile_set->dimensions.y);
					maska->fill(0xef6700aa);
					Gdk::Cairo::set_source_pixbuf(cr, maska, 0, 0);
					cr->paint();
				}

				Gdk::Cairo::set_source_pixbuf(cr, ref_tile_set->get_element_image(e), 0, 0);
				cr->paint();
				
				cr->restore();
			}
			
		}
		
		else if (draw_type == TILE_TYPE) {
			
			int cr_coord_x_init = (get_width() - ref_tile_set->elements_dimensions.x) / 2;
			int cr_coord_y_init = (get_height() - (ref_tile_set->dimensions.z + ref_tile_set->dimensions.y)) / 2;

			cr->save();
			cr->translate(cr_coord_x_init, cr_coord_y_init);
			Gdk::Cairo::set_source_pixbuf(cr, ref_tile_set->get_element_image(ref_tile_set->selected_element), 0, 0);
			cr->paint();
			cr->restore();

		}
	}
	
	
	return true;
}

bool mDrawingArea::on_event(GdkEvent *evento)
{
	if (evento->any.type == GDK_BUTTON_PRESS) {
		int x, y;
		get_pointer(x, y);
		
		
		if (ref_tile_set != NULL) {
			if (draw_type == TILE_SET_TYPE) {
				int x_init = (get_width() - ref_tile_set->dimensions.x) / 2;
				int y_init = (get_height() - (ref_tile_set->dimensions.z + ref_tile_set->dimensions.y)) / 2;
				
				int x_selected =  x - x_init;
				
				if (y > y_init && y < (y_init + ref_tile_set->dimensions.z + ref_tile_set->dimensions.y)) {
					if (x_selected > 0) {
						ref_tile_set->selected_element = x_selected / ref_tile_set->elements_dimensions.x;
					}
				}
			}
			child_area->queue_draw();
			queue_draw();
		}
	}
	return true;
}

void mDrawingArea::set_tileset_reference(tileset *tls_reference)
{
	ref_tile_set = tls_reference;
}

void mDrawingArea::unset_tileset_reference()
{
	ref_tile_set = NULL;
}