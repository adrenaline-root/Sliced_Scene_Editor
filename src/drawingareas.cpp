#include "../include/drawingareas.h"


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
	//glClearColor(0., 0., 0., 1.0);
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
		glOrtho( -orto / 2, orto / 2, -ortov / 2 , ortov / 2, -ortov * 2, ortov * 2);
		//glOrtho(0.0, orto, 0.0, ortov, -ortov, ortov);
		
		glRotatef(45.0, 1.0, 0.0, 0.0);
		glRotatef(rotation, 0.0, 1.0, 0.0);
		//float x_aspect_correction = 1.0 / std::cos(45 * PI / 180);
		glScalef(scale, scale, scale);
		//glScalef(1.0, z_aspect_correction, 1.0);
		
				
		glTranslatef(tranlation_x, 0.0, tranlation_y);
		glActiveTexture(GL_TEXTURE0);
		

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glLineWidth(1.0);
		drawRejilla();
		//DrawPoints();
		//glLineWidth(2.0);
		Draw_ejes();
		
		glColor4f(1.0, 1.0, 1.0, 1.0);
		
		auto start = std::chrono::system_clock::now();
		if (int(scene_handler->sliced_objects.size()) > 0) {
			
			if (scene_handler->ligths_on == false) {
				scene_handler->ligths_handler.render_heigth_maps(scene_handler->standard_mesh, scene_handler->texture_handler.generated_textures[0]);
				scene_handler->ligths_handler.render_tile_map();
				scene_handler->ligths_on = true;
			}
			else {
				if (scene_handler->ligths_handler.ligths.size() > 0) {
					//Ligth *ligth = &scene_handler->ligths_handler.ligths[0];
					//ligth->position = {mouse_pos.x, 50.0, mouse_pos.y};
					
					//glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(-rotation), glm::vec3(0.0, 1.0, 0.0));
					//glm::vec4 light_dir = glm::vec4(1., 0.0, 0., 0.0);// * rot;

					//ligth->direction = {light_dir.x, light_dir.y, light_dir.z};
					//ligth->render_oclusion_map(scene_handler->ligths_handler.tile_map, scene_handler->ligths_handler.heigth_map1, scene_handler->ligths_handler.heigth_map2, {gwa.width, gwa.height});
					
					//if (ligth->oclusion_map) scene_handler->render_compute_output(gwa.width, gwa.height);
					scene_handler->render_scene_lt(gwa.width, gwa.height, tranlation_x, tranlation_y, scale, rotation);
					scene_handler->compute_shadows(gwa.width, gwa.height);
//					
//					ligth = NULL;
//					delete(ligth);
					
				}
				else scene_handler->render_scene(gwa.width, gwa.height, tranlation_x, tranlation_y, scale, rotation);
				
			
			}

			//scene_handler->render_filtered_scene(gwa.width, gwa.height, tranlation_x, tranlation_y, scale, rotation);
			//scene_handler->ray_cast(gwa.width, gwa.height, tranlation_x, tranlation_y, scale, rotation, mouse_pos);
			//scene_handler->render_shadow_map(gwa.width, gwa.height, mouse_pos);
			//scene_handler->render_grey_scale(gwa.width, gwa.height, tranlation_x, tranlation_y, scale, rotation, boxSelector.position);
			//scene_handler->render_colored_and_grey_sceen(gwa.width, gwa.height, tranlation_x, tranlation_y, scale, rotation, mouse_pos);
		}
		
		glUseProgram(0);
		
		
		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start;
		//std::cout << elapsed_seconds.count() << std::endl;
		
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		//glOrtho( -orto / 2, orto / 2, -ortov / 2 / z_aspect_correction, ortov / 2 / z_aspect_correction, -ortov / 2, ortov / 2);
		glOrtho( -orto / 2, orto / 2, -ortov / 2 , ortov / 2, -ortov * 2, ortov * 2);
		
		glRotatef(45.0, 1.0, 0.0, 0.0);
		glRotatef(rotation, 0.0, 1.0, 0.0);
	
		glScalef(scale, scale, scale);
		glScalef(1.0, z_aspect_correction, 1.0);
		
		glTranslatef(tranlation_x, 0.0, tranlation_y);
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glLineWidth(1.0);
		
		//glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		drawBoxSelector();
		if (scene_handler->ligths_handler.ligths.size() > 0) {
			drawLigth(scene_handler->ligths_handler.get_selected_ligth());
		}
		
		//scene_handler->render_shadow_map(gwa.width, gwa.height, mouse_pos);
		glFlush();
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
	if (evento->any.type == GDK_MOTION_NOTIFY) {
		mouse_pos = {evento->motion.x, evento->motion.y};
	}
	
	if (evento->any.type == GDK_BUTTON_PRESS) {
		grab_focus();
	}
	
	if (is_focus()) {
		if (evento->any.type == GDK_KEY_PRESS) {
			on_key_press_event(&evento->key);
		}
	}
	
	
	queue_draw();
	

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
				if (scale > 0.25) scale -= scaleInc;
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
				//scene_handler->shader_handler.load_mesh_to_gl_buffers(scene_handler->layered_mesh);
				
			}
			
			inst = NULL;
			delete(inst);
				
			
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
				//scene_handler->shader_handler.load_mesh_to_gl_buffers(scene_handler->layered_mesh);
			}
			
			else scene_handler->action = NO_ACTION;
			
			inst = NULL;
			delete(inst);
		}
	}
	
	
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
	scene_handler->ligths_handler.prepare_shaders();
	
	
	
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

void glDrawingArea::drawRejilla()
{
	
	int limRejx[] = {boxSelector.position.x -(rejillaSize * 100), boxSelector.position.x + rejillaSize * 100};
	int limRejy[] = {boxSelector.position.y -(rejillaSize * 100), boxSelector.position.y + rejillaSize * 100};
	
	glColor3f(0.3, 0.3, 0.3);
	int y = limRejy[0];
	for (int x = limRejx[0]; x <= limRejx[1]; x += rejillaSize) {
		if (x != limRejx[0] && x != limRejx[1]) {
			glBegin(GL_LINES);
				glVertex3f(x, 0, limRejy[0]);
				glVertex3f(x, 0, limRejy[1]);
			glEnd();
			glBegin(GL_LINES);
				glVertex3f(limRejx[0], 0, y);
				glVertex3f(limRejx[1], 0, y);
			glEnd();
		}
		y += rejillaSize;
		
	}
}

void glDrawingArea::drawBoxSelector()
{
	
	int indexes[] = {0, 1, 2, 2, 3, 0};
	
	// Dibuja una guía para saber la posicion del selector en la rejilla;
	
	glColor4f(.7, .4, .4, 1.);
	
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
	
	glColor4f(.9, .4, .3, 1.);
	
	for (int x = 0; x < int(boxSelector.indexPool.size()); x += 2) {
		int index1 = boxSelector.indexPool[x] * 3;
		int index2 = boxSelector.indexPool[x + 1] * 3;
		
		glBegin(GL_LINES);
			glVertex3f(boxSelector.vertexPool[index1], boxSelector.vertexPool[index1 + 1], boxSelector.vertexPool[index1 + 2]);
			glVertex3f(boxSelector.vertexPool[index2], boxSelector.vertexPool[index2 + 1], boxSelector.vertexPool[index2 + 2]);
		glEnd();
	}
	
	glColor4f(1., 1., 1., 1.);
}

void glDrawingArea::drawLigth(Ligth *ligth)
{
	glm::vec3 pos = glm::vec3(ligth->position.x, ligth->position.y, ligth->position.z);
	glm::vec3 normalized_dir = glm::normalize(glm::vec3(ligth->direction.x, ligth->direction.y, ligth->direction.z));
	glm::vec3 end = glm::vec3(normalized_dir.x, normalized_dir.y, normalized_dir.z);
	
	end = glm::vec3(pos.x + end.x * 64., pos.y + end.y * 64., pos.z + end.z * 64.);
	glColor3f(1., 1., 1.);
	glBegin(GL_LINES);
		glVertex3f(pos.x, pos.y, pos.z);
		glVertex3f(end.x, end.y, end.z);
	glEnd();
	
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