
// Cube vertices (x, y, z) all positive, larger scale
 float vertices[] = {
    // Front face
     0.0f,  0.0f,  10.0f,  // 0
    10.0f,  0.0f,  10.0f,  // 1
    10.0f, 10.0f,  10.0f,  // 2
     0.0f, 10.0f,  10.0f,  // 3
    // Back face
     0.0f,  0.0f,  20.0f,  // 4
    10.0f,  0.0f,  20.0f,  // 5
    10.0f, 10.0f,  20.0f,  // 6
     0.0f, 10.0f,  20.0f   // 7
};

// Indices for the 12 triangles (2 per face)
unsigned int indices[] = {
    // Front
    0, 1, 2,
    2, 3, 0,
    // Right
    1, 5, 6,
    6, 2, 1,
    // Back
    5, 4, 7,
    7, 6, 5,
    // Left
    4, 0, 3,
    3, 7, 4,
    // Top
    3, 2, 6,
    6, 7, 3,
    // Bottom
    4, 5, 1,
    1, 0, 4
};
