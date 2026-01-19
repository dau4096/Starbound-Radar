#include "includes.h"
#include "global.h"
#include "utils.h"
#include <stb_image.h>
#include <stb_image_write.h>
using namespace std;
using namespace utils;
using namespace glm;


void APIENTRY openGLErrorCallback(
		GLenum source,
		GLenum type, GLuint id,
		GLenum severity,
		GLsizei length, const GLchar* message,
		const void* userParam
	) {
	/*
	Nicely formatted callback from;
	[https://learnopengl.com/In-Practice/Debugging]
	*/
	if ((id == 131169u) || (id == 131185u) || (id == 131218u) || (id == 131204u)) {return; /* Ignored warning IDs that are not errors */}

	std::cout << "---------------" << std::endl << "Debug message (" << id << ") | " << message << std::endl;

	switch (source)
	{
		case GL_DEBUG_SOURCE_API:             {std::cout << "Source: API"; break;}
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   {std::cout << "Source: Window System"; break;}
		case GL_DEBUG_SOURCE_SHADER_COMPILER: {std::cout << "Source: Shader Compiler"; break;}
		case GL_DEBUG_SOURCE_THIRD_PARTY:     {std::cout << "Source: Third Party"; break;}
		case GL_DEBUG_SOURCE_APPLICATION:     {std::cout << "Source: Application"; break;}
		case GL_DEBUG_SOURCE_OTHER:           {std::cout << "Source: Other"; break;}
	} std::cout << std::endl;

	switch (type)
	{
		case GL_DEBUG_TYPE_ERROR:               {std::cout << "Type: Error"; break;}
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: {std::cout << "Type: Deprecated Behaviour"; break;}
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  {std::cout << "Type: Undefined Behaviour"; break;} 
		case GL_DEBUG_TYPE_PORTABILITY:         {std::cout << "Type: Portability"; break;}
		case GL_DEBUG_TYPE_PERFORMANCE:         {std::cout << "Type: Performance"; break;}
		case GL_DEBUG_TYPE_MARKER:              {std::cout << "Type: Marker"; break;}
		case GL_DEBUG_TYPE_PUSH_GROUP:          {std::cout << "Type: Push Group"; break;}
		case GL_DEBUG_TYPE_POP_GROUP:           {std::cout << "Type: Pop Group"; break;}
		case GL_DEBUG_TYPE_OTHER:               {std::cout << "Type: Other"; break;}
	} std::cout << std::endl;
	
	switch (severity)
	{
		case GL_DEBUG_SEVERITY_HIGH:         {std::cout << "Severity: high"; break;}
		case GL_DEBUG_SEVERITY_MEDIUM:       {std::cout << "Severity: medium"; break;}
		case GL_DEBUG_SEVERITY_LOW:          {std::cout << "Severity: low"; break;}
		case GL_DEBUG_SEVERITY_NOTIFICATION: {std::cout << "Severity: notification"; break;}
	} std::cout << std::endl << std::endl;

	if (dev::PAUSE_ON_OPENGL_ERROR) {
		utils::pause();
	}
}





GLuint compileShader(GLenum shaderType, string filePath) {
	std::string source = utils::readFile(filePath);
	const char* src = source.c_str();

	GLuint shader = glCreateShader(shaderType);
	if (shader == 0) {
		raise("Error: Failed to create shader.");
		return 0;
	}

	glShaderSource(shader, 1, &src, nullptr);

	glCompileShader(shader);
	

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		if (!utils::isConsoleVisible()) {
			utils::showConsole();
		}
		char infolog[512];
		glGetShaderInfoLog(shader, 512, nullptr, infolog);
		raise("Error: Shader compilation failed;\n" + string(infolog));
	}

	return shader;
}




namespace uniforms {

//Uniforms; [Many overloads]
static inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, bool value) {
	GLuint location = glGetUniformLocation(shaderProgram, uniformName);
	if (location >= 0) {
		glUniform1i(location, value);
	}
}
static inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, size_t value) {
	GLuint location = glGetUniformLocation(shaderProgram, uniformName);
	if (location >= 0) {
		glUniform1ui(location, value);
	}
}
static inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, int value) {
	GLuint location = glGetUniformLocation(shaderProgram, uniformName);
	if (location >= 0) {
		glUniform1i(location, value);
	}
}
static inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, float value) {
	GLuint location = glGetUniformLocation(shaderProgram, uniformName);
	if (location >= 0) {
		glUniform1f(location, value);
	}
}
static inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, glm::ivec2 value) {
	GLuint location = glGetUniformLocation(shaderProgram, uniformName);
	if (location >= 0) {
		glUniform2i(location, value.x, value.y);
	}
}
static inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, glm::vec2 value) {
	GLuint location = glGetUniformLocation(shaderProgram, uniformName);
	if (location >= 0) {
		glUniform2f(location, value.x, value.y);
	}
}
static inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, glm::ivec3 value) {
	GLuint location = glGetUniformLocation(shaderProgram, uniformName);
	if (location >= 0) {
		glUniform3i(location, value.x, value.y, value.z);
	}
}
static inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, glm::vec3 value) {
	GLuint location = glGetUniformLocation(shaderProgram, uniformName);
	if (location >= 0) {
		glUniform3f(location, value.x, value.y, value.z);
	}
}
static inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, glm::ivec4 value) {
	GLuint location = glGetUniformLocation(shaderProgram, uniformName);
	if (location >= 0) {
		glUniform4i(location, value.x, value.y, value.z, value.w);
	}
}
static inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, glm::vec4 value) {
	GLuint location = glGetUniformLocation(shaderProgram, uniformName);
	if (location >= 0) {
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}
}

}





namespace graphics {

GLFWwindow* initialiseWindow(glm::ivec2 resolution, const char* title) {
	if (!glfwInit()) {
		raise("Failed to initialize GLFW");
		return nullptr;
	}


	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, display::OPENGL_VERSION_MAJOR);  //OpenGL major ver (4)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, display::OPENGL_VERSION_MINOR);  //OpenGL minor ver (6)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  //Use Core (not ES)


	GLFWwindow* Window = glfwCreateWindow(resolution.x, resolution.y, title, NULL, NULL);
	if (!Window) {
		glfwTerminate();
		raise("Failed to create GLFW window");
		return nullptr;
	}
	glfwMakeContextCurrent(Window);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		raise("Failed to initialize GLEW.");
	}

	glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	return Window;
}



GLuint createShaderProgram(std::string fragShaderName, std::string vertexShaderName) {
	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, "src/shaders/"+ vertexShaderName);
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, "src/shaders/"+ fragShaderName);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	GLint success;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		if (!utils::isConsoleVisible()) {
			utils::showConsole();
		}
		char infolog[512];
		glGetProgramInfoLog(shaderProgram, 512, nullptr, infolog);
		raise("Error: Program linking failed;\n" + string(infolog));
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}




GLuint createComputeShader(std::string compShaderName) {
	GLuint computeShader = compileShader(GL_COMPUTE_SHADER, "src/shaders/" + compShaderName);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, computeShader);
	glLinkProgram(shaderProgram);

	GLint success;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		if (!utils::isConsoleVisible()) {
			utils::showConsole();
		}
		char infolog[512];
		glGetProgramInfoLog(shaderProgram, 512, nullptr, infolog);
		raise("Error: Compute shader program linking failed:\n" + std::string(infolog));
	}

	glDeleteShader(computeShader);

	return shaderProgram;
}




GLuint createShaderStorageBufferObject(int binding, size_t bufferSize=0, GLuint glType=GL_DYNAMIC_DRAW) {
	GLuint SSBO;
	glGenBuffers(1, &SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, nullptr, glType);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	return SSBO;
}

//Different types for GPU and CPU (padding, unecessary data removed.)
template<typename TGPU, typename TCPU>
void updateShaderStorageBufferObject(
		GLuint SSBO, std::vector<TCPU>* dataSetIn, int allocSize
	) {

	size_t singleItemSize = sizeof(TGPU);
	size_t size = (allocSize == -1) ? dataSetIn->size() : allocSize;
	std::vector<TGPU> dataSet;

	for (size_t index=0; index<size; index++) {
		dataSet.push_back(TGPU(*(dataSetIn->data() + index)));
	}

	if (size > 0 && !dataSet.empty()) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, singleItemSize * size, dataSet.data());
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
}

//Overload of the above without the size specified.
template<typename TGPU, typename TCPU>
void updateShaderStorageBufferObject(
		GLuint SSBO, std::vector<TCPU>* dataSetIn
	) {
	updateShaderStorageBufferObject(SSBO, dataSetIn, dataSetIn->size());
}

//Same type for CPU & GPU.
template<typename T>
void updateShaderStorageBufferObject(
	GLuint SSBO,
	T* data,
	size_t count
) {
	size_t size = sizeof(T) * count;

	if (count > 0) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, size, data);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
}



//// TEXTURES ////
void saveImage(GLuint textureID, bool silent=false) {
    std::vector<unsigned char> pixels(currentRenderResolution.x * currentRenderResolution.y * 3u);

    glBindTexture(GL_TEXTURE_2D, textureID);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    glBindTexture(GL_TEXTURE_2D, 0);

	stbi_flip_vertically_on_write(true);

	std::filesystem::path dirName = std::filesystem::path("saved.images");
	std::filesystem::create_directories(dirName);

	std::string timeStr = utils::getTimestamp();
	std::filesystem::path imagePath = dirName / (timeStr + ".png");

	stbi_write_png(
		imagePath.string().c_str(),
		currentRenderResolution.x, currentRenderResolution.y,
		3u, pixels.data(), currentRenderResolution.x*3u
	);

	if (!silent) {std::cout << "Successfully saved image as : [" << imagePath << "]" << std::endl;}
}



GLuint createGLImage2D(size_t width, size_t height, GLint internalFormat=GL_RGBA32F, GLint samplingType=GL_NEAREST, GLint edgeSampling=GL_REPEAT) {
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexStorage2D(GL_TEXTURE_2D, 1, internalFormat, width, height);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, samplingType);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, samplingType);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}


GLuint loadGLTexture2D(const std::string textureName, glm::ivec2 expectedRes=glm::ivec2(-1, -1)) {
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	int width, height, channels;
	std::string texturePath = "textures/" + textureName + ".png";
	unsigned char* textureData = stbi_load(
		texturePath.c_str(),
		&width, &height,
		&channels, 4
	);

	if (
		!textureData ||
		((expectedRes.x != -1) && (width != expectedRes.x)) ||
		((expectedRes.y != -1) && (height != expectedRes.y))
	) { //If not valid image path or wrong res.
		std::cout << "Failed to load texture : " << textureName << ".png : Image was not found/correct resolution." << std::endl;
		std::cerr << "Expected [" << expectedRes.x << ", " << expectedRes.y << "] : Got [" << width << ", " << height << "]" << std::endl;
		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteTextures(1, &textureID);
		return 0;
	}



	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(textureData);

	return textureID;
}


GLuint createTexture2DArray(std::vector<std::string>& textureNames, glm::ivec2 expectedRes=display::TEXTURE_RESOLUTION) {
	GLuint sheetArrayID;
	glGenTextures(1, &sheetArrayID);
	glBindTexture(GL_TEXTURE_2D_ARRAY, sheetArrayID);


	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, expectedRes.x, expectedRes.y, textureNames.size(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT); //Textures repeat UV.
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT); // ^ ^ ^ ^ ^ ^ ^ ^ ^
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); //Clamp to nearest layer


	int width, height, channels;
	int layerIndex = -1;
	for (const std::string& textureName : textureNames) {
		layerIndex++;
		if (layerIndex >= textureNames.size()) {break; /* Reached final layer. */}
		if (textureName.empty()) {continue; /* Ignore blank entries. */}

		std::string texturePath = "textures/" + textureName;
		unsigned char* textureData = stbi_load(
			texturePath.c_str(),
			&width, &height,
			&channels, 4 //RGBA, 4 channels.
		);

		if (
			!textureData ||
			((expectedRes.x != -1) && (width != expectedRes.x)) ||
			((expectedRes.y != -1) && (height != expectedRes.y))
		) { //Could not find, or was wrong res.
			std::cerr << "Could not find: [" << ("textures/" + textureName) << "]." << std::endl;
			stbi_image_free(textureData);
			continue; //Invalid layer.
		}

		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layerIndex, expectedRes.x, expectedRes.y, 1, GL_RGBA, GL_UNSIGNED_BYTE, textureData);

	}

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	return sheetArrayID;
}


GLuint createGLImage2DArray(glm::ivec3 resolution, GLenum filtering=GL_NEAREST) {
	GLuint arrayID;
	glGenTextures(1, &arrayID);
	glBindTexture(GL_TEXTURE_2D_ARRAY, arrayID);
	glTexStorage3D(
		GL_TEXTURE_2D_ARRAY, 1, GL_RGBA32F,
		resolution.x, resolution.y, resolution.z //Width, Height, Layers.
	);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, filtering);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, filtering);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	return arrayID;
}
//// TEXTURES ////




inline GLuint getEmptyVAO() {
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	return VAO;
}




//// FRAMEBUFFERS ////
//Without depth texture
GLuint createAFBO(glm::uvec2 resolution, GLuint& colourTexture) {
    GLuint FBO;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    //Colour
    glGenTextures(1, &colourTexture);
    glBindTexture(GL_TEXTURE_2D, colourTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, resolution.x, resolution.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colourTexture, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, drawBuffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        raise("Colour FBO incomplete!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return FBO;
}

//With depth texture
GLuint createADFBO(glm::uvec2 resolution, GLuint& colourTexture, GLuint& depthTexture) {
    GLuint FBO;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    //Colour
    glGenTextures(1, &colourTexture);
    glBindTexture(GL_TEXTURE_2D, colourTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, resolution.x, resolution.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colourTexture, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //Depth
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, resolution.x, resolution.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

    GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, drawBuffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        raise("Colour FBO [W/ depth] incomplete!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return FBO;
}
//// FRAMEBUFFERS ////


//// ATOMIC ////
static GLuint zero = 0u;
void fetchAndClearAtomic(GLuint atomicCounter, GLuint* counterValue) {
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicCounter);
	if (counterValue) {
		glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), counterValue);	
	}
	glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &zero);
}
GLuint createAtomicCounter(unsigned int binding) {
	GLuint atomicCounter;
	glGenBuffers(1, &atomicCounter);
	fetchAndClearAtomic(atomicCounter, nullptr);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, binding, atomicCounter);
	return atomicCounter;
}
//// ATOMIC ////




void prepareOpenGL() {
	//OpenGL setup;
	glViewport(0, 0, display::RENDER_RESOLUTION.x, display::RENDER_RESOLUTION.y);
#ifdef SCREENSPACE_ONLY
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
#endif
	GLIndex::genericVAO = getEmptyVAO();	

	//Debug settings
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(openGLErrorCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

	utils::GLErrorcheck("Initialisation", true); //Old basic debugging
}

}



namespace frame {


inline void renderingGeneric(const std::string& shaderName="") {
	glBindVertexArray(GLIndex::genericVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	if (!shaderName.empty()) {
		utils::GLErrorcheck(shaderName, true);
	}
}



void draw() {
	
}



}