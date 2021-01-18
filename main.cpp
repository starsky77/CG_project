#include "utils.h"
#include "nurbs/draw_objects.h"

//windows
GLFWwindow* window;
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// settings
static const char *varyings[] = {
	"selected_alias"
	// "alias"
	// "TexCoords","selected_alias","a","b","c"
};

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
double lastX = SCR_WIDTH / 2.0f;
double lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 LightPositions[] = {
	glm::vec3(1.2f, 1.0f, 2.0f),
	glm::vec3(1.2f, 2.0f, 0.0f),
	glm::vec3(-1.2f, 2.0f, 2.0f),
	glm::vec3(-1.2f, 2.0f, 0.0f)};

glm::vec3 &lightPos(LightPositions[0]);


//Others
unsigned int depthMapFBO, depthMap;
extern const int SHADOW_WIDTH, SHADOW_HEIGHT;
static const int MAX_OBJECTS = 50;
ObjTree* tree = NULL, * objects[MAX_OBJECTS];
int current_object, object_cnt = 1;




int init()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return 0;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, click_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return 0;
	}
	glEnable(GL_DEPTH_TEST);

	// 2021-1-18 -sxj
	init_nurbs();

	return 1;
}


int main()
{

	if (!init())
	{
		exit(0);
	}

	// build and compile our shader zprogram
	//------------------------------------------------------------------------------------------------
	Shader cursorShader("shaders/cursor.vert", "shaders/cursor.frag", "shaders/cursor.geom");
	Shader lightingShader("shaders/1.color.vert", "shaders/1.color.frag");
	Shader simpleShader("shaders/1.color.vs", "shaders/simple.fs");
	Shader screenShader("shaders/view.vs", "shaders/core.fs");
	Shader skyboxShader("shaders/skycube.vs", "shaders/skycube.fs");
	Shader depthShader("shaders/1.color.vs", "shaders/simple.frag");
	Shader cornerShader("shaders/view.vs", "shaders/core.frag");

	meshShader = &lightingShader;
	// Shader lightingShader("shaders/geom.vert","shaders/geom.frag","shaders/geom_.geom",varyings);
	// Shader lightingShader("shaders/1.color_.vert", "shaders/1.color_.frag","shaders/pass_through.geom");
	// Shader lightCubeShader("shaders/1.light_cube.vs", "shaders/1.light_cube.fs");
	// unsigned int feedback_vbo = lightingShader.vbo[0], select_xfb = lightingShader.xfb;
	// unsigned int select_program = lightingShader.ID;
	// unsigned int select_program=Feedback_Initialize(&feedback_vbo,&select_xfb);
	// Shader lightingShader("shaders/cursor.vert", "shaders/cursor.frag", "shaders/cursor.geom");
	//--------------------------------------------------------------------------------------------------
	

	// select buffers setup
	//------------------------------------------------------------------------------------------------
	unsigned int tex, buf;
	// Generate a name for the buffer object, bind it to the
	// GL_TEXTURE_BINDING, and allocate 4K for the buffer
	glGenBuffers(1, &buf);
	glBindBuffer(GL_TEXTURE_BUFFER, buf);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(int), NULL, GL_DYNAMIC_READ);
	// Generate a new name for our texture
	glGenTextures(1, &tex);
	// Bind it to the buffer texture target to create it
	glBindTexture(GL_TEXTURE_BUFFER, tex);
	// Attach the buffer object to the texture and specify format as
	// single channel floating point
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32I, buf);
	// Now bind it for read-write to one of the image units
	glBindImageTexture(0, tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32I);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	unsigned int quadVBO, quadVAO;
	float corner[] = {
		0.5f, 1.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.0f, 0.0f,
		1.0f, 0.5f, 1.0f, 0.0f,
		0.5f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.5f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f};
	float quad[] = {
		-1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f};
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);

	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), &quad, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	unsigned int cornerVAO, cornerVBO;
	glGenVertexArrays(1, &cornerVAO);
	glGenBuffers(1, &cornerVBO);
	glBindVertexArray(cornerVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cornerVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(corner), &corner, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//------------------------------------------------------------------------------------------------
	//--------cube texture
	std::vector<std::string> faces{
		"skybox/right.jpg",
		"skybox/left.jpg",
		"skybox/top.jpg",
		"skybox/bottom.jpg",
		"skybox/front.jpg",
		"skybox/back.jpg"};
	unsigned int cubemapTexture = loadCubemap(faces);

	screenShader.use();
	screenShader.setInt("screenTexture", 0);
	cornerShader.setInt("screenTexture", 0);
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	unsigned int texColorBuffer;
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texColorBuffer, 0);

	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "error: framebuffer\n";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	gen_preview_framebuffer();


	//----------------------------------------------------------------------------------------------------
	// load models texture and light
	//Model temple("mod/nanosuit/nanosuit.obj");
	//Model temple("mod/bmw/bmw.obj");
	Model temple("mod/bunny/bunny.obj");
	//Model temple("nanosuit/nanosuit.obj");
	//Model temple("gallery/gallery.obj");
	Light lights(LightPositions, 4);
	mesh_Bunny = new Model("mod/bunny/bunny.obj");
	unsigned int diffuseMap = loadTexture("container2.png");
	unsigned int specularMap = loadTexture("container2_specular.png");


	// shader configuration
	lightingShader.use();
	lightingShader.setInt("material.diffuse", 0);
	lightingShader.setInt("material.specular", 1);
	lightingShader.setInt("shadowMap", 2);
	lightingShader.setFloat("material.shininess", 64);



	// render loop-----------------------------------------------------------------------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
		// Shader &genericShader=feedback?cursorShader:lightingShader;
		Shader &genericShader=cursor_hidden?lightingShader:cursorShader;
		// if(!shader_selection)glEnable(GL_RASTERIZER_DISCARD);

		double currentFrame = glfwGetTime();
		// genericShader.setFloat("time",currentFrame);
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		processInput(window);
		// glBindFramebuffer(GL_FRAMEBUFFER,framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, postrender ? framebuffer : 0);
		// glBindFramebuffer(GL_FRAMEBUFFER,0);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 1, 0XFF);
		glStencilMask(0XFF);
		genericShader.use();


		float scale = 1.02f;
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 tmpmodel = glm::scale(model, glm::vec3(scale, scale, scale));
		glm::mat4 lightSpaceTrans = glm::lookAt(lightPos, glm::vec3(0.0f), camera.WorldUp);

		if (display_corner)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
			glEnable(GL_DEPTH_TEST);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			depthShader.use();

			// view/projection transformations
			model = glm::mat4(1.0f);

			// depthShader.setMat4("projection",projection);
			depthShader.setMat4("projection", glm::perspective(glm::radians(89.0f), (float)SHADOW_WIDTH / SHADOW_HEIGHT, 0.1f, 10.0f));
			depthShader.setMat4("view", lightSpaceTrans);
			depthShader.setMat4("model", model);
			depthShader.setVec3("viewPos", lightPos);
			// bind diffuse map
			renderPlane();
			// TODO:
			drawNURBSSurface();


			
			if(tree)DrawObjCollection(tree,depthShader);
			if (model_draw)
			{
				depthShader.setMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.1f, 0.0f)));
				temple.Draw(depthShader);
			}

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, postrender ? framebuffer : 0);
			model = glm::mat4(1.0f);
		}
		// glm::mat4 pick=glm::pickMatrix(glm::vec2(),glm::vec2(),glViewport())

		int viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		genericShader.use();
		genericShader.setVec2("pickPosition", glm::vec2(lastX / viewport[2] * 2 - 1.0f, (1 - lastY / viewport[3]) * 2 - 1.0f));
		// if (feedback)
		// {
		// 	// glEnable(GL_RASTERIZER_DISCARD);
		// 	glUseProgram(select_program);
		// 	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, select_xfb);
		// 	glBeginTransformFeedback(GL_TRIANGLES);

		// 	// else glResumeTransformFeedback();
		// 	// renderCube();
		// }
		genericShader.setMat4("lightView", glm::perspective(glm::radians(89.0f), (float)SHADOW_WIDTH / SHADOW_HEIGHT, 0.1f, 10.0f) * lightSpaceTrans);
		view = camera.GetViewMatrix();
		genericShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		genericShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		genericShader.setVec3("lightPos", lightPos);
		genericShader.setVec3("viewPos", camera.Position);
		// view/projection transformations
		genericShader.setMat4("projection", projection);
		genericShader.setMat4("view", view);

		// world transformation
		genericShader.setMat4("model", model);

		// bind specular map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
		if (display_corner)
		{
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, depthMap);
		}

		// FIXME: should do the select pass in reverse order
		genericShader.setInt("alias", 0);
		renderPlane();
		// TODO:
		drawNURBSSurface();



		// bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		

		
		simpleShader.setMat4("projection", projection);
		simpleShader.setMat4("view", view);
		if (tree)
		{
			DrawObjCollection(tree, genericShader,current_object,&simpleShader);
		}
		if (!cursor_hidden && objectType)
		{
			model = glm::mat4(glm::mat3(camera.Right, camera.Up, -camera.Front));
			model = glm::translate(model, camera.Position * glm::mat3(model) + glm::vec3(0.0, 0.0, -3.0));
			genericShader.setMat4("model", model);
			if (objectType % 4 == 0)
			{
				renderCube();
			}
			else if (objectType % 4 == 1)
			{
				renderCube_2();
			}
			else if (objectType % 4 == 2)
			{
				renderCube_3();
			}
			else if (objectType % 4 == 3)
			{
				renderMesh_1();
			}
			if (draw_request)
			{
				if (!tree)
				{
					if (objectType % 4 == 0)
					{
						objects[object_cnt] = tree = CreatLeafnode(object_cnt, 's', model, renderCube);
					}
					else if (objectType % 4 == 1)
					{
						objects[object_cnt] = tree = CreatLeafnode(object_cnt, 's', model, renderCube_2);
					}
					else if (objectType % 4 == 2)
					{
						objects[object_cnt] = tree = CreatLeafnode(object_cnt, 's', model, renderCube_3);
					}
					else if (objectType % 4 == 3)
					{
						objects[object_cnt] = tree = CreatLeafnode(object_cnt, 's', model, renderMesh_1);
					}
					object_cnt++;
				}
				else
				{
					if (objectType % 4 == 0)
					{
						objects[object_cnt] = CreatLeafnode(object_cnt, 's', model, renderCube);
					}
					else if (objectType % 4 == 1)
					{
						objects[object_cnt] = CreatLeafnode(object_cnt, 's', model, renderCube_2);
					}
					else if (objectType % 4 == 2)
					{
						objects[object_cnt] = CreatLeafnode(object_cnt, 's', model, renderCube_3);
					}
					else if (objectType % 4 == 3)
					{
						objects[object_cnt] = CreatLeafnode(object_cnt, 's', model, renderMesh_1);
					}
					objects[object_cnt]->rightSibling = tree->rightSibling;
					tree->rightSibling = objects[object_cnt++];
				}
				draw_request = false;
			}
		}
		if (model_draw)
		{
			// genericShader.use();
			genericShader.setInt("alias", 2);
			genericShader.setMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.1f, 0.0f)));
			temple.Draw(genericShader);
		}
		if (feedback)
		{
			// glEndTransformFeedback();
			// glDisable(GL_RASTERIZER_DISCARD);
			glGetNamedBufferSubData(buf, 0, sizeof(int), &current_object);
			std::cout << current_object << std::endl;
			static int none[1]={-1};
			//     bool b=glUnmapNamedBuffer(feedback_vbo);
			// glPauseTransformFeedback();
			// glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, 5 * sizeof(int), NULL, GL_DYNAMIC_READ);
			glBufferData(GL_TEXTURE_BUFFER, sizeof(int), none, GL_DYNAMIC_READ);

			// glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
		}

		// also draw the lamp object
		lights.Draw(camera);
		if (skybox)
		{
			glStencilMask(0x00);
			// skybox
			// glDepthMask(GL_FALSE);
			glm::mat4 skyview = glm::mat4(glm::mat3(view));
			glDepthFunc(GL_LEQUAL);
			skyboxShader.use();
			skyboxShader.setMat4("projection", projection);
			skyboxShader.setMat4("view", skyview);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
			renderCube();
			// glBindVertexArray(cubeVAO);
			// glDrawArrays(GL_TRIANGLES,0,36);

			// glDepthMask(GL_TRUE);
			glDepthFunc(GL_LESS);
		}
		if (edge)
		{
			glStencilFunc(GL_NOTEQUAL, 1, 0XFF);
			// glStencilMask(0x00);
			glDisable(GL_DEPTH_TEST);
			simpleShader.use();
			simpleShader.setMat4("projection", projection);
			simpleShader.setMat4("view", view);
			simpleShader.setMat4("model", tmpmodel);
			// renderCube();
			// glDrawArrays(GL_TRIANGLES,0,36);
			glStencilMask(0xFF);
			glEnable(GL_DEPTH_TEST);
			glStencilFunc(GL_ALWAYS, 1, 0XFF);
		}

		if (postrender)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDisable(GL_DEPTH_TEST);
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			screenShader.use();
			glBindVertexArray(quadVAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texColorBuffer);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		if (display_corner)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDisable(GL_DEPTH_TEST);
			cornerShader.use();
			glBindVertexArray(cornerVAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, depthMap);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
	if (Motion)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			lightPos += 2.5f * deltaTime * camera.Front;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			lightPos -= 2.5f * deltaTime * camera.Front;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			lightPos -= 2.5f * deltaTime * camera.Right;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			lightPos += 2.5f * deltaTime * camera.Right;
	}
	else
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.ProcessKeyboard(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.ProcessKeyboard(BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.ProcessKeyboard(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.ProcessKeyboard(RIGHT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		postrender = !postrender;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		edge = !edge;
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		skybox = !skybox;
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		model_draw = !model_draw;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		display_corner = !display_corner;
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		Motion = !Motion;
	// if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
	// 	shader_selection=!shader_selection;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		if (cursor_hidden)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			firstMouse = true;
			glfwSetCursorPos(window, SCR_WIDTH / 2, SCR_HEIGHT / 2);
		}
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		cursor_hidden = !cursor_hidden;
	}
}


void click_callback(GLFWwindow *window, int button, int action, int mods)
{
	if (cursor_hidden)
		return;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		feedback = true;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
		feedback = false;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		draw_request = true;
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	if (current_object > 0 && feedback &&current_object < object_cnt)
	{
		// // ObjectMove(objects,current_object,xoffset,yoffset);
		glm::mat4 &model = objects[current_object]->model;
		try
		{
			double distance = glm::length(camera.Position - glm::vec3(model[4]));
			auto rad = glm::vec3(xoffset, yoffset, 0.0f);
			rad *= (1.0 / (double)SCR_WIDTH * glm::radians(camera.Zoom)) * distance;
			auto rot = glm::mat3(model);
			auto cam = glm::mat3(camera.Right, camera.Up, -camera.Front);
			model = glm::translate(model, glm::vec3(cam * rad * rot));
		}
		catch (...)
		{
			std::cout << "error";
		};
	}
	lastX = xpos;
	lastY = ypos;

	if (cursor_hidden)
		camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
	if (cursor_hidden)
		camera.ProcessMouseScroll(yoffset);
	else
	{
		if (yoffset > 0)
			objectType += int(yoffset);
		else
			objectType = 0;
	}
}


