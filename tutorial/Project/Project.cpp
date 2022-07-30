#include "Project.h"
#include <iostream>
#include "./igl/file_dialog_open.h"
#include "./GuiHandler.h"

Layer *defaultLayer = new Layer(true, "default", std::vector<int>());

IGL_INLINE void Project::my_open_dialog_load_mesh()
{
    std::string fname = igl::file_dialog_open();

    if (fname.length() == 0)
        return;

    LoadMeshFromFile(fname.c_str());
}

int indxFile = 0;

bool Project::LoadMeshFromFile(
    const std::string &mesh_file_name_string)
{
    int index = AddShapeObjectFromFile("file: " + indxFile++, mesh_file_name_string, -1, this, 0);
    SetShapeShader(index, 2);
    SetShapeMaterial(index, 2);
    std::cout << "Load mesh from file: " << mesh_file_name_string << std::endl;
    return true;
}

Project::Project() {}

void Project::SetMenu(igl::opengl::glfw::imgui::ImGuiMenu *menu)
{
    this->menu = menu;
}

void Project::TextureModeChange()
{
    for (int i : pShapes)
    {
        SceneObject *shp = sceneObjects[i];
        int shpIndx = shp->getIndex();
        if (data_list[shpIndx]->mode == TRIANGLES)
        {
            data_list[shpIndx]->mode = LINES;
        }
        else
            data_list[shpIndx]->mode = TRIANGLES;
    }
}

int Project::AddShapeObjectFromFile(std::string name, std::string file_name, int parent, Viewer *viewer, int viewPort = 0)
{
    int shpIndx = AddShapeFromFile(file_name, parent, TRIANGLES, viewPort);
    if (viewPort == 0)
    {
        SetShapeViewport(shpIndx, 1);
    }
    SceneObject *scnShape = new SceneObject(name, MeshCopy, defaultLayer, shpIndx, viewer);
    scnShape->setlastBizPosition(Eigen::Vector3f(0, 0, 0));
    sceneObjects.push_back(scnShape);
    return shpIndx;
}

int Project::AddShapeObject(std::string name, igl::opengl::glfw::Viewer::shapes shapeType,
                            Viewer *viewer, int parent, int viewPort = 0)
{
    int shpIndx = AddShape(shapeType, parent, TRIANGLES, viewPort);
    if (viewPort == 0)
    {
        SetShapeViewport(shpIndx, 1);
    }
    SceneObject *scnShape = new SceneObject(name, shapeType, defaultLayer, shpIndx, viewer);
    scnShape->setlastBizPosition(Eigen::Vector3f(0, 0, 0));
    sceneObjects.push_back(scnShape);
    return shpIndx;
}

void Project::Init(int DISPLAY_WIDTH, int DISPLAY_HEIGHT)
{
    farShapes = std::vector<int>();
    this->DISPLAY_WIDTH = DISPLAY_WIDTH;
    this->DISPLAY_HEIGHT = DISPLAY_HEIGHT;
    layers.push_back(defaultLayer);
    globalTime = 0;
    unsigned int texIDs[6] = {0, 1, 2, 3, 4, 5};
    unsigned int slots[6] = {0, 1, 2, 3, 4, 5};
    int pickingShaderIndx = AddShader("shaders/pickingShader");
    int cubemapShaderIndx = AddShader("shaders/cubemapShader");
    basicShaderIndx = AddShader("shaders/basicShader");
    blurShaderIndx = AddShader("shaders/blurShader");

    unsigned int grassTexIndx = AddTexture("textures/grass.bmp", 2);
    unsigned int dayLightBoxTexIndx = AddTexture("textures/cubemaps/Daylight Box_", 3);
    unsigned int boxTexIndx = AddTexture("textures/box0.bmp", 2);
    unsigned int planeTexIndx = AddTexture("textures/plane.png", 2);
    unsigned int snakeTexIndx = AddTexture("textures/snake.jpg", 2);
    unsigned int brickTexIndx = AddTexture("textures/bricks.jpg", 2);

    int grass2DMatIndx = AddMaterial(&grassTexIndx, slots, 1);
    int dayLight3DMatIndx = AddMaterial(&dayLightBoxTexIndx, slots + 1, 1);
    int box2DMatIndx = AddMaterial(&boxTexIndx, slots + 2, 1);
    int plane2DMatIndx = AddMaterial(&planeTexIndx, slots + 3, 1);
    int snake2DMatIndx = AddMaterial(&snakeTexIndx, slots + 4, 1);
    int brick2DMatIndx = AddMaterial(&brickTexIndx, slots + 5, 1);

    std::vector<Eigen::Vector3f> points = {Eigen::Vector3f(0, 0, 0),
                                           Eigen::Vector3f(0, 10, 0),
                                            Eigen::Vector3f(-5, 10, 0),
                                            Eigen::Vector3f(-5, 0, 0) };

    // Cube map -->
    cubeMapIndx = AddShapeObject("cubeMap", Cube, this, -2);
    SetShapeShader(cubeMapIndx, cubemapShaderIndx);
    SetShapeMaterial(cubeMapIndx, dayLight3DMatIndx);

    selected_data_index = cubeMapIndx;
    ShapeTransformation(scaleAll, 150, 0);
    SetShapeStatic(cubeMapIndx);

    // End cubeMap
    // Picking plane -->

    pickingPlaneIndx = AddShapeObject("Picking plane", Plane, this, -2, 2);
    SetShapeShader(pickingPlaneIndx, pickingShaderIndx);
    SetShapeMaterial(pickingPlaneIndx, plane2DMatIndx);

    selected_data_index = pickingPlaneIndx;
    ShapeTransformation(zTranslate, -1.1, 1);

    SetShapeStatic(pickingPlaneIndx);

    // Picking plane End
    int index;
    index = AddShapeObject("test", Octahedron, this, -1);
    SetShapeShader(index, basicShaderIndx);
    SetShapeMaterial(index, box2DMatIndx);
    sceneObjects[index]->addBiz(BezMovment(points, 0, 300), &max_time);
    sceneObjects[index]->move(1, y);
    sceneObjects[index]->move(3, x);

    index = AddShapeObject("test 1", Cube, this, -4);
    SetShapeShader(index, basicShaderIndx);
    SetShapeMaterial(index, grass2DMatIndx);
    sceneObjects[index]->move(2, x);
    sceneObjects[index]->move(2, z);

    index = AddShapeObject("test 2", Cube, this, -3);
    SetShapeShader(index, basicShaderIndx);
    SetShapeMaterial(index, brick2DMatIndx);

    sceneObjects[selected_data_index]->move(-1, y);
}

float Project::calcDistanceFromCam(SceneObject *shp)
{
    Eigen::Vector3f cameraPos = renderer->cameraPos;
    Eigen::Vector3f shapePos = shp->getCurrentPosition();
    return (shapePos - cameraPos).norm();
}

void Project::updateFarShapes()
{
    for (int indx : farShapes)
    {
        SetShapeShader(indx, basicShaderIndx);
    }
    farShapes.clear();
    for (SceneObject *shp : sceneObjects)
    {
        if (shp->getIndex() == cubeMapIndx || shp->getIndex() == pickingPlaneIndx)
        {
            continue;
        }
        float dist = calcDistanceFromCam(shp);
        if (dist > farCoeff)
        {
            SetShapeShader(shp->getIndex(), blurShaderIndx);
            shp->blurC = (dist - 4) / 16;
            farShapes.push_back(shp->getIndex());
        }
    }
}

void Project::Update(const Eigen::Matrix4f &Proj, const Eigen::Matrix4f &View, const Eigen::Matrix4f &Model, unsigned int shaderIndx, unsigned int shapeIndx)
{
    updateFarShapes();
    Shader *s = shaders[shaderIndx];
    long ctime;
    if (isActive)
    {
        if (shapeIndx == cubeMapIndx)
            ++globalTime;
        if (globalTime >= max_time)
        {
            globalTime = max_time;
            Deactivate();
        }
        ctime = globalTime;
    }
    else
        ctime = time;

    if (shapeIndx != cubeMapIndx && shapeIndx != pickingPlaneIndx)
    {

        SceneObject *scnShape = sceneObjects[shapeIndx];
        Eigen::Vector3f pos = scnShape->getCurrentPosition();
        Eigen::Vector3f bizPos = scnShape->getCurrentPositionAt((float)ctime);
        Eigen::Vector3f newPos = scnShape->getDesignPosition() + bizPos;

        if (newPos != pos)
        {
            Eigen::Vector3f delta = newPos - pos;
            selected_data_index = shapeIndx;
            ShapeTransformation(xTranslate, delta(x), 0);
            ShapeTransformation(yTranslate, delta(y), 0);
            ShapeTransformation(zTranslate, delta(z), 0);

            scnShape->setlastBizPosition(bizPos);
        }
    }

    s->SetUniform2f("vsR", (int)(sceneObjects[shapeIndx]->blurC * 30) + 10, 0);

    s->Bind();
    s->SetUniformMat4f("Proj", Proj);
    s->SetUniformMat4f("View", View);
    s->SetUniformMat4f("Model", Model);
    if (!sceneObjects[shapeIndx]->getLayer()->getIsVisible())
    {
        s->SetUniform2f("transparency", 0, 0);
    }
    else if (sceneObjects[shapeIndx]->isTransparent)
    {
        s->SetUniform2f("transparency", 0.4, 0);
    }
    else
    {
        s->SetUniform2f("transparency", 1, 0);
    }
    if (data_list[shapeIndx]->GetMaterial() >= 0 && !materials.empty())
    {
        BindMaterial(s, data_list[shapeIndx]->GetMaterial());
    }
    s->Unbind();
}

void Project::WhenRotate()
{
}

void Project::WhenTranslate()
{
}

const int numOfCubeMaps = 4;

std::string cubeMaps[numOfCubeMaps] = {"Daylight Box_", "grass_cubemap_",
                                       "desert_cubemap_","nature_cubemap_"};

int cubeMapCurrTextIndx = 0;

void Project::PassCubeMap()
{
    cubeMapCurrTextIndx = (cubeMapCurrTextIndx + 1) % numOfCubeMaps;
    ChangeCubeMap(cubeMaps[cubeMapCurrTextIndx]);
}

void Project::ChangeCubeMap(std::string file_name)
{
    unsigned int tex = AddTexture("textures/cubemaps/" + file_name, 3);
    unsigned int numOfMats = materials.size();
    int cubeMat = AddMaterial(&tex, &numOfMats, 1);
    SetShapeMaterial(cubeMapIndx, cubeMat);
}

void Project::Animate()
{
    animating = !animating;
}

void Project::ScaleAllShapes(float amt, int viewportIndx)
{
    for (int i = 1; i < data_list.size(); i++)
    {
        if (data_list[i]->Is2Render(viewportIndx))
        {
            data_list[i]->MyScale(Eigen::Vector3d(amt, amt, amt));
        }
    }
}

Project::~Project(void)
{
}

void Project::SetRenderer(Renderer *renderer)
{
    if (this->renderer != nullptr)
        throw std::invalid_argument("renderer cannot be set twice");
    this->renderer = renderer;
}

void Project::Play()
{
    animating = !animating;
    globalTime = -1;
}
