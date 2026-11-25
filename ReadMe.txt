HalfLifeClone/
│
├── CMakeLists.txt
│
├── external/
│   ├── glfw/
│   ├── glad/
│   └── glm/
│
├── assets/
│   ├── models/
│   ├── textures/
│   ├── sounds/
│   └── shaders/
│
├── src/
│   ├── main.cpp
│   │
│   ├── Engine/
│   │   ├── Window.cpp
│   │   ├── Renderer.cpp
│   │   └── Input.cpp
│   │
│   ├── Graphics/
│   │   ├── Shader.cpp
│   │   ├── Camera.cpp
│   │   ├── Mesh.cpp
│   │   └── Model.cpp
│   │
│   ├── Player/
│   │   └── Player.cpp
│   │
│   ├── Weapon/
│   │   └── Weapon.cpp
│   │
│   ├── Enemy/
│   │   └── Enemy.cpp
│   │
│   └── Game/
│       └── Game.cpp
│
└── build/
=====================================================================================================================
Tuyệt vời! Chúng ta sẽ tự xây dựng một game FPS 3D tương tự **Half-Life 1 bằng C++ và OpenGL**, chạy native trên **Ubuntu 24.04.5 LTS**.

Đây là hướng phát triển khá thú vị vì chúng ta không phụ thuộc vào Unreal Engine hay Unity. Bạn sẽ hiểu cách một game engine hoạt động bên trong.

Tôi sẽ đồng hành cùng bạn từ việc thiết lập môi trường, viết code đến khi có một game FPS hoàn chỉnh có thể chạy trên Ubuntu.

# 1. Công nghệ sử dụng

Tôi đề xuất bộ công nghệ sau:

| Thành phần | Công nghệ | Mục đích |
|---|---|---|
| Ngôn ngữ | C++17 | Lập trình game |
| Graphics API | OpenGL 3.3 Core | Render đồ họa 3D |
| Window | GLFW | Tạo cửa sổ game |
| OpenGL Loader | GLAD | Nạp các hàm OpenGL |
| Math | GLM | Vector, Matrix, Camera |
| Model Loading | Assimp | Đọc model 3D |
| Texture | stb_image | Đọc hình ảnh |
| Audio | OpenAL Soft | Âm thanh |
| Build | CMake | Biên dịch project |

**Tại sao chọn OpenGL 3.3?**

OpenGL 3.3 Core đủ để xây dựng một game FPS 3D với:
- 3D rendering.
- Texture và lighting.
- Shadow mapping.
- Skeletal animation.
- Particle effects.
- Post-processing.

Đây cũng là phiên bản phù hợp để học lập trình đồ họa 3D.

# 2. Kiến trúc project

Chúng ta sẽ tổ chức project theo hướng một game engine nhỏ.

```text
HalfLifeClone/
│
├── CMakeLists.txt
│
├── external/
│   ├── glfw/
│   ├── glad/
│   └── glm/
│
├── assets/
│   ├── models/
│   ├── textures/
│   ├── sounds/
│   └── shaders/
│
├── src/
│   ├── main.cpp
│   │
│   ├── Engine/
│   │   ├── Window.cpp
│   │   ├── Renderer.cpp
│   │   └── Input.cpp
│   │
│   ├── Graphics/
│   │   ├── Shader.cpp
│   │   ├── Camera.cpp
│   │   ├── Mesh.cpp
│   │   └── Model.cpp
│   │
│   ├── Player/
│   │   └── Player.cpp
│   │
│   ├── Weapon/
│   │   └── Weapon.cpp
│   │
│   ├── Enemy/
│   │   └── Enemy.cpp
│   │
│   └── Game/
│       └── Game.cpp
│
└── build/
```

# 3. Cài đặt môi trường trên Ubuntu

Mở Terminal và cài đặt các thư viện:

```bash
sudo apt update

sudo apt install -y \
    build-essential \
    cmake \
    git \
    pkg-config \
    libgl1-mesa-dev \
    libglfw3-dev \
    libglew-dev \
    libglm-dev \
    mesa-utils
```

Trong giai đoạn đầu, chúng ta sử dụng **GLEW thay cho GLAD** vì Ubuntu cung cấp sẵn package, giúp việc thiết lập đơn giản hơn.

Kiểm tra OpenGL:

```bash
glxinfo | grep "OpenGL version"
```

Nếu chưa có lệnh `glxinfo`, hãy cài:

```bash
sudo apt install mesa-utils
```

Lệnh trên sẽ cho biết phiên bản OpenGL mà hệ thống đang hỗ trợ.

# 4. Bắt đầu xây dựng game – Milestone 1

Mục tiêu đầu tiên:

- Tạo cửa sổ game.
- Khởi tạo OpenGL.
- Hiển thị màn hình 3D.
- Xử lý vòng lặp game (Game Loop).
- Nhấn ESC để thoát.

Đây là nền tảng của toàn bộ game.

### Bước 1: Tạo project

```bash
mkdir -p ~/Projects/HalfLifeClone/src
cd ~/Projects/HalfLifeClone
```

### Bước 2: Tạo CMakeLists.txt

Tạo file `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.16)

project(HalfLifeClone LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

add_executable(HalfLifeClone
    src/main.cpp
)

target_link_libraries(HalfLifeClone
    PRIVATE
    glfw
    OpenGL::GL
    GLEW::GLEW
)

```

### Bước 3: Viết chương trình main.cpp

Tạo file `src/main.cpp`:

```cpp
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

const unsigned int WIDTH = 1280;
const unsigned int HEIGHT = 720;

int main()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    // Configure OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE
    );

    // Create window
    GLFWwindow* window = glfwCreateWindow(
        WIDTH,
        HEIGHT,
        "HalfLife Clone",
        nullptr,
        nullptr
    );

    if (!window)
    {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Print OpenGL version
    std::cout << "OpenGL Version: "
              << glGetString(GL_VERSION)
              << std::endl;

    // Main Game Loop
    while (!glfwWindowShouldClose(window))
    {
        // Input
        if (glfwGetKey(window, GLFW_KEY_ESCAPE)
            == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }

        // Rendering
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT |
                GL_DEPTH_BUFFER_BIT);

        // Update screen
        glfwSwapBuffers(window);

        // Process events
        glfwPollEvents();
    }

    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
```

### Bước 4: Build chương trình

```bash
cmake -S . -B build

cmake --build build -j$(nproc)
```

### Bước 5: Chạy game

```bash
./build/HalfLifeClone
```

Nếu thành công, bạn sẽ thấy một cửa sổ 1280 × 720 với nền màu xanh đen.

Đây chính là cửa sổ game đầu tiên của chúng ta!

# 5. Lộ trình phát triển tiếp theo

Sau khi hoàn thành Milestone 1, chúng ta sẽ phát triển theo thứ tự:

| Milestone | Nội dung |
|---|---|
| 1 | GLFW, OpenGL initialization, Game Loop |
| 2 | Render hình khối 3D, Shader, VAO, VBO |
| 3 | Camera 3D, góc nhìn FPS |
| 4 | Điều khiển Player bằng WASD và chuột |
| 5 | Collision Detection, Gravity, Jump |
| 6 | Xây dựng Map 3D |
| 7 | Texture, Lighting, Shadow |
| 8 | Súng và cơ chế bắn |
| 9 | Enemy và AI |
| 10 | Health, HUD, Sound và hoàn thiện game |

**Một nguyên tắc quan trọng:** Chúng ta sẽ không vội viết tất cả vào `main.cpp`. Tôi muốn xây dựng kiến trúc hướng đối tượng, tách biệt Engine, Renderer, Player, Weapon và Enemy.

Điều này giúp project dễ bảo trì và mở rộng khi game lớn dần.

---

### Bắt đầu nhé!

Bạn hãy thực hiện Milestone 1 trước.

Sau đó gửi tôi:
1. Kết quả chạy chương trình.
2. Phiên bản OpenGL hiển thị trong Terminal.

Tiếp theo, chúng ta sẽ xây dựng **3D Renderer và Shader bằng GLSL**, bắt đầu đưa những hình khối 3D đầu tiên vào game.
============================================================================================================================
Tốt! Như vậy chúng ta đã hoàn thành **Milestone 1 – Khởi tạo OpenGL Window và Game Loop**.

Bây giờ bắt đầu phần thú vị hơn: đưa các vật thể 3D vào game.

# Milestone 2 – 3D Rendering

Mục tiêu của giai đoạn này:

1. Hiểu cách OpenGL hiển thị vật thể 3D.
2. Tạo một hình tam giác bằng OpenGL.
3. Tìm hiểu VAO, VBO và Vertex Attributes.
4. Viết Vertex Shader và Fragment Shader bằng GLSL.
5. Render một hình lập phương 3D (3D Cube).

Đây là nền tảng để sau này xây dựng bản đồ, súng và nhân vật trong game.

## 1. Hiểu kiến trúc Rendering Pipeline

Khi game hiển thị một vật thể 3D, dữ liệu sẽ đi qua các bước chính:



Pipeline cơ bản:

```text
       CPU (C++)
          |
          v
     Vertex Data
          |
          v
      VBO / VAO
          |
          v
     Vertex Shader
          |
          v
    Primitive Assembly
          |
          v
    Rasterization
          |
          v
    Fragment Shader
          |
          v
      Framebuffer
          |
          v
       Display
```

Giải thích các thành phần:

| Thành phần | Chức năng |
|---|---|
| Vertex Data | Dữ liệu tọa độ các đỉnh |
| VBO | Lưu trữ dữ liệu Vertex trên GPU |
| VAO | Lưu cấu hình Vertex Attributes |
| Vertex Shader | Xử lý tọa độ các đỉnh |
| Rasterization | Chuyển tam giác thành các Fragment |
| Fragment Shader | Xác định màu sắc từng Fragment |
| Framebuffer | Nơi lưu kết quả render |

Một điều quan trọng: **OpenGL không tự biết cách vẽ hình tam giác.** Chúng ta phải cung cấp dữ liệu và viết Shader để GPU biết cách xử lý chúng.

## 2. Tạo cấu trúc thư mục

Trong thư mục project:

```bash
cd ~/Projects/HalfLifeClone

mkdir -p assets/shaders
```

Cấu trúc mới:

```text
HalfLifeClone/
│
├── CMakeLists.txt
├── src/
│   └── main.cpp
│
├── assets/
│   └── shaders/
│       ├── vertex.glsl
│       └── fragment.glsl
│
└── build/
```

## 3. Viết Vertex Shader

Tạo file:

`assets/shaders/vertex.glsl`

```glsl
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vertexColor;

void main()
{
    gl_Position = vec4(aPos, 1.0);

    vertexColor = aColor;
}
```

Giải thích:

- `aPos`: Tọa độ của Vertex.
- `aColor`: Màu sắc của Vertex.
- `gl_Position`: Tọa độ đầu ra của Vertex Shader.
- `vertexColor`: Truyền màu sang Fragment Shader.

Ở đây chúng ta sử dụng tọa độ NDC (Normalized Device Coordinates), có giá trị từ -1 đến 1.

## 4. Viết Fragment Shader

Tạo file:

`assets/shaders/fragment.glsl`

```glsl
#version 330 core

in vec3 vertexColor;

out vec4 FragColor;

void main()
{
    FragColor = vec4(vertexColor, 1.0);
}
```

Fragment Shader quyết định màu sắc của từng pixel được rasterize từ tam giác.

## 5. Cập nhật CMakeLists.txt

Thêm dòng sau vào cuối file:

```cmake
target_compile_definitions(HalfLifeClone PRIVATE
    ASSET_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets"
)
```

Dòng này giúp chương trình tìm được thư mục Shader ngay cả khi executable nằm trong thư mục `build`.

## 6. Viết chương trình Render Triangle

Thay toàn bộ nội dung `src/main.cpp` bằng code sau:

```cpp
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifndef ASSET_DIR
#define ASSET_DIR "assets"
#endif

const unsigned int WIDTH = 1280;
const unsigned int HEIGHT = 720;

// Read shader file
std::string loadShader(const std::string& path)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open shader: " + path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

// Compile shader
GLuint compileShader(
    GLenum type,
    const std::string& source)
{
    GLuint shader = glCreateShader(type);

    const char* sourcePtr = source.c_str();

    glShaderSource(shader, 1, &sourcePtr, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char infoLog[512];

        glGetShaderInfoLog(
            shader,
            512,
            nullptr,
            infoLog
        );

        std::cerr << "Shader compilation failed:\n"
                  << infoLog << std::endl;

        glDeleteShader(shader);

        return 0;
    }

    return shader;
}

// Create shader program
GLuint createShaderProgram()
{
    std::string vertexSource = loadShader(
        std::string(ASSET_DIR) + "/shaders/vertex.glsl"
    );

    std::string fragmentSource = loadShader(
        std::string(ASSET_DIR) + "/shaders/fragment.glsl"
    );

    GLuint vertexShader = compileShader(
        GL_VERTEX_SHADER,
        vertexSource
    );

    GLuint fragmentShader = compileShader(
        GL_FRAGMENT_SHADER,
        fragmentSource
    );

    if (!vertexShader || !fragmentShader)
    {
        if (vertexShader)
            glDeleteShader(vertexShader);

        if (fragmentShader)
            glDeleteShader(fragmentShader);

        return 0;
    }

    GLuint program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);

    GLint success;

    glGetProgramiv(
        program,
        GL_LINK_STATUS,
        &success
    );

    if (!success)
    {
        char infoLog[512];

        glGetProgramInfoLog(
            program,
            512,
            nullptr,
            infoLog
        );

        std::cerr << "Shader linking failed:\n"
                  << infoLog << std::endl;

        glDeleteProgram(program);
        program = 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

int main()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "GLFW initialization failed\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE
    );

    GLFWwindow* window = glfwCreateWindow(
        WIDTH,
        HEIGHT,
        "HalfLife Clone - Milestone 2",
        nullptr,
        nullptr
    );

    if (!window)
    {
        std::cerr << "Window creation failed\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "GLEW initialization failed\n";

        glfwDestroyWindow(window);
        glfwTerminate();

        return -1;
    }

    glViewport(0, 0, WIDTH, HEIGHT);

    std::cout << "OpenGL Version: "
              << glGetString(GL_VERSION)
              << std::endl;

    GLuint shaderProgram = 0;
    GLuint VAO = 0;
    GLuint VBO = 0;

    try
    {
        shaderProgram = createShaderProgram();

        if (!shaderProgram)
        {
            throw std::runtime_error(
                "Cannot create shader program"
            );
        }

        // Vertex data
        float vertices[] =
        {
            // Position           // Color
            -0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,
             0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,
             0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f
        };

        // Create VAO and VBO
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(vertices),
            vertices,
            GL_STATIC_DRAW
        );

        // Position attribute
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            6 * sizeof(float),
            (void*)0
        );

        glEnableVertexAttribArray(0);

        // Color attribute
        glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            6 * sizeof(float),
            (void*)(3 * sizeof(float))
        );

        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;

        if (VAO)
            glDeleteVertexArrays(1, &VAO);

        if (VBO)
            glDeleteBuffers(1, &VBO);

        if (shaderProgram)
            glDeleteProgram(shaderProgram);

        glfwDestroyWindow(window);
        glfwTerminate();

        return -1;
    }

    // Game Loop
    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }

        // Clear screen
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render triangle
        glUseProgram(shaderProgram);

        glBindVertexArray(VAO);

        glDrawArrays(
            GL_TRIANGLES,
            0,
            3
        );

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
```

## 7. Build và chạy

Chạy tại thư mục gốc của project:

```bash
cmake -S . -B build

cmake --build build -j$(nproc)

./build/HalfLifeClone
```

Kết quả mong đợi:

- Một cửa sổ 1280 × 720.
- Nền màu xanh đen.
- Một hình tam giác ở giữa màn hình.
- Ba đỉnh có màu đỏ, xanh lá và xanh dương.
- Màu chuyển tiếp giữa các đỉnh.

## 8. Những kiến thức C++ và OpenGL cần chú ý

Trong đoạn code trên, tôi muốn bạn chú ý 4 API quan trọng:

| API | Ý nghĩa |
|---|---|
| `glGenBuffers()` | Tạo VBO |
| `glBufferData()` | Đưa dữ liệu Vertex lên GPU |
| `glVertexAttribPointer()` | Mô tả cấu trúc dữ liệu Vertex |
| `glDrawArrays()` | Yêu cầu GPU vẽ hình |

Đặc biệt, hãy chú ý:

```cpp
glDrawArrays(GL_TRIANGLES, 0, 3);
```

Lệnh này yêu cầu OpenGL vẽ một hình tam giác từ 3 Vertex đầu tiên.

**Lưu ý nhỏ:** Thêm `#include <stdexcept>` vào đầu `main.cpp` để sử dụng `std::runtime_error`.

---

### 9. Tiếp theo chúng ta sẽ làm gì?

Sau khi bạn chạy thành công hình tam giác, chúng ta sẽ chuyển sang:

**Milestone 2.2 – Render 3D Cube**

Chúng ta sẽ học:
- 3D Coordinates.
- Model, View, Projection Matrix.
- Perspective Projection.
- Depth Testing.
- GLM (OpenGL Mathematics).

Kết quả sẽ là một khối lập phương 3D có thể xoay trong không gian.

Đây là bước rất quan trọng trước khi xây dựng camera góc nhìn thứ nhất giống Half-Life.

Bạn hãy chạy thử và cho tôi biết hình tam giác đã xuất hiện chưa nhé!
==================================================================================================================================
Rất tốt! 🎮 Vậy là chúng ta đã có thể render hình ảnh bằng OpenGL.

Bây giờ chúng ta chuyển sang **Milestone 2.2 – Render vật thể 3D (3D Cube)**.

Đây là một bước rất quan trọng. Chúng ta sẽ xây dựng nền tảng đồ họa 3D để sau này có thể tạo:
- Các bức tường và hành lang.
- Súng 3D.
- Nhân vật.
- Bản đồ giống Half-Life.

# 1. Mục tiêu

Thay vì một tam giác 2D, chúng ta sẽ render một khối lập phương 3D.



Khối lập phương sẽ:
- Có 6 mặt.
- Có 8 đỉnh hình học.
- Có 36 Vertex khi sử dụng GL_TRIANGLES.
- Có màu sắc khác nhau giữa các mặt.
- Tự động xoay theo thời gian.

# 2. Tìm hiểu Model, View, Projection

Đây là ba khái niệm quan trọng nhất trong lập trình đồ họa 3D.



**Model Matrix**

Xác định vị trí, góc xoay và kích thước của vật thể trong thế giới 3D.

**View Matrix**

Xác định vị trí và hướng nhìn của camera.

**Projection Matrix**

Chuyển không gian 3D sang không gian 2D để hiển thị lên màn hình.

Ba ma trận này được sử dụng trong Vertex Shader để biến đổi tọa độ Vertex.

# 3. Sử dụng GLM

Chúng ta đã cài `libglm-dev` ở Milestone 1.

GLM cung cấp các kiểu dữ liệu và phép toán toán học thường dùng trong OpenGL.

Ví dụ:

```cpp
glm::vec3 position(0.0f, 0.0f, 0.0f);

glm::mat4 model(1.0f);

model = glm::rotate(
    model,
    glm::radians(45.0f),
    glm::vec3(0.0f, 1.0f, 0.0f)
);
```

Một số kiểu dữ liệu quan trọng:

| Kiểu dữ liệu | Ý nghĩa |
|---|---|
| `glm::vec2` | Vector 2 chiều |
| `glm::vec3` | Vector 3 chiều |
| `glm::vec4` | Vector 4 chiều |
| `glm::mat3` | Ma trận 3×3 |
| `glm::mat4` | Ma trận 4×4 |

# 4. Cập nhật Vertex Shader

Mở file:

`assets/shaders/vertex.glsl`

Thay bằng:

```glsl
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model
                  * vec4(aPos, 1.0);

    vertexColor = aColor;
}
```

Điểm khác biệt so với Shader trước:

```glsl
gl_Position = projection * view * model
              * vec4(aPos, 1.0);
```

Đây chính là phép biến đổi tọa độ từ Local Space sang Clip Space.

Lưu ý: Trong GLSL, thứ tự nhân ma trận rất quan trọng.

# 5. Fragment Shader

File `assets/shaders/fragment.glsl` giữ nguyên:

```glsl
#version 330 core

in vec3 vertexColor;

out vec4 FragColor;

void main()
{
    FragColor = vec4(vertexColor, 1.0);
}
```

# 6. Cập nhật main.cpp

Đây là phần chính.

Chúng ta sử dụng:
- `glm::translate()` để di chuyển Cube.
- `glm::rotate()` để xoay Cube.
- `glm::lookAt()` để tạo View Matrix.
- `glm::perspective()` để tạo Perspective Projection.

Thay toàn bộ `src/main.cpp` bằng:

```cpp
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef ASSET_DIR
#define ASSET_DIR "assets"
#endif

const unsigned int WIDTH = 1280;
const unsigned int HEIGHT = 720;

// =====================================================
// Shader utilities
// =====================================================

std::string loadShader(const std::string& path)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        throw std::runtime_error(
            "Cannot open shader: " + path
        );
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

GLuint compileShader(
    GLenum type,
    const std::string& source)
{
    GLuint shader = glCreateShader(type);

    const char* sourcePtr = source.c_str();

    glShaderSource(shader, 1, &sourcePtr, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char infoLog[512];

        glGetShaderInfoLog(
            shader, 512, nullptr, infoLog
        );

        std::cerr << infoLog << std::endl;

        glDeleteShader(shader);

        return 0;
    }

    return shader;
}

GLuint createShaderProgram()
{
    std::string vertexSource = loadShader(
        std::string(ASSET_DIR) + "/shaders/vertex.glsl"
    );

    std::string fragmentSource = loadShader(
        std::string(ASSET_DIR) + "/shaders/fragment.glsl"
    );

    GLuint vertexShader = compileShader(
        GL_VERTEX_SHADER, vertexSource
    );

    GLuint fragmentShader = compileShader(
        GL_FRAGMENT_SHADER, fragmentSource
    );

    if (!vertexShader || !fragmentShader)
    {
        if (vertexShader)
            glDeleteShader(vertexShader);

        if (fragmentShader)
            glDeleteShader(fragmentShader);

        return 0;
    }

    GLuint program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);

    GLint success;

    glGetProgramiv(
        program, GL_LINK_STATUS, &success
    );

    if (!success)
    {
        char infoLog[512];

        glGetProgramInfoLog(
            program, 512, nullptr, infoLog
        );

        std::cerr << "Shader linking error: "
                  << infoLog << std::endl;

        glDeleteProgram(program);
        program = 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

// =====================================================
// Main
// =====================================================

int main()
{
    if (!glfwInit())
    {
        std::cerr << "GLFW initialization failed\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE
    );

    GLFWwindow* window = glfwCreateWindow(
        WIDTH,
        HEIGHT,
        "HalfLife Clone - 3D Cube",
        nullptr,
        nullptr
    );

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "GLEW initialization failed\n";

        glfwDestroyWindow(window);
        glfwTerminate();

        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    GLuint shaderProgram = 0;
    GLuint VAO = 0;
    GLuint VBO = 0;

    try
    {
        shaderProgram = createShaderProgram();

        if (!shaderProgram)
        {
            throw std::runtime_error(
                "Shader program creation failed"
            );
        }

        // =================================================
        // Cube vertices
        // Position (x,y,z) + Color (r,g,b)
        // 36 vertices = 12 triangles = 6 faces
        // =================================================

        float vertices[] =
        {
            // Front face - Red
            -0.5f,-0.5f, 0.5f, 1,0,0,
             0.5f,-0.5f, 0.5f, 1,0,0,
             0.5f, 0.5f, 0.5f, 1,0,0,

             0.5f, 0.5f, 0.5f, 1,0,0,
            -0.5f, 0.5f, 0.5f, 1,0,0,
            -0.5f,-0.5f, 0.5f, 1,0,0,

            // Back face - Green
            -0.5f,-0.5f,-0.5f, 0,1,0,
            -0.5f, 0.5f,-0.5f, 0,1,0,
             0.5f, 0.5f,-0.5f, 0,1,0,

             0.5f, 0.5f,-0.5f, 0,1,0,
             0.5f,-0.5f,-0.5f, 0,1,0,
            -0.5f,-0.5f,-0.5f, 0,1,0,

            // Left face - Blue
            -0.5f,-0.5f,-0.5f, 0,0,1,
            -0.5f,-0.5f, 0.5f, 0,0,1,
            -0.5f, 0.5f, 0.5f, 0,0,1,

            -0.5f, 0.5f, 0.5f, 0,0,1,
            -0.5f, 0.5f,-0.5f, 0,0,1,
            -0.5f,-0.5f,-0.5f, 0,0,1,

            // Right face - Yellow
             0.5f,-0.5f,-0.5f, 1,1,0,
             0.5f, 0.5f,-0.5f, 1,1,0,
             0.5f, 0.5f, 0.5f, 1,1,0,

             0.5f, 0.5f, 0.5f, 1,1,0,
             0.5f,-0.5f, 0.5f, 1,1,0,
             0.5f,-0.5f,-0.5f, 1,1,0,

            // Top face - Cyan
            -0.5f, 0.5f,-0.5f, 0,1,1,
            -0.5f, 0.5f, 0.5f, 0,1,1,
             0.5f, 0.5f, 0.5f, 0,1,1,

             0.5f, 0.5f, 0.5f, 0,1,1,
             0.5f, 0.5f,-0.5f, 0,1,1,
            -0.5f, 0.5f,-0.5f, 0,1,1,

            // Bottom face - Magenta
            -0.5f,-0.5f,-0.5f, 1,0,1,
             0.5f,-0.5f,-0.5f, 1,0,1,
             0.5f,-0.5f, 0.5f, 1,0,1,

             0.5f,-0.5f, 0.5f, 1,0,1,
            -0.5f,-0.5f, 0.5f, 1,0,1,
            -0.5f,-0.5f,-0.5f, 1,0,1
        };

        // Create VAO and VBO
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(vertices),
            vertices,
            GL_STATIC_DRAW
        );

        // Position attribute
        glVertexAttribPointer(
            0, 3, GL_FLOAT, GL_FALSE,
            6 * sizeof(float), (void*)0
        );

        glEnableVertexAttribArray(0);

        // Color attribute
        glVertexAttribPointer(
            1, 3, GL_FLOAT, GL_FALSE,
            6 * sizeof(float),
            (void*)(3 * sizeof(float))
        );

        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Initialization error: "
                  << e.what() << std::endl;

        if (VAO)
            glDeleteVertexArrays(1, &VAO);

        if (VBO)
            glDeleteBuffers(1, &VBO);

        if (shaderProgram)
            glDeleteProgram(shaderProgram);

        glfwDestroyWindow(window);
        glfwTerminate();

        return -1;
    }

    // =================================================
    // Game Loop
    // =================================================

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE)
            == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }

        float currentTime = glfwGetTime();

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);

        glClear(
            GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT
        );

        // Model Matrix
        glm::mat4 model(1.0f);

        model = glm::rotate(
            model,
            currentTime,
            glm::vec3(0.5f, 1.0f, 0.0f)
        );

        // View Matrix
        glm::mat4 view = glm::lookAt(
            glm::vec3(2.0f, 2.0f, 3.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        // Projection Matrix
        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            (float)WIDTH / (float)HEIGHT,
            0.1f,
            100.0f
        );

        // Send matrices to GPU
        glUseProgram(shaderProgram);

        glUniformMatrix4fv(
            glGetUniformLocation(shaderProgram, "model"),
            1, GL_FALSE, glm::value_ptr(model)
        );

        glUniformMatrix4fv(
            glGetUniformLocation(shaderProgram, "view"),
            1, GL_FALSE, glm::value_ptr(view)
        );

        glUniformMatrix4fv(
            glGetUniformLocation(shaderProgram, "projection"),
            1, GL_FALSE, glm::value_ptr(projection)
        );

        // Draw Cube
        glBindVertexArray(VAO);

        glDrawArrays(
            GL_TRIANGLES,
            0,
            36
        );

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
```

# 7. Build và chạy

Không cần thay đổi `CMakeLists.txt` vì chúng ta đã liên kết GLM thông qua header-only library.

Chạy:

```bash
cd ~/Projects/HalfLifeClone

cmake --build build -j$(nproc)

./build/HalfLifeClone
```

Kết quả mong đợi là một khối lập phương 3D có 6 mặt màu sắc khác nhau, tự động xoay quanh tâm.

# 8. Tôi muốn bạn hiểu kỹ 3 phần quan trọng

### A. Depth Testing

```cpp
glEnable(GL_DEPTH_TEST);
```

Tại sao cần lệnh này?

Hãy tưởng tượng Cube có hai mặt:
- Mặt phía trước.
- Mặt phía sau.

Nếu không bật Depth Testing, GPU có thể vẽ mặt phía sau đè lên mặt phía trước.

Depth Testing giúp OpenGL xác định pixel nào gần camera hơn để hiển thị chính xác.

### B. Perspective Projection

```cpp
glm::mat4 projection = glm::perspective(
    glm::radians(45.0f),
    (float)WIDTH / (float)HEIGHT,
    0.1f,
    100.0f
);
```

Các tham số:
- `45.0f`: Góc nhìn FOV.
- `WIDTH / HEIGHT`: Tỷ lệ khung hình.
- `0.1f`: Khoảng cách gần nhất camera nhìn thấy.
- `100.0f`: Khoảng cách xa nhất camera nhìn thấy.

### C. Game Loop

```cpp
while (!glfwWindowShouldClose(window))
{
    // Input

    // Update

    // Rendering

    // Swap buffers
}
```

Đây là trái tim của game engine.

Sau này chúng ta sẽ bổ sung:
- Input processing.
- Physics update.
- Player update.
- Enemy AI update.
- Rendering.

---

# 9. Milestone tiếp theo: FPS Camera

Sau khi Cube chạy thành công, chúng ta sẽ chuyển sang một phần cực kỳ quan trọng:

**Milestone 3 – First Person Camera**

Mục tiêu:

1. Di chuyển camera bằng chuột.
2. WASD để di chuyển.
3. Mouse movement để xoay góc nhìn.
4. Xử lý Pitch và Yaw.
5. Tạo nhân vật có thể di chuyển trong không gian 3D.

Kết quả mong muốn: Bạn có thể nhìn xung quanh và di chuyển trong thế giới 3D giống như Half-Life.

**Hãy chạy thử Cube trước nhé.** Khi đã thấy khối lập phương 3D xoay được, chúng ta sẽ bắt đầu xây dựng FPS Camera.
==============================================================================================================================
Tuyệt vời! 🎮 Vậy chúng ta đã hoàn thành **Milestone 2.2 – Rotating 3D Cube**.

Tiếp theo là **Milestone 3 – FPS Camera**. Đây là bước rất quan trọng để biến chương trình OpenGL thành một game góc nhìn thứ nhất giống Half-Life.

## Milestone 3: Xây dựng FPS Camera

Chúng ta sẽ bổ sung các tính năng:

1. **W, A, S, D:** Di chuyển camera tiến, lùi, trái, phải.
2. **Mouse:** Xoay góc nhìn bằng chuột.
3. **ESC:** Thoát game.
4. **Mouse Capture:** Ẩn và khóa con trỏ chuột trong cửa sổ game.

Sau bước này, bạn có thể di chuyển xung quanh khối lập phương bằng bàn phím và chuột.

### 1. Tạo file `src/Camera.h`

Đầu tiên, chúng ta tạo một class `Camera` để quản lý vị trí và hướng nhìn.

```cpp
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum CameraMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Camera
{
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;

    float MovementSpeed;
    float MouseSensitivity;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f))
    {
        Position = position;

        WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

        Yaw = -90.0f;
        Pitch = 0.0f;

        MovementSpeed = 3.0f;
        MouseSensitivity = 0.1f;

        updateCameraVectors();
    }

    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(
            Position,
            Position + Front,
            Up
        );
    }

    void ProcessKeyboard(CameraMovement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;

        if (direction == FORWARD)
            Position += Front * velocity;

        if (direction == BACKWARD)
            Position -= Front * velocity;

        if (direction == LEFT)
            Position -= Right * velocity;

        if (direction == RIGHT)
            Position += Right * velocity;
    }

    void ProcessMouseMovement(float xOffset, float yOffset)
    {
        xOffset *= MouseSensitivity;
        yOffset *= MouseSensitivity;

        Yaw += xOffset;
        Pitch += yOffset;

        // Prevent camera from flipping upside down
        if (Pitch > 89.0f)
            Pitch = 89.0f;

        if (Pitch < -89.0f)
            Pitch = -89.0f;

        updateCameraVectors();
    }

private:
    void updateCameraVectors()
    {
        glm::vec3 front;

        front.x = cos(glm::radians(Yaw)) *
                  cos(glm::radians(Pitch));

        front.y = sin(glm::radians(Pitch));

        front.z = sin(glm::radians(Yaw)) *
                  cos(glm::radians(Pitch));

        Front = glm::normalize(front);

        Right = glm::normalize(glm::cross(Front, WorldUp));

        Up = glm::normalize(glm::cross(Right, Front));
    }
};
```

### 2. Giải thích cách Camera hoạt động

Camera có ba thông tin quan trọng:

| Thành phần | Ý nghĩa |
|---|---|
| `Position` | Vị trí camera trong không gian 3D |
| `Front` | Hướng camera đang nhìn |
| `Up` | Hướng lên trên của camera |

Ngoài ra, chúng ta sử dụng hai góc:

- `Yaw`: Xoay camera sang trái hoặc phải.
- `Pitch`: Xoay camera lên trên hoặc xuống dưới.

Ví dụ:

```cpp
Yaw = -90.0f;
Pitch = 0.0f;
```

Camera ban đầu nhìn theo hướng trục Z âm.

Hàm quan trọng nhất là:

```cpp
glm::mat4 GetViewMatrix()
{
    return glm::lookAt(
        Position,
        Position + Front,
        Up
    );
}
```

Hàm `glm::lookAt()` tạo View Matrix dựa trên vị trí và hướng nhìn của camera.

---

### 3. Cập nhật `main.cpp`

**Bước 1: Thêm các biến và khai báo**

Ở đầu file `main.cpp`, thêm:

```cpp
#include "Camera.h"

Camera camera;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float lastX = 640.0f;
float lastY = 360.0f;

bool firstMouse = true;
```

**Bước 2: Xử lý chuột**

Thêm callback sau vào `main.cpp`:

```cpp
void mouse_callback(GLFWwindow* window,
                    double xpos,
                    double ypos)
{
    if (firstMouse)
    {
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);

        firstMouse = false;
    }

    float xOffset = static_cast<float>(xpos) - lastX;
    float yOffset = lastY - static_cast<float>(ypos);

    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);

    camera.ProcessMouseMovement(xOffset, yOffset);
}
```

Lưu ý:

```cpp
float yOffset = lastY - static_cast<float>(ypos);
```

Chúng ta đảo chiều phép trừ vì tọa độ Y của chuột trong GLFW tăng từ trên xuống dưới, trong khi trục Y trong hệ tọa độ OpenGL tăng từ dưới lên trên.

**Bước 3: Xử lý bàn phím**

Thêm hàm:

```cpp
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}
```

**Bước 4: Cấu hình chuột sau khi tạo GLFW window**

Sau khi tạo window thành công và trước game loop, thêm:

```cpp
glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

glfwSetCursorPosCallback(window, mouse_callback);
```

`GLFW_CURSOR_DISABLED` sẽ ẩn và khóa con trỏ chuột trong cửa sổ game.

**Bước 5: Cập nhật Game Loop**

Trong vòng lặp `while`, trước khi xử lý input, thêm:

```cpp
float currentFrame = glfwGetTime();

deltaTime = currentFrame - lastFrame;
lastFrame = currentFrame;

processInput(window);
```

Sau đó, tìm đoạn code tạo View Matrix cũ:

```cpp
glm::mat4 view = glm::lookAt(
    glm::vec3(2.0f, 2.0f, 3.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f)
);
```

Thay bằng:

```cpp
glm::mat4 view = camera.GetViewMatrix();
```

Các phần còn lại như Model Matrix, Projection Matrix và `glDrawArrays()` giữ nguyên.

---

### 4. Build và chạy chương trình

```bash
cd ~/Projects/HalfLifeClone

cmake --build build -j$(nproc)

./build/HalfLifeClone
```

### 5. Kết quả mong đợi

Khi chạy chương trình:

| Phím / thao tác | Chức năng |
|---|---|
| W | Di chuyển về phía trước |
| S | Di chuyển về phía sau |
| A | Di chuyển sang trái |
| D | Di chuyển sang phải |
| Mouse Left/Right | Xoay camera trái/phải |
| Mouse Up/Down | Nhìn lên/xuống |
| ESC | Thoát game |

### 6. Chúng ta đang xây dựng những gì?

Đến đây, kiến trúc chương trình đã có:

```text
HalfLifeClone/
│
├── CMakeLists.txt
│
├── src/
│   ├── main.cpp
│   └── Camera.h
│
├── assets/
│   └── shaders/
│       ├── vertex.glsl
│       └── fragment.glsl
│
└── build/
```

Một điểm quan trọng: chúng ta sử dụng `deltaTime` để tính khoảng cách di chuyển.

Ví dụ:

```cpp
float velocity = MovementSpeed * deltaTime;
```

Nhờ đó, tốc độ di chuyển không phụ thuộc vào FPS. Máy chạy 30 FPS hay 144 FPS thì camera vẫn di chuyển với tốc độ gần như nhau.

**Sau khi hoàn thành Milestone 3**, bước tiếp theo sẽ là Milestone 4: tạo môi trường 3D (Floor, Walls, Map) để chúng ta có thể đi lại trong một căn phòng giống game FPS thực sự.
===================================================================================================================================
Tiếp tục nhé! Bây giờ chúng ta sẽ chuyển từ một khối lập phương đơn lẻ sang một **môi trường 3D giống một căn phòng trong game Half-Life**.

# Milestone 4 – Xây dựng môi trường 3D (3D Environment)

Mục tiêu của bước này:

1. Tạo mặt sàn (Floor).
2. Tạo 4 bức tường (Walls).
3. Giữ lại khối lập phương ở giữa phòng.
4. Có thể di chuyển camera để quan sát môi trường 3D.

Chúng ta sẽ tận dụng lại cube đã xây dựng ở Milestone 2, thay vì tạo thêm một loại hình học mới.

## 1. Thay đổi Fragment Shader

Hiện tại Fragment Shader sử dụng màu được truyền từ Vertex Shader.

Chúng ta cần bổ sung khả năng chọn màu riêng cho từng đối tượng.

Mở `assets/shaders/fragment.glsl`, thay bằng:

```glsl
#version 330 core

in vec3 vertexColor;

out vec4 FragColor;

uniform vec3 objectColor;
uniform bool useVertexColor;

void main()
{
    if (useVertexColor)
    {
        FragColor = vec4(vertexColor, 1.0);
    }
    else
    {
        FragColor = vec4(objectColor, 1.0);
    }
}
```

Giải thích:

- `useVertexColor = true`: Sử dụng màu RGB của từng vertex. Khối lập phương ban đầu vẫn có 6 màu.
- `useVertexColor = false`: Sử dụng một màu duy nhất được thiết lập qua `objectColor`. Chúng ta dùng cách này cho sàn và tường.

Vertex Shader không cần thay đổi.

## 2. Tạo hàm drawCube()

Mở `src/main.cpp`.

Thêm hàm sau phía trên `main()`:

```cpp
void drawCube(
    GLuint shaderProgram,
    GLuint VAO,
    const glm::mat4& model,
    const glm::vec3& color,
    bool useVertexColor)
{
    GLint modelLoc =
        glGetUniformLocation(shaderProgram, "model");

    GLint colorLoc =
        glGetUniformLocation(shaderProgram, "objectColor");

    GLint useVertexColorLoc =
        glGetUniformLocation(shaderProgram, "useVertexColor");

    glUniformMatrix4fv(
        modelLoc,
        1,
        GL_FALSE,
        glm::value_ptr(model)
    );

    glUniform3fv(
        colorLoc,
        1,
        glm::value_ptr(color)
    );

    glUniform1i(
        useVertexColorLoc,
        useVertexColor ? 1 : 0
    );

    glBindVertexArray(VAO);

    glDrawArrays(GL_TRIANGLES, 0, 36);
}
```

Hàm này giúp chúng ta tái sử dụng cùng một cube để tạo ra nhiều đối tượng khác nhau.

Ví dụ:

```cpp
drawCube(shaderProgram, VAO, floorModel,
         glm::vec3(0.4f, 0.4f, 0.4f), false);
```

Nghĩa là vẽ một khối lập phương với màu xám, sử dụng Model Matrix `floorModel`.

## 3. Xây dựng căn phòng

Chúng ta sẽ sử dụng hệ tọa độ 3D như sau:

- Trục X: Trái và phải.
- Trục Y: Lên và xuống.
- Trục Z: Trước và sau.

Căn phòng có kích thước:

| Thành phần | Kích thước |
|---|---|
| Floor | 20 × 0.2 × 20 |
| Wall | 20 × 5 × 0.2 |
| Camera ban đầu | (0, 0, 3) |
| Tâm phòng | (0, 0, 0) |

Trong Game Loop, tìm đoạn code cũ:

```cpp
// Model matrix
glm::mat4 model(1.0f);

model = glm::rotate(
    model,
    static_cast<float>(glfwGetTime()),
    glm::vec3(0.5f, 1.0f, 0.0f)
);

// View matrix
glm::mat4 view = camera.GetViewMatrix();

// Projection matrix
glm::mat4 projection = glm::perspective(
    glm::radians(45.0f),
    static_cast<float>(WIDTH) / HEIGHT,
    0.1f,
    100.0f
);

// Send matrices to shader
...
```

Thay phần xử lý Model, View, Projection và Render bằng đoạn code sau:

```cpp
// View matrix
glm::mat4 view = camera.GetViewMatrix();

// Projection matrix
glm::mat4 projection = glm::perspective(
    glm::radians(45.0f),
    static_cast<float>(WIDTH) / HEIGHT,
    0.1f,
    100.0f
);

// Activate shader
glUseProgram(shaderProgram);

// Send View Matrix
GLint viewLoc =
    glGetUniformLocation(shaderProgram, "view");

glUniformMatrix4fv(
    viewLoc,
    1,
    GL_FALSE,
    glm::value_ptr(view)
);

// Send Projection Matrix
GLint projectionLoc =
    glGetUniformLocation(shaderProgram, "projection");

glUniformMatrix4fv(
    projectionLoc,
    1,
    GL_FALSE,
    glm::value_ptr(projection)
);


// ==============================
// FLOOR
// ==============================

glm::mat4 floorModel(1.0f);

floorModel = glm::translate(
    floorModel,
    glm::vec3(0.0f, -1.0f, 0.0f)
);

floorModel = glm::scale(
    floorModel,
    glm::vec3(20.0f, 0.2f, 20.0f)
);

drawCube(
    shaderProgram,
    VAO,
    floorModel,
    glm::vec3(0.4f, 0.4f, 0.4f),
    false
);


// ==============================
// BACK WALL
// ==============================

glm::mat4 backWall(1.0f);

backWall = glm::translate(
    backWall,
    glm::vec3(0.0f, 1.5f, -10.0f)
);

backWall = glm::scale(
    backWall,
    glm::vec3(20.0f, 5.0f, 0.2f)
);

drawCube(
    shaderProgram,
    VAO,
    backWall,
    glm::vec3(0.7f, 0.3f, 0.3f),
    false
);


// ==============================
// FRONT WALL
// ==============================

glm::mat4 frontWall(1.0f);

frontWall = glm::translate(
    frontWall,
    glm::vec3(0.0f, 1.5f, 10.0f)
);

frontWall = glm::scale(
    frontWall,
    glm::vec3(20.0f, 5.0f, 0.2f)
);

drawCube(
    shaderProgram,
    VAO,
    frontWall,
    glm::vec3(0.3f, 0.3f, 0.7f),
    false
);


// ==============================
// LEFT WALL
// ==============================

glm::mat4 leftWall(1.0f);

leftWall = glm::translate(
    leftWall,
    glm::vec3(-10.0f, 1.5f, 0.0f)
);

leftWall = glm::scale(
    leftWall,
    glm::vec3(0.2f, 5.0f, 20.0f)
);

drawCube(
    shaderProgram,
    VAO,
    leftWall,
    glm::vec3(0.3f, 0.7f, 0.3f),
    false
);


// ==============================
// RIGHT WALL
// ==============================

glm::mat4 rightWall(1.0f);

rightWall = glm::translate(
    rightWall,
    glm::vec3(10.0f, 1.5f, 0.0f)
);

rightWall = glm::scale(
    rightWall,
    glm::vec3(0.2f, 5.0f, 20.0f)
);

drawCube(
    shaderProgram,
    VAO,
    rightWall,
    glm::vec3(0.7f, 0.7f, 0.3f),
    false
);


// ==============================
// ROTATING CUBE
// ==============================

glm::mat4 cubeModel(1.0f);

cubeModel = glm::rotate(
    cubeModel,
    static_cast<float>(glfwGetTime()),
    glm::vec3(0.5f, 1.0f, 0.0f)
);

drawCube(
    shaderProgram,
    VAO,
    cubeModel,
    glm::vec3(1.0f, 1.0f, 1.0f),
    true
);
```

**Lưu ý:** Giữ nguyên các lệnh `glfwSwapBuffers(window)` và `glfwPollEvents()` ở cuối Game Loop.

## 4. Build và chạy

```bash
cd ~/Projects/HalfLifeClone

cmake --build build -j$(nproc)

./build/HalfLifeClone
```

Bạn sẽ thấy một căn phòng với sàn, bốn bức tường và khối lập phương đang xoay ở giữa.

## 5. Chúng ta vừa học thêm điều gì?

Có hai khái niệm rất quan trọng trong OpenGL:

**Model Matrix**

Model Matrix xác định vị trí, kích thước và góc xoay của một đối tượng.

Ví dụ:

```cpp
glm::mat4 model(1.0f);

model = glm::translate(
    model,
    glm::vec3(0.0f, -1.0f, 0.0f)
);

model = glm::scale(
    model,
    glm::vec3(20.0f, 0.2f, 20.0f)
);
```

Đoạn code trên biến cube kích thước 1 × 1 × 1 thành một mặt sàn kích thước 20 × 0.2 × 20.

**Tái sử dụng Mesh**

Chúng ta chỉ cần tạo một VAO/VBO cho cube, sau đó dùng nhiều Model Matrix khác nhau để tạo ra nhiều đối tượng.

Đây là một nguyên tắc quan trọng trong thiết kế game engine.

---

## 6. Lộ trình tiếp theo

Sau Milestone 4, tôi đề xuất:

| Milestone | Nội dung |
|---|---|
| 4 | 3D Environment – Floor, Walls, Map |
| 5 | FPS Movement – Di chuyển trên mặt phẳng, Jump, Gravity |
| 6 | Collision Detection – Không đi xuyên qua tường |
| 7 | Texture Mapping – Áp dụng texture cho sàn và tường |
| 8 | Lighting – Ánh sáng và bóng đổ |
| 9 | 3D Model Loading – Đọc model `.obj` |
| 10 | Weapon System – Xây dựng súng góc nhìn thứ nhất |

**Bạn hãy hoàn thành Milestone 4 trước.** Sau đó chúng ta sẽ làm hệ thống Gravity, Jump và Collision Detection. Đây là những thành phần cốt lõi để biến môi trường 3D hiện tại thành một FPS game thực sự.
================================================================================================================================
Chúng ta tiếp tục với **Milestone 5 – FPS Movement: chuyển động nhân vật giống game Half-Life**.

Ở Milestone 3, chúng ta đã có camera điều khiển bằng chuột và WASD. Tuy nhiên, camera hiện tại vẫn có một số hạn chế:

- Khi nhìn lên và nhấn W, camera sẽ bay lên.
- Chưa có trọng lực (Gravity).
- Chưa thể nhảy (Jump).
- Chưa có khái niệm mặt đất (Ground).

Trong Milestone 5, chúng ta sẽ khắc phục những vấn đề này.

# Milestone 5 – FPS Movement

## 1. Mục tiêu

Sau khi hoàn thành, nhân vật sẽ có các chức năng:

| Điều khiển | Chức năng |
|---|---|
| W | Đi về phía trước |
| S | Đi lùi |
| A | Di chuyển sang trái |
| D | Di chuyển sang phải |
| Space | Nhảy |
| Mouse | Xoay hướng nhìn |
| ESC | Thoát game |

Đặc biệt:
- Nhân vật chỉ di chuyển trên mặt phẳng XZ.
- Camera không tự bay lên khi nhìn lên trời và nhấn W.
- Khi nhảy, nhân vật chịu tác động của trọng lực.
- Nhân vật có thể tiếp đất.

## 2. Tách hướng nhìn và hướng di chuyển

Đây là phần quan trọng nhất.

Camera FPS có hai hướng cần phân biệt:

**Camera Front:** hướng nhìn của camera, bao gồm cả Pitch (nhìn lên/xuống).

**Movement Direction:** hướng di chuyển trên mặt phẳng XZ, không phụ thuộc Pitch.

Ví dụ:

Bạn nhìn lên trời nhưng nhấn W. Nhân vật vẫn phải đi về phía trước trên mặt đất, không được bay lên.

Ta có thể tính hướng di chuyển như sau:

```cpp
glm::vec3 front;

front.x = cos(glm::radians(Yaw));
front.y = 0.0f;
front.z = sin(glm::radians(Yaw));

front = glm::normalize(front);
```

Lưu ý: `Yaw` quyết định hướng di chuyển ngang, còn `Pitch` chỉ ảnh hưởng đến hướng nhìn.

## 3. Cập nhật Camera.h

Thay thế nội dung `src/Camera.h` bằng phiên bản sau:

```cpp
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

enum CameraMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Camera
{
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;

    float MovementSpeed;
    float MouseSensitivity;

    Camera(glm::vec3 position =
           glm::vec3(0.0f, 0.0f, 3.0f))
    {
        Position = position;

        WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

        Yaw = -90.0f;
        Pitch = 0.0f;

        MovementSpeed = 5.0f;
        MouseSensitivity = 0.1f;

        updateCameraVectors();
    }

    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(
            Position,
            Position + Front,
            Up
        );
    }

    void ProcessKeyboard(
        CameraMovement direction,
        float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;

        // Calculate movement direction on XZ plane
        glm::vec3 forward;

        forward.x = cos(glm::radians(Yaw));
        forward.y = 0.0f;
        forward.z = sin(glm::radians(Yaw));

        forward = glm::normalize(forward);

        glm::vec3 right =
            glm::normalize(
                glm::cross(forward, WorldUp)
            );

        if (direction == FORWARD)
            Position += forward * velocity;

        if (direction == BACKWARD)
            Position -= forward * velocity;

        if (direction == LEFT)
            Position -= right * velocity;

        if (direction == RIGHT)
            Position += right * velocity;
    }

    void ProcessMouseMovement(
        float xOffset,
        float yOffset)
    {
        xOffset *= MouseSensitivity;
        yOffset *= MouseSensitivity;

        Yaw += xOffset;
        Pitch += yOffset;

        if (Pitch > 89.0f)
            Pitch = 89.0f;

        if (Pitch < -89.0f)
            Pitch = -89.0f;

        updateCameraVectors();
    }

private:

    void updateCameraVectors()
    {
        glm::vec3 front;

        front.x =
            cos(glm::radians(Yaw)) *
            cos(glm::radians(Pitch));

        front.y =
            sin(glm::radians(Pitch));

        front.z =
            sin(glm::radians(Yaw)) *
            cos(glm::radians(Pitch));

        Front = glm::normalize(front);

        Right = glm::normalize(
            glm::cross(Front, WorldUp)
        );

        Up = glm::normalize(
            glm::cross(Right, Front)
        );
    }
};
```

Điểm thay đổi quan trọng là hàm `ProcessKeyboard()`.

Trước đây, camera sử dụng `Front` để di chuyển. Bây giờ, chúng ta tính riêng `forward` trên mặt phẳng XZ.

## 4. Thêm Gravity và Jump

Bây giờ chúng ta cần quản lý trạng thái vật lý của nhân vật.

Thêm các biến sau vào phần đầu `main.cpp`:

```cpp
float playerSpeed = 5.0f;

float gravity = -9.81f;

float verticalVelocity = 0.0f;

float jumpForce = 5.0f;

float groundHeight = 0.0f;

bool isGrounded = true;
```

Ở đây:

| Biến | Ý nghĩa |
|---|---|
| gravity | Gia tốc trọng trường |
| verticalVelocity | Vận tốc theo trục Y |
| jumpForce | Vận tốc ban đầu khi nhảy |
| groundHeight | Độ cao mặt đất |
| isGrounded | Nhân vật có đang đứng trên mặt đất không |

### 4.1. Hàm Jump

Thêm hàm:

```cpp
void processJump(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS
        && isGrounded)
    {
        verticalVelocity = jumpForce;
        isGrounded = false;
    }
}
```

Khi nhấn Space, nếu nhân vật đang đứng trên mặt đất, ta tạo vận tốc nhảy theo trục Y.

### 4.2. Hàm cập nhật vật lý

```cpp
void updatePhysics(float deltaTime)
{
    // Apply gravity
    verticalVelocity += gravity * deltaTime;

    // Update player position
    camera.Position.y +=
        verticalVelocity * deltaTime;

    // Ground collision
    if (camera.Position.y <= groundHeight)
    {
        camera.Position.y = groundHeight;

        verticalVelocity = 0.0f;

        isGrounded = true;
    }
}
```

Đây là mô hình vật lý đơn giản.

Công thức:

\[
v = v_0 + gt
\]

\[
y = y_0 + vt
\]

Trong đó:
- \(v_0\): vận tốc ban đầu.
- \(g\): gia tốc trọng trường.
- \(t\): thời gian.
- \(y\): vị trí theo trục Y.

## 5. Cập nhật processInput()

Trong `main.cpp`, sửa hàm `processInput()`:

```cpp
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE)
        == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W)
        == GLFW_PRESS)
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_S)
        == GLFW_PRESS)
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_A)
        == GLFW_PRESS)
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_D)
        == GLFW_PRESS)
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    processJump(window);
}
```

## 6. Cập nhật Game Loop

Trong vòng lặp chính `while`, chúng ta cần gọi `updatePhysics()` sau `processInput()`:

```cpp
while (!glfwWindowShouldClose(window))
{
    float currentFrame =
        static_cast<float>(glfwGetTime());

    deltaTime = currentFrame - lastFrame;

    lastFrame = currentFrame;

    processInput(window);

    updatePhysics(deltaTime);

    glClearColor(
        0.05f, 0.05f, 0.08f, 1.0f
    );

    glClear(
        GL_COLOR_BUFFER_BIT |
        GL_DEPTH_BUFFER_BIT
    );

    // Render scene here

    glfwSwapBuffers(window);

    glfwPollEvents();
}
```

**Lưu ý về tọa độ:** Với căn phòng ở Milestone 4, mặt sàn có tâm tại `y = -1.0` và scale theo Y là `0.2`. Vì vậy, mặt trên của sàn nằm ở `y = -0.9`.

Nếu camera có vị trí là tâm camera, ta nên đặt `groundHeight` bằng độ cao camera khi đứng trên sàn, thay vì đặt bằng độ cao bề mặt sàn.

Ví dụ, camera cao 1.7 đơn vị so với mặt sàn:

```cpp
float groundHeight = 0.8f;
```

Và khởi tạo camera tại:

```cpp
Camera camera(glm::vec3(0.0f, 0.8f, 3.0f));
```

Điều này giúp camera không bị đặt xuyên vào sàn.

---

# 7. Kiểm tra kết quả

Sau khi sửa code:

```bash
cd ~/Projects/HalfLifeClone

cmake --build build -j$(nproc)

./build/HalfLifeClone
```

Thử lần lượt:

1. Nhấn W: nhân vật đi tới.
2. Nhấn S: nhân vật đi lùi.
3. Nhấn A/D: di chuyển ngang.
4. Nhìn lên trời và nhấn W: nhân vật không bay lên.
5. Nhấn Space: nhân vật nhảy.
6. Thả Space: nhân vật tiếp tục chịu trọng lực và rơi xuống.
7. Khi chạm đất, nhân vật có thể nhảy tiếp.

## 8. Một số giới hạn hiện tại

Milestone 5 mới triển khai vật lý cơ bản:

- Chưa có va chạm với tường.
- Chưa có giới hạn biên căn phòng.
- Chưa có tăng tốc/giảm tốc.
- Chưa có nhảy liên tục khi giữ Space (hiện tại chỉ nhảy khi đang grounded, nhưng có thể kích hoạt lại ngay khi tiếp đất).

**Milestone tiếp theo – Milestone 6: Collision Detection**

Chúng ta sẽ xây dựng hệ thống va chạm để:
- Không đi xuyên qua tường.
- Không đi xuyên qua các vật thể.
- Giữ nhân vật trong phạm vi căn phòng.
- Chuẩn bị nền tảng cho gameplay giống Half-Life.

Trước tiên, hãy hoàn thành Milestone 5 và kiểm tra xem nhân vật đã đi, nhìn, nhảy và tiếp đất đúng chưa.
=================================================================================================================================
"Tiếp tục dự án HalfLifeClone từ Milestone 6."