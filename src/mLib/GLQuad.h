
class GLQuad
{
public:
    void init()
    {
        static const float diamond[4][2] = {
            { 0.0, 0.0 }, /* Top point */
            { 0.0, 1.0 }, /* Right point */
            { 1.0, 1.0 }, /* Bottom point */
            { 1.0, 0.0 } }; /* Left point */

        static const float texCoords[4][2] = {
            { 0.0, 0.0, }, /* Red */
            { 0.0, 1.0, }, /* Green */
            { 1.0, 1.0, }, /* Blue */
            { 1.0, 0.0, } }; /* White */

        /* Allocate and assign a Vertex Array Object to our handle */
        glGenVertexArrays(1, &vao);

        /* Bind our Vertex Array Object as the current used object */
        glBindVertexArray(vao);

        /* Allocate and assign two Vertex Buffer Objects to our handle */
        glGenBuffers(2, vbo);

        /* Bind our first VBO as being the active buffer and storing vertex attributes (coordinates) */
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

        /* Copy the vertex data from diamond to our buffer */
        /* 8 * sizeof(GLfloat) is the size of the diamond array, since it contains 8 GLfloat values */
        glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), diamond, GL_STATIC_DRAW);

        /* Specify that our coordinate data is going into attribute index 0, and contains two floats per vertex */
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

        /* Enable attribute index 0 as being used */
        glEnableVertexAttribArray(0);

        /* Bind our second VBO as being the active buffer and storing vertex attributes (colors) */
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);

        /* Copy the color data from colors to our buffer */
        /* 12 * sizeof(GLfloat) is the size of the colors array, since it contains 12 GLfloat values */
        glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), texCoords, GL_STATIC_DRAW);

        /* Specify that our color data is going into attribute index 1, and contains three floats per vertex */
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

        /* Enable attribute index 1 as being used */
        glEnableVertexAttribArray(1);
    }

    void render()
    {
        /* Invoke glDrawArrays telling that our data is a line loop and we want to draw 2-4 vertexes */
        glDrawArrays(GL_QUADS, 0, 4);
    }

private:
    GLuint vao, vbo[2];

    //glDeleteBuffers(2, vbo);
    //glDeleteVertexArrays(1, &vao);
};

