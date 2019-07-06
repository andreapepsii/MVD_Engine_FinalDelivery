//
//  Game.cpp
//
//  Copyright � 2018 Alun Evans. All rights reserved.
//

#include "Game.h"
#include "Shader.h"
#include "extern.h"
#include "Parsers.h"
#include "FloorScript.h"
#include "SpawnControllerScript.h"
#include "NavmeshScript.h"
#include "MoveScript.h"


Game::Game() {

}

//Nothing here yet
void Game::init(int w, int h) {

	window_width_ = w; window_height_ = h;
	//******* INIT SYSTEMS *******

	//init systems except debug, which needs info about scene
	control_system_.init();
	graphics_system_.init(window_width_, window_height_, "data/assets/");
	debug_system_.init(&graphics_system_);
    script_system_.init(&control_system_);
	gui_system_.init(window_width_, window_height_);
    animation_system_.init();
	particle_system_.init();
    
    graphics_system_.screen_background_color = lm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    createFreeCamera_(13.614, 16, 32, -0.466, -0.67, -0.579);

	/******** SHADERS **********/
	Shader* blend_shader = graphics_system_.loadShader("data/shaders/phong_blend.vert", "data/shaders/phong.frag");
	Shader* cubemap_shader = graphics_system_.loadShader("data/shaders/cubemap.vert", "data/shaders/cubemap.frag");
	Shader* terrain_shader = graphics_system_.loadShader("data/shaders/phong.vert", "data/shaders/terrain.frag");
	Shader* blend2_shader = graphics_system_.loadShader("data/shaders/phong_blend.vert", "data/shaders/phong.frag");
	Shader* phong_shader = graphics_system_.loadShader("data/shaders/phong.vert", "data/shaders/phong.frag");
	Shader* animation_shader = graphics_system_.loadShader("data/shaders/phong_anim.vert", "data/shaders/phong.frag");
	Shader* tileset_shader = graphics_system_.loadShader("data/shaders/tileset.vert", "data/shaders/tileset.frag");
	Shader* reflection_shader = graphics_system_.loadShader("data/shaders/reflection.vert", "data/shaders/reflection.frag");

	
	createBlendShape(blend2_shader);
	createSkybox(cubemap_shader);
	createTerrain(terrain_shader);

	//floor for ball animation
	int floor_entity = ECS.createEntity("floor");
	ECS.getComponentFromEntity<Transform>(floor_entity).translate(0.0f, -0.02f, 0.0f);
	ECS.getComponentFromEntity<Transform>(floor_entity).scale(0.25, 1.0, 0.25);
	Mesh& floor_mesh = ECS.createComponentForEntity<Mesh>(floor_entity);
	floor_mesh.geometry = graphics_system_.createGeometryFromFile("data/assets/floor_40x40.obj");
	floor_mesh.material = graphics_system_.createMaterial();
	graphics_system_.getMaterial(floor_mesh.material).shader_id = phong_shader->program;
	graphics_system_.getMaterial(floor_mesh.material).diffuse_map = Parsers::parseTexture("data/assets/block_blue.tga");
	
	createAIexample(phong_shader);
	addSimpleAnimation(phong_shader);
	addSkinnedAnimation(animation_shader, "data/assets/Punching.dae");
	createMaterialSetExample(tileset_shader);
	
	// particle effect
	int snow = ECS.createEntity("Snow");
	ParticleEmitter& particleEm = ECS.createComponentForEntity<ParticleEmitter>(snow);
	ECS.createComponentForEntity<Transform>(snow);
	ECS.getComponentFromEntity<Transform>(snow).rotateLocal(3.14, lm::vec3(1, 0, 0));
	ECS.getComponentFromEntity<Transform>(snow).translate(-140, 30, 0);
	particleEm.num_particles = 10000;
	particleEm.max_life = 500;
	particleEm.particle_size = 2.0;
	particle_system_.createParticle(particleEm);


	//general lighting
	// direction/spot light
	int ent_light_dir = ECS.createEntity("light_dir");
	ECS.getComponentFromEntity<Transform>(ent_light_dir).translate(0, 100, 80);
	Light& light_comp_dir = ECS.createComponentForEntity<Light>(ent_light_dir);
	light_comp_dir.color = lm::vec3(1.0f, 1.0f, 1.0f);
	light_comp_dir.direction = lm::vec3(0.0f, -1.0f, -0.4f);
	light_comp_dir.type = LightTypeDirectional; //change for direction or spot
	light_comp_dir.position = lm::vec3(0, 0, 0);
	light_comp_dir.forward = light_comp_dir.direction.normalize();
	light_comp_dir.setPerspective(60 * DEG2RAD, 1, 1, 200);
	light_comp_dir.update();
	light_comp_dir.cast_shadow = true;

    //******* LATE INIT AFTER LOADING RESOURCES *******//
    graphics_system_.lateInit();
    script_system_.lateInit();
    animation_system_.lateInit();
    debug_system_.lateInit();

	debug_system_.setActive(true);

}

//update each system in turn
void Game::update(float dt) {

	if (ECS.getAllComponents<Camera>().size() == 0) {print("There is no camera set!"); return;}

	//update input
	control_system_.update(dt);

	//collision
	collision_system_.update(dt);

    //animation
    animation_system_.update(dt);
    
	//scripts
	script_system_.update(dt);

	//render
	graphics_system_.update(dt);
    
	if (graphics_system_.particlesOn) {
		//particles
		particle_system_.update();
	}
    
	//gui
	gui_system_.update(dt);

	//debug
	debug_system_.update(dt);
   
}
//update game viewports
void Game::update_viewports(int window_width, int window_height) {
	window_width_ = window_width;
	window_height_ = window_height;

	auto& cameras = ECS.getAllComponents<Camera>();
	for (auto& cam : cameras) {
		cam.setPerspective(60.0f*DEG2RAD, (float)window_width_ / (float) window_height_, 0.01f, 10000.0f);
	}

	graphics_system_.updateMainViewport(window_width_, window_height_);
}

Material& Game::createMaterial(GLuint shader_program) {
    int mat_index = graphics_system_.createMaterial();
    Material& ref_mat = graphics_system_.getMaterial(mat_index);
    ref_mat.shader_id = shader_program;
    return ref_mat;
}

void Game::createSkybox(Shader* cubemap_shader){
	
	//environment
	int cubemap_geom = graphics_system_.createGeometryFromFile("data/assets/cubemap.obj");
	
	std::vector<std::string> daily_cube{
		"data/assets/skybox/right.tga","data/assets/skybox/left.tga",
		"data/assets/skybox/top.tga","data/assets/skybox/bottom.tga",
		"data/assets/skybox/front.tga", "data/assets/skybox/back.tga" };
	
	graphics_system_.setEnvironment(Parsers::parseCubemap(daily_cube),
		cubemap_geom, cubemap_shader->program);

}

void Game::createTerrain(Shader* terrain_shader) {

	//terrain material and noise map

   //noise map data - must be cleaned up
	ImageData noise_image_data;
	float terrain_height = 20.0f;

	int mat_terrain_index = graphics_system_.createMaterial();
	Material& mat_terrain = graphics_system_.getMaterial(mat_terrain_index);
	mat_terrain.name = "terrain";
	mat_terrain.shader_id = terrain_shader->program;
	mat_terrain.specular = lm::vec3(0, 0, 0);
	mat_terrain.diffuse_map = Parsers::parseTexture("data/assets/terrain/grass01.tga");
	mat_terrain.diffuse_map_2 = Parsers::parseTexture("data/assets/terrain/cliffs.tga");
	mat_terrain.normal_map = Parsers::parseTexture("data/assets/terrain/grass01_n.tga");
	//read texture, pass optional variables to get pointer to pixel data
	mat_terrain.noise_map = Parsers::parseTexture("data/assets/terrain/heightmap1.tga",
		&noise_image_data,
		true);
	mat_terrain.height = terrain_height;
	mat_terrain.uv_scale = lm::vec2(100, 100);

	//terrain
	//create terrain geometry - this function is a wrapper for Geometry::createTerrain
	int terrain_geometry = graphics_system_.createTerrainGeometry(500,
		0.4f,
		terrain_height,
		noise_image_data);

	//delete noise_image data other we have a memory leak
	delete noise_image_data.data;

	//terrain
	int terrain_entity = ECS.createEntity("Terrain");
	Mesh& terrain_mesh = ECS.createComponentForEntity<Mesh>(terrain_entity);
	Transform& transform_mesh = ECS.createComponentForEntity<Transform>(terrain_entity);
	transform_mesh.translate(lm::vec3(-140, 0, 0));
	terrain_mesh.geometry = terrain_geometry;
	terrain_mesh.material = mat_terrain_index;
	terrain_mesh.render_mode = RenderModeForward;

}

void Game::createBlendShape(Shader* blend_shader) {
	
	//Parsers::parseMTL("data/assets/toon/", "toon_base.mtl", graphics_system_.getMaterials(), blend_shader->program);

	
	int mat_toon_index = graphics_system_.createMaterial();
	Material& mat_toon = graphics_system_.getMaterial(mat_toon_index);
	mat_toon.name = "toon";
	mat_toon.shader_id = blend_shader->program;
	mat_toon.specular = lm::vec3(1, 1, 1);
	mat_toon.diffuse_map = Parsers::parseTexture("data/assets/toon/toon_base_Body_Diffuse.tga");
	mat_toon.transparency_map = Parsers::parseTexture("data/assets/toon/toon_base_Body_Opacity.tga");
	mat_toon.normal_map = Parsers::parseTexture("data/assets/toon/toon_base_Body_Normal.tga");

	int toon_ent = ECS.createEntity("toon");
	Mesh& toon_mesh = ECS.createComponentForEntity<Mesh>(toon_ent);
	Transform& toon_trans = ECS.createComponentForEntity<Transform>(toon_ent);
	toon_trans.translate(30, 0, -15);
	//create multi-material-set geometry
	toon_mesh.geometry = Parsers::parseOBJ_multi("data/assets/toon/toon_base.obj",
		graphics_system_.getGeometries(),
		graphics_system_.getMaterials());
	//get reference to geom object
	Geometry& toon_geom = graphics_system_.getGeometries()[toon_mesh.geometry];


	std::vector<float> vertices, normals, uvs;
	std::vector<unsigned int> indices;


	//add blend shapes here
	Parsers::parseOBJ("data/assets/toon/toon_happy.obj", vertices, uvs, normals, indices);

	toon_geom.addBlendShape(vertices);

	BlendShapes& blend_comp = ECS.createComponentForEntity<BlendShapes>(toon_ent);
	blend_comp.addShape("happy");
	blend_comp.blend_weights[0] = 1.0;

	std::vector<float> vertices2, normals2, uvs2;
	std::vector<unsigned int> indices2;

	Parsers::parseOBJ("data/assets/toon/toon_angry.obj", vertices2, uvs2, normals2, indices2);

	toon_geom.addBlendShape(vertices2);

	blend_comp.addShape("angry");
	blend_comp.blend_weights[1] = 1.0;

}

void Game::addSimpleAnimation(Shader* phong_shader){

	int ball_ent = ECS.createEntity("ball");
	Mesh& ball_mesh = ECS.createComponentForEntity<Mesh>(ball_ent);
	ball_mesh.geometry = graphics_system_.createGeometryFromFile("data/assets/ball.obj");
	ball_mesh.material = graphics_system_.createMaterial();
	graphics_system_.getMaterial(ball_mesh.material).shader_id = phong_shader->program;

	Parsers::parseAnimation("data/assets/bounce.anim");

}

void Game::addSkinnedAnimation(Shader* animation_shader, std::string filename) {

	Parsers::parseCollada(filename, animation_shader, graphics_system_);

}

void Game::createMaterialSetExample(Shader* phong_shader){
	
	//whole bunch of materials!
	Parsers::parseMTL("data/assets/nanosuit/", "nanosuit.mtl", graphics_system_.getMaterials(), phong_shader->program);

	int suit_geom = graphics_system_.createMultiGeometryFromFile("data/assets/nanosuit/nanosuit.obj");
	
	//basic blue material
	int mat_blue_check_index = graphics_system_.createMaterial();
	Material& mat_blue_check = graphics_system_.getMaterial(mat_blue_check_index);
	mat_blue_check.shader_id = phong_shader->program;
	mat_blue_check.diffuse_map = Parsers::parseTexture("data/assets/block_blue.tga");
	mat_blue_check.specular = lm::vec3(0, 0, 0);

	//suit 
	int suit_entity = ECS.createEntity("Suit");
	Transform& st = ECS.getComponentFromEntity<Transform>(suit_entity);
	st.scale(0.5f, 0.5f, 0.5f);
	st.translate(-5.0f, 0.0f, 0.0f);
	st.rotate(-45.0f*DEG2RAD, lm::vec3(0.0f, 1.0f, 0.0f));
	Mesh& suit_mesh = ECS.createComponentForEntity<Mesh>(suit_entity);
	suit_mesh.geometry = suit_geom;
	suit_mesh.material = mat_blue_check_index;
	suit_mesh.render_mode = RenderModeForward;

}

void Game::createAIexample(Shader * shader){

	//geometries
	int floor_geom_id = graphics_system_.createGeometryFromFile("data/assets/floor_5x5.obj");
	int teapot_geom_id = graphics_system_.createGeometryFromFile("data/assets/teapot_small.obj");

	//materials and textures
	int red_mat_id = graphics_system_.createMaterial();
	graphics_system_.getMaterial(red_mat_id).name = "red_mat_id";
	graphics_system_.getMaterial(red_mat_id).diffuse_map = Parsers::parseTexture("data/assets/red.tga");;
	graphics_system_.getMaterial(red_mat_id).shader_id = shader->program;
	graphics_system_.getMaterial(red_mat_id).specular = lm::vec3(0, 0, 0);

	std::cout << "To get a red tile (End point) press: " << red_mat_id << endl;

	int green_mat_id = graphics_system_.createMaterial();
	graphics_system_.getMaterial(green_mat_id).name = "green_mat_id";
	graphics_system_.getMaterial(green_mat_id).diffuse_map = Parsers::parseTexture("data/assets/green.tga");;
	graphics_system_.getMaterial(green_mat_id).shader_id = shader->program;
	graphics_system_.getMaterial(green_mat_id).specular = lm::vec3(0, 0, 0);

	std::cout << "To get a green tile (Start point) press: " << green_mat_id << endl;

	int white_mat_id = graphics_system_.createMaterial();
	graphics_system_.getMaterial(white_mat_id).name = "white_mat_id";
	graphics_system_.getMaterial(white_mat_id).diffuse_map = Parsers::parseTexture("data/assets/white.tga");;
	graphics_system_.getMaterial(white_mat_id).shader_id = shader->program;
	graphics_system_.getMaterial(white_mat_id).specular = lm::vec3(0, 0, 0);

	std::cout << "To get a white tile (Floor) press: " << white_mat_id << endl;

	int purple_mat_id = graphics_system_.createMaterial();
	graphics_system_.getMaterial(purple_mat_id).name = "purple_mat_id";
	graphics_system_.getMaterial(purple_mat_id).diffuse_map = Parsers::parseTexture("data/assets/block_purple.tga");;
	graphics_system_.getMaterial(purple_mat_id).shader_id = shader->program;
	graphics_system_.getMaterial(purple_mat_id).specular = lm::vec3(0, 0, 0);

	std::cout << "To get a pruple tile (Non-walkable floor) press: " << purple_mat_id << endl;

	int black_mat_id = graphics_system_.createMaterial();
	graphics_system_.getMaterial(black_mat_id).name = "black_mat_id";
	graphics_system_.getMaterial(black_mat_id).diffuse_map = Parsers::parseTexture("data/assets/block_teal.tga");;
	graphics_system_.getMaterial(black_mat_id).shader_id = shader->program;
	graphics_system_.getMaterial(black_mat_id).specular = lm::vec3(0, 0, 0);

	std::cout << "When the tiles are black, you can edit them with the previous mentioned buttons" << endl;

	//******* CREATE ENTITIES AND ADD COMPONENTS *******//

	//generates floor tiles 
	for (int i = 0; i < 12; i++) {
		for (int j = 0; j < 8; j++) {
			int tag = (i * 8) + j;
			int num = ECS.createEntity("floor_" + to_string(tag));
			Transform& trans = ECS.createComponentForEntity<Transform>(num);
			trans.translate(lm::vec3(15.0 - i * 2.5, 0.0, - 10 - j * 2.5));
			trans.scaleLocal(0.5, 0.5, 0.5);
			Mesh& mesh = ECS.createComponentForEntity<Mesh>(num);
			mesh.geometry = floor_geom_id;
			mesh.material = black_mat_id;
			cout << mesh.geometry << " " << mesh.material << endl;
		}
	}

	//generates character to go throw grid
	int teapot = ECS.createEntity("teapot");
	Transform& trans = ECS.createComponentForEntity<Transform>(teapot);
	trans.translate(lm::vec3(0.0f, -1.0f, -20.0f));
	Mesh& mesh = ECS.createComponentForEntity<Mesh>(teapot);
	mesh.geometry = teapot_geom_id;
	mesh.material = purple_mat_id;
	//int teapot = createObj("teapot", lm::vec3(0.0f, 13.0f, -21.5f), 0.0, lm::vec3(1.0, 0.0, 0.0), lm::vec3(1.0, 1.0, 1.0), teapot_geom_id, purple_mat_id, lm::vec3(0.0f, 0.5f, 0.0f), lm::vec3(0.75f, 0.5f, 0.5f));
	//players, cameras and lights

	//******* INIT SCRIPTS SYSTEM ******//

	//script to control floor set up and draw it 
	FloorScript* fs = new FloorScript(0);
	script_system_.registerScript(fs);
	//script to control character

	//script to control the player spawning with a minimum correct path
	SpawnControllerScript* scs = new SpawnControllerScript(teapot);
	script_system_.registerScript(scs);
	scs->fs = fs;

	//script to check where the character can move
	NavmeshScript* nms = new NavmeshScript(teapot);
	script_system_.registerScript(nms);
	nms->fs = fs;
	nms->scs = scs;

	//script to move character into next position
	MoveScript* ms = new MoveScript(teapot);
	script_system_.registerScript(ms);
	ms->nms = nms;

}

int Game::createFreeCamera_(float px, float py, float pz, float fx, float fy, float fz) {
	int ent_player = ECS.createEntity("PlayerFree");
	Camera& player_cam = ECS.createComponentForEntity<Camera>(ent_player);
    lm::vec3 the_position(px, py, px);
	ECS.getComponentFromEntity<Transform>(ent_player).translate(the_position);
	player_cam.position = the_position;
	player_cam.forward = lm::vec3(fx, fy, fz);
	player_cam.setPerspective(60.0f*DEG2RAD, (float)window_width_/(float)window_height_, 0.1f, 1000.0f);

	ECS.main_camera = ECS.getComponentID<Camera>(ent_player);

	control_system_.control_type = ControlTypeFree;

	return ent_player;
}

int Game::createPlayer_(float aspect, ControlSystem& sys) {
	int ent_player = ECS.createEntity("PlayerFPS");
	Camera& player_cam = ECS.createComponentForEntity<Camera>(ent_player);
	lm::vec3 the_position(0.0f, 3.0f, 5.0f);
	ECS.getComponentFromEntity<Transform>(ent_player).translate(the_position);
	player_cam.position = the_position;
	player_cam.forward = lm::vec3(0.0f, 0.0f, -1.0f);
	player_cam.setPerspective(60.0f*DEG2RAD, aspect, 0.01f, 10000.0f);

	//FPS colliders 
	//each collider ray entity is parented to the playerFPS entity
	int ent_down_ray = ECS.createEntity("Down Ray");
	Transform& down_ray_trans = ECS.getComponentFromEntity<Transform>(ent_down_ray);
	down_ray_trans.parent = ECS.getComponentID<Transform>(ent_player); //set parent as player entity *transform*!
	Collider& down_ray_collider = ECS.createComponentForEntity<Collider>(ent_down_ray);
	down_ray_collider.collider_type = ColliderTypeRay;
	down_ray_collider.direction = lm::vec3(0.0, -1.0, 0.0);
	down_ray_collider.max_distance = 100.0f;

	int ent_left_ray = ECS.createEntity("Left Ray");
	Transform& left_ray_trans = ECS.getComponentFromEntity<Transform>(ent_left_ray);
	left_ray_trans.parent = ECS.getComponentID<Transform>(ent_player); //set parent as player entity *transform*!
	Collider& left_ray_collider = ECS.createComponentForEntity<Collider>(ent_left_ray);
	left_ray_collider.collider_type = ColliderTypeRay;
	left_ray_collider.direction = lm::vec3(-1.0, 0.0, 0.0);
	left_ray_collider.max_distance = 1.0f;

	int ent_right_ray = ECS.createEntity("Right Ray");
	Transform& right_ray_trans = ECS.getComponentFromEntity<Transform>(ent_right_ray);
	right_ray_trans.parent = ECS.getComponentID<Transform>(ent_player); //set parent as player entity *transform*!
	Collider& right_ray_collider = ECS.createComponentForEntity<Collider>(ent_right_ray);
	right_ray_collider.collider_type = ColliderTypeRay;
	right_ray_collider.direction = lm::vec3(1.0, 0.0, 0.0);
	right_ray_collider.max_distance = 1.0f;

	int ent_forward_ray = ECS.createEntity("Forward Ray");
	Transform& forward_ray_trans = ECS.getComponentFromEntity<Transform>(ent_forward_ray);
	forward_ray_trans.parent = ECS.getComponentID<Transform>(ent_player); //set parent as player entity *transform*!
	Collider& forward_ray_collider = ECS.createComponentForEntity<Collider>(ent_forward_ray);
	forward_ray_collider.collider_type = ColliderTypeRay;
	forward_ray_collider.direction = lm::vec3(0.0, 0.0, -1.0);
	forward_ray_collider.max_distance = 1.0f;

	int ent_back_ray = ECS.createEntity("Back Ray");
	Transform& back_ray_trans = ECS.getComponentFromEntity<Transform>(ent_back_ray);
	back_ray_trans.parent = ECS.getComponentID<Transform>(ent_player); //set parent as player entity *transform*!
	Collider& back_ray_collider = ECS.createComponentForEntity<Collider>(ent_back_ray);
	back_ray_collider.collider_type = ColliderTypeRay;
	back_ray_collider.direction = lm::vec3(0.0, 0.0, 1.0);
	back_ray_collider.max_distance = 1.0f;

	//the control system stores the FPS colliders 
	sys.FPS_collider_down = ECS.getComponentID<Collider>(ent_down_ray);
	sys.FPS_collider_left = ECS.getComponentID<Collider>(ent_left_ray);
	sys.FPS_collider_right = ECS.getComponentID<Collider>(ent_right_ray);
	sys.FPS_collider_forward = ECS.getComponentID<Collider>(ent_forward_ray);
	sys.FPS_collider_back = ECS.getComponentID<Collider>(ent_back_ray);

	ECS.main_camera = ECS.getComponentID<Camera>(ent_player);

	sys.control_type = ControlTypeFPS;

	return ent_player;
}

