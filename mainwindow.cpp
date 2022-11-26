#include "mainwindow.h"
#include <iostream>

#define PI 3.14159265358979323846

// =============================
// 		MAIN WINDOW METHODS
// =============================

MainWindow::MainWindow()
{
} 

MainWindow::MainWindow(BaseObjectType* obj, Glib::RefPtr<Gtk::Builder> const& builder)
	: Gtk::Window(obj)
	, builder{builder}
{
	setMenu();
	configureButtons();
	//add_events(Gdk::KEY_PRESS_MASK | Gdk::KEY_RELEASE_MASK | Gdk::POINTER_MOTION_MASK);
	add_events(Gdk::KEY_PRESS_MASK | Gdk::KEY_RELEASE_MASK | Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK);
	set_can_focus(true);
	set_sensitive(true);
	
	
	
	set_default_size(800, 600);
	set_position(Gtk::WIN_POS_CENTER);
	set_border_width(2);
	
	std::cout << "starting derived Window" << std::endl;
	builder->get_widget_derived("glArea", glAreaN);
	builder->get_widget_derived("TilesArea", tiles_draw_area);
	builder->get_widget_derived("TileArea", selected_tile_draw_area);
	
	tiles_draw_area->draw_type = TILE_SET_TYPE;
	tiles_draw_area->child_area = selected_tile_draw_area;
	
	selected_tile_draw_area->draw_type = TILE_TYPE;
	
	glAreaN->scene_handler = &scene_handler;
}

MainWindow::~MainWindow()
{
	
}

void MainWindow::setMenu()
{
	mRefActionGroup = Gtk::ActionGroup::create();
	mRefUIManager = Gtk::UIManager::create();
	
	Gtk::AccelKey Add_key("a", "<Actions>//Add_Object");
	Gtk::AccelKey Delete_key("d", "<Actions>//Delete_Object");
	Gtk::AccelKey Rotate_key("r", "<Actions>//Rotate_Object");
	Gtk::AccelKey Selection_key("s", "<Actions>//Select_Tile");
	Gtk::AccelKey Move_key("m", "<Actions>//Move_Object");
	
	
	// ACCIONES DEL MENU PRINCIPAL
	
	mRefActionGroup->add(Gtk::Action::create("File_Menu", "Archivo"));
	mRefActionGroup->add(Gtk::Action::create("New_Scene", "New Scene"));
	mRefActionGroup->add(Gtk::Action::create("Load_Scene", "Load Scene"), sigc::mem_fun(*this, &MainWindow::loadScene));
	mRefActionGroup->add(Gtk::Action::create("Save_Scene", "Save Scene"), sigc::mem_fun(*this, &MainWindow::saveScene));
	
	
	//mRefActionGroup->add(Gtk::Action::create("SaveScene", "Save"), sigc::mem_fun(*this, &MainWindow::saveGLContext));
	mRefActionGroup->add(Gtk::Action::create("Edit_Menu", "Editar"));
	mRefActionGroup->add(Gtk::Action::create("Add_Object", "Añadir objeto"), Add_key,sigc::mem_fun(*this, &MainWindow::add_object_to_scene));
	mRefActionGroup->add(Gtk::Action::create("Delete_Object", "Eliminar objeto"), Delete_key,sigc::mem_fun(*this, &MainWindow::delete_object_from_scene));
	mRefActionGroup->add(Gtk::Action::create("Select_Tile", "Select tile"), Selection_key, sigc::mem_fun(*this, &MainWindow::select_tile));
	mRefActionGroup->add(Gtk::Action::create("Rotate_Object", "Rotar objeto"), Rotate_key, sigc::mem_fun(*this, &MainWindow::rotar_objeto));
	mRefActionGroup->add(Gtk::Action::create("Move_Object", "Mover objeto"), Move_key, sigc::mem_fun(*this, &MainWindow::mover_objeto));
	mRefActionGroup->add(Gtk::Action::create("Set_Rejilla", "Rejilla"), sigc::mem_fun(*this, &MainWindow::set_rejilla));


	mRefUIManager->insert_action_group(mRefActionGroup);
	add_accel_group(mRefUIManager->get_accel_group());

	// EXPLICITACION DE LA JERARQUIA DEL MENU

	Glib::ustring ui_info = 

		"<ui>"
		"	<menubar name = 'MenuBar'>"
		"		<menu action = 'File_Menu'>"
		"			<menuitem action = 'New_Scene'/>"
		"			<menuitem action = 'Load_Scene'/>"
		"			<menuitem action = 'Save_Scene'/>"
		"		</menu>"
		"		<menu action = 'Edit_Menu'>"
		"			<menuitem action = 'Add_Object'/>"
		"			<menuitem action = 'Delete_Object'/>"
		"			<menuitem action = 'Select_Tile'/>"
		"			<menuitem action = 'Rotate_Object'/>"
		"			<menuitem action = 'Move_Object'/>"
		"			<menuitem action = 'Set_Rejilla'/>"
		"		</menu>"
		"	</menubar>"
		"</ui>";

	
	// CONTROL DE ERRORES EN EL MENU

	#ifdef GLIBMM_EXCEPTIONS_ENABLED
	try
	{
		mRefUIManager->add_ui_from_string(ui_info);
	}
	catch(const Glib::Error& ex)
	{
		std::cerr << "building menus failed: " << ex.what();
	}
	#else
	std::auto_ptr<Glib::Error> ex;

	mRefUIManager->add_ui_from_string(ui_info, ex);
	if(ex.get())
	{
		std::cerr << "building menus failed: " << ex->what();
	}
	#endif //GLIBMM_EXCEPTIONS_ENABLED

	// ADDICION DEL MENU A LA CAJA PRINCIPAL DE LA VENTANA (YA QUE LAS VENTANAS SOLO PUEDEN TENER UN WIDGET ADHERIDO)

	Gtk::Box *mainBox = nullptr;
	builder->get_widget("MainBox", mainBox);
	
	Gtk::Widget *pMenu = mRefUIManager->get_widget("/MenuBar");
	mainBox->pack_start(*pMenu, Gtk::PACK_SHRINK);

}

void MainWindow::configureButtons()
{
	Gtk::Button *ana_btn = nullptr;
	Gtk::Button *brr_btn = nullptr;
	Gtk::Button *open_filechooser_btn = nullptr;
	Gtk::Entry *filechooser_entry = nullptr;
	Gtk::ListBox *list_box = nullptr;
	
	builder->get_widget("AnaButton", ana_btn);
	builder->get_widget("BorrButton", brr_btn);
	builder->get_widget("open_filechooser_btn", open_filechooser_btn);
	builder->get_widget("FileChooserEntry", filechooser_entry);
	builder->get_widget("ListBox", list_box);
	
	ana_btn->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::loadTileSet));
	brr_btn->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::deleteTileSet));
	
	open_filechooser_btn->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::showFileChooser), filechooser_entry));
	
	list_box->signal_selected_rows_changed().connect(sigc::mem_fun(*this, &MainWindow::selectTileSet));
}


void MainWindow::loadScene()
{
	Gtk::FileChooserDialog dialog(*this, "Select a scene", Gtk::FILE_CHOOSER_ACTION_OPEN);
	
	dialog.add_button("Accept", Gtk::RESPONSE_OK);
	dialog.add_button("Cancel", Gtk::RESPONSE_CLOSE);
	
	if (dialog.run() == Gtk::RESPONSE_OK) {
		std::cout << "loading scene" << std::endl;
		bool loaded = scene_handler.load_scene(dialog.get_filename());
		
		if (loaded) {

			Gtk::ListBox *lbox = nullptr;
			builder->get_widget("ListBox", lbox);
			
			for (auto tile : scene_handler.tileset_handler.tile_set_list) {
				Gtk::ListBoxRow *row = Gtk::manage(new Gtk::ListBoxRow);
				row->add_label(tile.name, true, Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
			
				lbox->append(*row);
				lbox->select_row(*row);
			
				scene_handler.tileset_handler.select_tile(lbox->get_selected_row()->get_index());
			
			}
			
			tileset *tile = &scene_handler.tileset_handler.tile_set_list[scene_handler.tileset_handler.tile_set_list.size() - 1];
			
			tiles_draw_area->set_tileset_reference(tile);
			selected_tile_draw_area->set_tileset_reference(tile);
			
			
			lbox->show_all();
			
			
			scene_handler.make_context_current(glAreaN->xdisplay, glAreaN->win, glAreaN->glc);
			scene_handler.shader_handler.load_mesh_to_gl_buffers(scene_handler.layered_mesh);
			
			glAreaN->queue_draw();
		}
		
	}
	
	
}

void MainWindow::saveScene()
{
	Gtk::Dialog *save_scene_dialog = nullptr;
	Gtk::Button *accept_btn = nullptr;
	Gtk::Button *cancel_btn = nullptr;
	Gtk::Entry *filechooser_entry = nullptr;
	Gtk::Entry *save_name_entry = nullptr;
	
	
	
	builder->get_widget("SaveSceneDialog", save_scene_dialog);
	
	builder->get_widget("accept_filechooser_btn", accept_btn);
	builder->get_widget("cancel_filechooser_btn", cancel_btn);
	
	builder->get_widget("FileChooserEntry", filechooser_entry);
	builder->get_widget("SaveNameEntry", save_name_entry);
	
	accept_btn->signal_clicked().connect(sigc::bind(sigc::mem_fun(*save_scene_dialog, &Gtk::Dialog::response), Gtk::RESPONSE_OK));
	cancel_btn->signal_clicked().connect(sigc::bind(sigc::mem_fun(*save_scene_dialog, &Gtk::Dialog::response), Gtk::RESPONSE_CLOSE));
	
	if (save_scene_dialog->run() == Gtk::RESPONSE_OK) {
		
		std::string filename = filechooser_entry->get_text() + "/" + save_name_entry->get_text();
		std::cout << "Saving scene: " << filename << std::endl;
		
		scene_handler.save_scene(filename);
		
		save_scene_dialog->close();
	}
	
	else {
		save_scene_dialog->close();
	}
	
	
}

void MainWindow::showFileChooser(Gtk::Entry *entry)
{
	Gtk::FileChooserDialog dialog(*this, "Select a folder", Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER);
	dialog.add_button("Abrir", Gtk::RESPONSE_OK);
	dialog.add_button("Cancelar", Gtk::RESPONSE_CANCEL);
	
	if (dialog.run() == Gtk::RESPONSE_OK) {
		entry->set_text(dialog.get_filename());
		dialog.close();
	}
	
	else {
		dialog.close();
	}
}

void MainWindow::loadTileSet() 
{
	Gtk::FileChooserDialog dialog(*this, "Select a file", Gtk::FILE_CHOOSER_ACTION_OPEN);
	dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	
	if (dialog.run() == Gtk::RESPONSE_OK) {
		
		bool is_not_in = scene_handler.add_tile(dialog.get_filename());
		
		if (is_not_in) {
			
			Gtk::ListBox *lbox = nullptr;
			builder->get_widget("ListBox", lbox);
			
			tileset *tile = &scene_handler.tileset_handler.tile_set_list[scene_handler.tileset_handler.tile_set_list.size() - 1];
			
			tiles_draw_area->set_tileset_reference(tile);
			selected_tile_draw_area->set_tileset_reference(tile);
			
			Gtk::ListBoxRow *row = Gtk::manage(new Gtk::ListBoxRow);
			row->add_label(tile->name, true, Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
			
			lbox->append(*row);
			lbox->select_row(*row);
			
			scene_handler.tileset_handler.select_tile(lbox->get_selected_row()->get_index());
			
			lbox->show_all();
		}
		
		dialog.close();
		
	}
	
	else dialog.close();

}

void MainWindow::deleteTileSet()
{
	Gtk::ListBox *lbox = nullptr;
	builder->get_widget("ListBox", lbox);
	
	if (int(lbox->get_children().size()) > 0) {
	
		for (int i = 0; i < int(lbox->get_children().size()); i++)
		{
			if (lbox->get_row_at_index(i)->is_selected())
			{
				Gtk::ListBoxRow *mRow = lbox->get_selected_row();
				
				
				tiles_draw_area->unset_tileset_reference();
				selected_tile_draw_area->unset_tileset_reference();
				
				tiles_draw_area->queue_draw();
				selected_tile_draw_area->queue_draw();
				
				scene_handler.tileset_handler.tile_set_list.erase(scene_handler.tileset_handler.tile_set_list.begin() + scene_handler.tileset_handler.selected_tile);
				
				lbox->remove(*mRow);
				
				if (i > 0)
				{
					Gtk::ListBoxRow *new_selected_row = lbox->get_row_at_index(i - 1);
					lbox->select_row(*new_selected_row);
				}
				if (i == 0 && int(lbox->get_children().size()) > 0)
				{
					Gtk::ListBoxRow *new_selected_row = lbox->get_row_at_index(i);
					lbox->select_row(*new_selected_row);
				}

				break;
			}
		}
	}
}

void MainWindow::selectTileSet()
{
	Gtk::ListBox *list_box = nullptr;
	builder->get_widget("ListBox", list_box);
	
	scene_handler.tileset_handler.select_tile(list_box->get_selected_row()->get_index()); 
	
	tiles_draw_area->ref_tile_set = scene_handler.tileset_handler.get_selected_tile_pntr();
	selected_tile_draw_area->ref_tile_set = scene_handler.tileset_handler.get_selected_tile_pntr();
	
	tiles_draw_area->queue_draw();
	selected_tile_draw_area->queue_draw();
	
}

void MainWindow::add_object_to_scene()
{
	if (scene_handler.tileset_handler.tile_set_list.size() > 0) {
//		std::cout << "El tileset seleccionado es: " << scene_handler.tileset_handler.tile_set_list[scene_handler.tileset_handler.selected_tile].name << std::endl;
		
		scene_handler.add_sliced_object_to_scene(glAreaN->boxSelector.position);
		
//		std::cout << "=======================================" << std::endl;
//		std::cout << "Los objectos añadidos a la escena son: " << std::endl;
//		for (auto sliced_obj : scene_handler.sliced_objects) {
//			std::cout << "Sliced object: " << sliced_obj.name << std::endl;
//			for (auto instancia : sliced_obj.instancias) {
//				std::printf("-------------: (%d, %d, %d)\n", instancia.position.x, instancia.position.y, instancia.position.z);
//			}
//		}
//		
		//std::cout << "Nombre de las texturas añadidas a la escena: " << std::endl;
//		for (std::string name : scene_handler.texture_handler.textures_names) {
//			std::cout << "--- " << name << std::endl;
//		}
		
		//scene_handler.make_context_current(glAreaN->xdisplay, glAreaN->win, glAreaN->glc);
		//scene_handler.gen_textures();
		
		std::cout << "texture generated" << std::endl;
		scene_handler.make_context_current(glAreaN->xdisplay, glAreaN->win, glAreaN->glc);
		scene_handler.shader_handler.load_mesh_to_gl_buffers(scene_handler.layered_mesh);
		
		glAreaN->queue_draw();
		
	}
}

void MainWindow::delete_object_from_scene()
{
	if (scene_handler.sliced_objects.size() > 0) { 
	
		scene_handler.delete_sliced_object_from_scene(glAreaN->boxSelector.position);
		scene_handler.shader_handler.load_mesh_to_gl_buffers(scene_handler.layered_mesh);
		glAreaN->queue_draw();

	}
}

void MainWindow::select_tile()
{
	
	if (tiles_draw_area->ref_tile_set != NULL) {
		
		tiles_draw_area->ref_tile_set->selected_element += 1;
		tiles_draw_area->ref_tile_set->selected_element %= tiles_draw_area->ref_tile_set->number_of_elements;
		tiles_draw_area->queue_draw();
		selected_tile_draw_area->queue_draw();
	
	}
}

void MainWindow::rotar_objeto()
{
	scene_handler.action = ROTATE;
}

void MainWindow::mover_objeto() 
{
	scene_handler.action = MOVE;
}

void MainWindow::set_rejilla()
{
	Gtk::Dialog *rej_dialog = nullptr;
	Gtk::Button *accept_btn = nullptr;
	Gtk::Button *cancel_btn = nullptr;
	Gtk::SpinButton *rej_spin = nullptr;
	
	builder->get_widget("RejillaDialog", rej_dialog);
	builder->get_widget("rejilla_accept_btn", accept_btn);
	builder->get_widget("rejilla_cancel_btn", cancel_btn);
	builder->get_widget("rejilla_sp_btn", rej_spin);
	
	
	
	accept_btn->signal_clicked().connect(sigc::bind(sigc::mem_fun(*rej_dialog, &Gtk::Dialog::response), Gtk::RESPONSE_OK));
	cancel_btn->signal_clicked().connect(sigc::bind(sigc::mem_fun(*rej_dialog, &Gtk::Dialog::response), Gtk::RESPONSE_CLOSE));
	
	
	if (rej_dialog->run() == Gtk::RESPONSE_OK) {
		glAreaN->rejillaSize = int(rej_spin->get_value());
		rej_dialog->close();
	}
	
	else rej_dialog->close();
	
	glAreaN->boxSelector.createCube(float(glAreaN->rejillaSize));
	glAreaN->boxSelector.position = {0, 0, 0};
	glAreaN->queue_draw();
	
	
}



