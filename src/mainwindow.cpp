#include "../include/mainwindow.h"
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
	add_events(Gdk::KEY_PRESS_MASK | Gdk::KEY_RELEASE_MASK | Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK);
	
	setMenu();
	configureButtons();
	configureLigthsOptions();
	//add_events(Gdk::KEY_PRESS_MASK | Gdk::KEY_RELEASE_MASK | Gdk::POINTER_MOTION_MASK);
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
	
	// ACCIONES DE ARCHIVO
	mRefActionGroup->add(Gtk::Action::create("File_Menu", "Archivo"));
	mRefActionGroup->add(Gtk::Action::create("New_Scene", "New Scene"));
	mRefActionGroup->add(Gtk::Action::create("Load_Scene", "Load Scene"), sigc::mem_fun(*this, &MainWindow::loadScene));
	mRefActionGroup->add(Gtk::Action::create("Save_Scene", "Save Scene"), sigc::mem_fun(*this, &MainWindow::saveScene));
	
	// ACCIONES DE VENTANA
	mRefActionGroup->add(Gtk::Action::create("View_Menu", "Mostrar"));
	mRefActionGroup->add(Gtk::Action::create("Show_hide_tiles", "Mostrar/Ocultar Tiles Area"), sigc::bind(sigc::mem_fun(*this, &MainWindow::show_hide_toolspane), TILES));
	mRefActionGroup->add(Gtk::Action::create("Show_hide_ligths", "Mostrar/Ocultar Ligths Area"), sigc::bind(sigc::mem_fun(*this, &MainWindow::show_hide_toolspane), LIGTHS));
	
	
	//mRefActionGroup->add(Gtk::Action::create("SaveScene", "Save"), sigc::mem_fun(*this, &MainWindow::saveGLContext));
	mRefActionGroup->add(Gtk::Action::create("Edit_Menu", "Editar"));
	mRefActionGroup->add(Gtk::Action::create("Add_Object", "Añadir objeto"), Add_key, sigc::mem_fun(*this, &MainWindow::add_object_to_scene));
	mRefActionGroup->add(Gtk::Action::create("Delete_Object", "Eliminar objeto"), Delete_key, sigc::mem_fun(*this, &MainWindow::delete_object_from_scene));
	mRefActionGroup->add(Gtk::Action::create("Select_Tile", "Select tile"), Selection_key, sigc::mem_fun(*this, &MainWindow::select_tile));
	mRefActionGroup->add(Gtk::Action::create("Rotate_Object", "Rotar objeto"), Rotate_key, sigc::mem_fun(*this, &MainWindow::rotar_objeto));
	mRefActionGroup->add(Gtk::Action::create("Move_Object", "Mover objeto"), Move_key, sigc::mem_fun(*this, &MainWindow::mover_objeto));
	mRefActionGroup->add(Gtk::Action::create("Set_Rejilla", "Rejilla"), sigc::mem_fun(*this, &MainWindow::set_rejilla));
	mRefActionGroup->add(Gtk::Action::create("Add_ligth", "Añadir luz"), sigc::mem_fun(*this, &MainWindow::add_ligth));


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
		
		"		<menu action = 'View_Menu'>"
		"			<menuitem action = 'Show_hide_tiles'/>"
		"			<menuitem action = 'Show_hide_ligths'/>"
		"		</menu>"
		
		"		<menu action = 'Edit_Menu'>"
		"			<menuitem action = 'Add_Object'/>"
		"			<menuitem action = 'Delete_Object'/>"
		"			<menuitem action = 'Select_Tile'/>"
		"			<menuitem action = 'Rotate_Object'/>"
		"			<menuitem action = 'Move_Object'/>"
		"			<menuitem action = 'Set_Rejilla'/>"
		"			<menuitem action = 'Add_ligth'/>"
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
	
	// CONFIGURACION TILESET BUTTONS
	
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
	
	// CONFIGURACION LIGTHS BUTTONS
	
	// TYPE BUTTONS
	Gtk::RadioButton *dir_btn = nullptr;
	Gtk::RadioButton *point_btn = nullptr;
	Gtk::RadioButton *foc_btn = nullptr;
	
	builder->get_widget("LigthDirRadioButton", dir_btn);
	builder->get_widget("LigthPointRadioButton", point_btn);
	builder->get_widget("LigthFocusRadioButton", foc_btn);
	
	Gtk::RadioButton **rbuttons = new Gtk::RadioButton*[3] {dir_btn, point_btn, foc_btn};
	
	for (int x = 0; x < 3; x++) {
		rbuttons[x]->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::alter_ligth), LIGTHS_OPTIONS::TYPE, 0));
	}
	
	// POSITION BUTTONS
	Gtk::SpinButton *x_btn = nullptr;
	Gtk::SpinButton *y_btn = nullptr;
	Gtk::SpinButton *z_btn = nullptr;
	
	builder->get_widget("LigthXSpin", x_btn);
	builder->get_widget("LigthYSpin", y_btn);
	builder->get_widget("LigthZSpin", z_btn);
	
	Gtk::SpinButton **spbuttons = new Gtk::SpinButton*[3] {x_btn, y_btn, z_btn};
	
	for (int x = 0; x < 3; x++) {
		spbuttons[x]->signal_changed().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::alter_ligth), LIGTHS_OPTIONS::POSITIONS, x));
	}
	
	
	// DIRECTION BUTTONS
	Gtk::SpinButton *x_dir_btn = nullptr;
	Gtk::SpinButton *y_dir_btn = nullptr;
	Gtk::SpinButton *z_dir_btn = nullptr;
	
	builder->get_widget("LigthDirXSpin", x_dir_btn);
	builder->get_widget("LigthDirYSpin", y_dir_btn);
	builder->get_widget("LigthDirZSpin", z_dir_btn);
	
	spbuttons = new Gtk::SpinButton*[3] {x_dir_btn, y_dir_btn, z_dir_btn};
	
	for (int x = 0; x < 3; x++) {
		spbuttons[x]->signal_changed().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::alter_ligth), LIGTHS_OPTIONS::DIRECTION, x));
	}
	
//	spbuttons = nullptr;
//	rbuttons = nullptr;
//	delete(rbuttons);
//	delete(spbuttons);
	
	// AMBIENT-SPECULAR BUTTONS
	 
	Gtk::ColorButton *amb_btn = nullptr;
	Gtk::ColorButton *spec_btn = nullptr;
	
	builder->get_widget("LigthsAmbientColorButton", amb_btn);
	builder->get_widget("LigthsSpecularColorButton", spec_btn);
	
	amb_btn->signal_color_set().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::alter_ligth), LIGTHS_OPTIONS::AMBIENT, 0));
	spec_btn->signal_color_set().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::alter_ligth), LIGTHS_OPTIONS::SPECULAR, 0));
	
	// FRUSTRUM, STRENGTH, ATTENUATION BUTTONS
	
	Gtk::SpinButton *frustrum_btn = nullptr;
	Gtk::SpinButton *strength_btn = nullptr;
	Gtk::SpinButton *attenuation_btn = nullptr;
	
	builder->get_widget("LigthsFrustrumSpin", frustrum_btn);
	builder->get_widget("LigthsStrengthSpin", strength_btn);
	builder->get_widget("LigthsAttenuationSpin", attenuation_btn);
	
	spbuttons = new Gtk::SpinButton*[3] {frustrum_btn, strength_btn, attenuation_btn};
	
	LIGTHS_OPTIONS opt[] = {FRUSTRUM, STRENGTH, ATTENUATION};
	
	for (int x = 0; x < 3; x++) {
		spbuttons[x]->signal_changed().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::alter_ligth), opt[x], x));
	}

	Gtk::ListBox *llist_box = nullptr;
	builder->get_widget("LigthsListBox", llist_box);
	llist_box->signal_selected_rows_changed().connect(sigc::mem_fun(*this, &MainWindow::select_ligth));
	

}


void MainWindow::configureLigthsOptions()
{
	Gtk::Button *ligth_type = nullptr;
	Gtk::Button *ligth_position = nullptr;
	Gtk::Button *ligth_direction = nullptr;
	Gtk::Button *ligth_ambient = nullptr;
	Gtk::Button *ligth_specular = nullptr;
	Gtk::Button *ligth_frustrum = nullptr;
	Gtk::Button *ligth_strength = nullptr;
	Gtk::Button *ligth_attenuation = nullptr;
	
	Gtk::ButtonBox *type_box = nullptr;
	Gtk::ButtonBox *positions_box = nullptr;
	Gtk::ButtonBox *directions_box = nullptr;
	Gtk::ColorButton *ambient_color_button = nullptr;
	Gtk::ColorButton *specular_color_button = nullptr;
	Gtk::ButtonBox *frustrum_spin_button = nullptr;
	Gtk::ButtonBox *strength_spin_button = nullptr;
	Gtk::ButtonBox *attenuation_spin_button = nullptr;
	
	
	builder->get_widget("LigthsTypeButton", ligth_type);
	builder->get_widget("LigthsTypeButtonBox", type_box);
	
	builder->get_widget("LigthsPositionsButton", ligth_position);
	builder->get_widget("LigthsPositionsButtonBox", positions_box);
	
	builder->get_widget("LigthsDirectionsButton", ligth_direction);
	builder->get_widget("LigthsDirectionsButtonBox", directions_box);
	
	builder->get_widget("LigthsAmbientButton", ligth_ambient);
	builder->get_widget("LigthsAmbientColorButton", ambient_color_button);
	
	builder->get_widget("LigthsSpecularButton", ligth_specular);
	builder->get_widget("LigthsSpecularColorButton", specular_color_button);
	
	builder->get_widget("LigthsFrustrumButton", ligth_frustrum);
	builder->get_widget("LigthsStrengthButton", ligth_strength);
	builder->get_widget("LigthsAttenuationButton", ligth_attenuation);
	
	builder->get_widget("LigthsFrustrumButtonBox", frustrum_spin_button);
	builder->get_widget("LigthsStrengthButtonBox", strength_spin_button);
	builder->get_widget("LigthsAttenuationButtonBox", attenuation_spin_button);
	
	ligth_position->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::hide_ligths_characteristics), positions_box, ligth_position));
	ligth_direction->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::hide_ligths_characteristics), directions_box, ligth_direction));
	ligth_type->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::hide_ligths_characteristics), type_box, ligth_type));
	ligth_ambient->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::hide_ligths_characteristics), ambient_color_button, ligth_ambient));
	ligth_specular->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::hide_ligths_characteristics), specular_color_button, ligth_specular));
	
	ligth_frustrum->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::hide_ligths_characteristics), frustrum_spin_button, ligth_frustrum));
	ligth_strength->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::hide_ligths_characteristics), strength_spin_button, ligth_strength));
	ligth_attenuation->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::hide_ligths_characteristics), attenuation_spin_button, ligth_attenuation));
	
}

void MainWindow::hide_ligths_characteristics(Gtk::Container *bbox, Gtk::Button *button)
{
	if (bbox->get_visible()) {
		bbox->hide();
		button->set_image_from_icon_name("gtk-go-down", Gtk::ICON_SIZE_BUTTON);
	}
	else {
		bbox->show();
		button->set_image_from_icon_name("gtk-go-up", Gtk::ICON_SIZE_BUTTON);
	}
	
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
			
			
			//scene_handler.make_context_current(glAreaN->xdisplay, glAreaN->win, glAreaN->glc);
			//scene_handler.shader_handler.load_mesh_to_gl_buffers(scene_handler.layered_mesh);
			
			glAreaN->queue_draw();
			
			tile = NULL;
			lbox = nullptr;
			delete(tile);
			delete(lbox);
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
		
		scene_handler.add_sliced_object_to_scene(glAreaN->boxSelector.position);
		std::cout << "texture generated" << std::endl;
		glAreaN->queue_draw();
		
	}
}

void MainWindow::delete_object_from_scene()
{
	if (scene_handler.sliced_objects.size() > 0) { 
	
		scene_handler.delete_sliced_object_from_scene(glAreaN->boxSelector.position);
		scene_handler.shader_handler.get_shader("main_shader").load_mesh_to_buffers(scene_handler.standard_mesh);
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
	//glAreaN->boxSelector.position = {0, 0, 0};
	glAreaN->queue_draw();
	
	
}

void MainWindow::show_hide_toolspane(TOOLBOX tooltype)
{
	if (tooltype == TILES) {
		
		Gtk::Paned *tiles_panned = nullptr;
		builder->get_widget("LateralPanedParent", tiles_panned);
		
		if (tiles_panned->get_visible()) tiles_panned->hide();
		else tiles_panned->show();
	}
	
	else if (tooltype == LIGTHS && scene_handler.ligths_handler.ligths.size() > 0) {
		
		Gtk::Paned *ligths_panned = nullptr;
		builder->get_widget("LigthsPanned", ligths_panned);
		
		if (ligths_panned->get_visible()) ligths_panned->hide();
		else ligths_panned->show();
	}
	
}

void MainWindow::add_ligth()
{
	std::cout << "adding ligth ------- " << std::endl;
	Gtk::ListBox *ligth_lbox = nullptr;
	builder->get_widget("LigthsListBox", ligth_lbox);
	
	Gtk::Paned *l_panned = nullptr;
	builder->get_widget("LigthsPanned", l_panned);
	
	l_panned->show();
	
	Ligth new_ligth;
	scene_handler.ligths_handler.add_ligth(new_ligth);
	
	Gtk::ListBoxRow *row = Gtk::manage(new Gtk::ListBoxRow);
	ligth_lbox->append(*row);
	ligth_lbox->select_row(*row);
	
	int selected_index = ligth_lbox->get_selected_row()->get_index();
	std::string name = "Ligth" + std::to_string(selected_index);
	row->add_label(name, true, Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
	//select_ligth();
	
	ligth_lbox->show_all();
	
}

void MainWindow::select_ligth()
{
	Gtk::ListBox *list_box = nullptr;
	builder->get_widget("LigthsListBox", list_box);
	
	scene_handler.ligths_handler.selected_ligth = list_box->get_selected_row()->get_index();
	
	Ligth *l = scene_handler.ligths_handler.get_selected_ligth();
	
	Gtk::RadioButton *dir_btn = nullptr;
	Gtk::RadioButton *point_btn = nullptr;
	Gtk::RadioButton *foc_btn = nullptr;
	
	builder->get_widget("LigthDirRadioButton", dir_btn);
	builder->get_widget("LigthPointRadioButton", point_btn);
	builder->get_widget("LigthFocusRadioButton", foc_btn);
	
	if (l->ligth_type == LigthType::DIRECTIONAL) dir_btn->set_active();
	else {
		if (l->ligth_type == LigthType::POINT) point_btn->set_active();
		else foc_btn->set_active();
	}
	
	Gtk::SpinButton *x_btn = nullptr;
	Gtk::SpinButton *y_btn = nullptr;
	Gtk::SpinButton *z_btn = nullptr;
	
	builder->get_widget("LigthXSpin", x_btn);
	builder->get_widget("LigthYSpin", y_btn);
	builder->get_widget("LigthZSpin", z_btn);
	
	x_btn->set_value(l->position.x);
	y_btn->set_value(l->position.y);
	z_btn->set_value(l->position.z);
	
	Gtk::SpinButton *x_dir_btn = nullptr;
	Gtk::SpinButton *y_dir_btn = nullptr;
	Gtk::SpinButton *z_dir_btn = nullptr;
	
	builder->get_widget("LigthDirXSpin", x_dir_btn);
	builder->get_widget("LigthDirYSpin", y_dir_btn);
	builder->get_widget("LigthDirZSpin", z_dir_btn);
	
	x_dir_btn->set_value(l->direction.x);
	y_dir_btn->set_value(l->direction.y);
	z_dir_btn->set_value(l->direction.z);
	
	Gtk::ColorButton *amb_btn = nullptr;
	Gtk::ColorButton *spec_btn = nullptr;
	
	builder->get_widget("LigthsAmbientColorButton", amb_btn);
	builder->get_widget("LigthsSpecularColorButton", spec_btn);
	
	
	Gdk::RGBA ambient, specular;
	ambient.set_rgba(l->ambient.x, l->ambient.y, l->ambient.z, 1.);
	specular.set_rgba(l->specular.x, l->specular.y, l->specular.z, 1.);
	
	amb_btn->set_rgba(ambient);
	spec_btn->set_rgba(specular);
	
	Gtk::SpinButton *frustrum = nullptr;
	Gtk::SpinButton *strength = nullptr;
	Gtk::SpinButton *attenuation = nullptr;
	
	builder->get_widget("LigthsFrustrumSpin", frustrum);
	builder->get_widget("LigthsStrengthSpin", strength);
	builder->get_widget("LigthsAttenuationSpin", attenuation);
	
	frustrum->set_value(l->frustrum);
	strength->set_value(l->strength);
	attenuation->set_value(l->attenuation);
	
}

void MainWindow::alter_ligth(LIGTHS_OPTIONS l_opt, int attr)
{
	if (scene_handler.ligths_handler.ligths.size() > 0) 
	{
		Ligth* ligth = scene_handler.ligths_handler.get_selected_ligth();
		
		if (l_opt == LIGTHS_OPTIONS::TYPE)
		{
			Gtk::RadioButton *dir_rbutton = nullptr;
			Gtk::RadioButton *point_rbutton = nullptr;
			Gtk::RadioButton *foc_rbutton = nullptr;
			
			builder->get_widget("LigthDirRadioButton", dir_rbutton);
			builder->get_widget("LigthPointRadioButton", point_rbutton);
			builder->get_widget("LigthFocusRadioButton", foc_rbutton);
			
			if (dir_rbutton->get_active()) ligth->ligth_type = LigthType::DIRECTIONAL;
			else {
				if (point_rbutton->get_active()) ligth->ligth_type = LigthType::POINT;
				else ligth->ligth_type = LigthType::FOCUS;
			}
		}
		
		else if (l_opt == LIGTHS_OPTIONS::POSITIONS) {
			
			Gtk::SpinButton *x_spbutton = nullptr;
			Gtk::SpinButton *y_spbutton = nullptr;
			Gtk::SpinButton *z_spbutton = nullptr;
			
			builder->get_widget("LigthXSpin", x_spbutton);
			builder->get_widget("LigthYSpin", y_spbutton);
			builder->get_widget("LigthZSpin", z_spbutton);
			
			if (attr == 0) ligth->position.x = float(x_spbutton->get_value());
			else if (attr == 1) ligth->position.y = float(y_spbutton->get_value());
			else if (attr == 2) ligth->position.z = float(z_spbutton->get_value());
			
			//ligth->position = {float(x_spbutton->get_value()), float(y_spbutton->get_value()), float(z_spbutton->get_value())};
			
		}
		
		
		else if (l_opt == LIGTHS_OPTIONS::DIRECTION) {
			
			Gtk::SpinButton *x_spbutton = nullptr;
			Gtk::SpinButton *y_spbutton = nullptr;
			Gtk::SpinButton *z_spbutton = nullptr;
			
			builder->get_widget("LigthDirXSpin", x_spbutton);
			builder->get_widget("LigthDirYSpin", y_spbutton);
			builder->get_widget("LigthDirZSpin", z_spbutton);
			
			if (attr == 0) ligth->direction.x = float(x_spbutton->get_value());
			else if (attr == 1) ligth->direction.y = float(y_spbutton->get_value());
			else if (attr == 2) ligth->direction.z = float(z_spbutton->get_value());
				
		}
		
		else if (l_opt == LIGTHS_OPTIONS::AMBIENT)
		{
			Gtk::ColorButton *color_btn = nullptr;
			builder->get_widget("LigthsAmbientColorButton", color_btn);
			
			Gdk::RGBA color = color_btn->get_rgba();
			ligth->ambient = {float(color.get_red()), float(color.get_green()), float(color.get_blue())};
			
		}
		
		else if (l_opt == LIGTHS_OPTIONS::SPECULAR)
		{
			Gtk::ColorButton *color_btn = nullptr;
			builder->get_widget("LigthsSpecularColorButton", color_btn);
			
			Gdk::RGBA color = color_btn->get_rgba();
			ligth->specular = {float(color.get_red()), float(color.get_green()), float(color.get_blue())};
		}
		
		else if (l_opt == FRUSTRUM)
		{
			Gtk::SpinButton *frustrum = nullptr;
			builder->get_widget("LigthsFrustrumSpin", frustrum);
			
			ligth->frustrum = float(frustrum->get_value());
		}
		
		else if (l_opt == STRENGTH)
		{
			Gtk::SpinButton *strength = nullptr;
			builder->get_widget("LigthsStrengthSpin", strength);
			
			ligth->strength = float(strength->get_value());
		}
		
		else if (l_opt == ATTENUATION)
		{
			Gtk::SpinButton *attenuation = nullptr;
			builder->get_widget("LigthsAttenuationSpin", attenuation);
			
			ligth->attenuation = float(attenuation->get_value());
		}
	
	}
	
	glAreaN->queue_draw();
}



