#include "SceneObject.h"

// Layer ------>
Layer::Layer(bool isVisible, std::string layerName, std::vector<int> layerShapes) : isVisible(isVisible), layerName(layerName), layerShapes(layerShapes) {}

std::string Layer::getName()
{
    return this->layerName;
}

void Layer::setName(std::string name)
{
    this->layerName = name;
}

bool Layer::getIsVisible()
{
    return this->isVisible;
}

void Layer::setIsVisible(bool toSet)
{
    this->isVisible = toSet;
}
std::vector<int> Layer::getLayerShapes()
{
    return this->layerShapes;
}

void Layer::addShape(int shapeIndex) {
    this->layerShapes.push_back(shapeIndex);
}

void Layer::removeShape(int shapeIndex) {
    std::vector<int> layerShapes;
    for (int num : this->layerShapes) {
        if (num != shapeIndex)
            layerShapes.push_back(num);
    }
    this->layerShapes = layerShapes;

}
//END Layer ------->

SceneObject::SceneObject(std::string shapeName, igl::opengl::glfw::Viewer::shapes shapeType,
    Layer* layer, int index, igl::opengl::glfw::Viewer* viewer) : name(shapeName), type(shapeType),
    layer(layer),parent(nullptr), index(index), designPosition(Eigen::Vector3f(0, 0, 0)), lastBizPosition(Eigen::Vector3f(0, 0, 0)), viewer(viewer)
{
    movement = std::vector<BezMovment>();
}

Layer* SceneObject::getLayer()
{
    return layer;
}
void SceneObject::changeLayer(Layer* layer)
{
    this->layer = layer;
}
int SceneObject::getIndex() {
    return index;
}


void SceneObject::addBiz(BezMovment biz, float *max_time) {
    this->movement.push_back(biz);
    if (biz.end_time > *max_time)
        *max_time = biz.end_time;
}

void SceneObject::clearBiz() {
    this->movement.clear();
}


Eigen::Vector3f SceneObject::getlastBizPosition() {
    return lastBizPosition;
}

void SceneObject::setlastBizPosition(Eigen::Vector3f pos) {
    lastBizPosition = pos;
}

Eigen::Vector3f SceneObject::getDesignPosition() {
        return designPosition;
}

void SceneObject::setDesignPosition(Eigen::Vector3f pos) {
    designPosition = pos;
}


SceneObject* SceneObject::getParent() {
    return parent;
}

void SceneObject::removeParent()
{
    currentPosition = getCurrentPositionAt(0);
    parent = nullptr;

}

void SceneObject::setParent(SceneObject* newParent) {
    if (parent != nullptr)
    {
        parent->removeChild(this);
    }
    parent = newParent;
    currentPosition = currentPosition - (newParent == nullptr ? Eigen::Vector3f(0,0,0) : newParent->getCurrentPosition());
    if (parent != nullptr)
    {
        parent->addChild(this);
    }
}



Eigen::Vector3f SceneObject::getCurrentPositionAt(float time)
{
    if (parent == nullptr)
        return getBizPosition(time);
    else
        return (getBizPosition(time) + this->parent->getBizPosition(time));
}

Eigen::Vector3f SceneObject::getCurrentPosition() {
    return lastBizPosition + designPosition;
}

std::vector<SceneObject*> SceneObject::getChildren() {
    return children;
}

void SceneObject::addChild(SceneObject* child) {
    children.push_back(child);
}

void SceneObject::removeChild(SceneObject* removedChild) {
    std::vector<SceneObject*> newChildren;
    for (SceneObject* child : children) {
        if (child != removedChild)
            newChildren.push_back(child);
    }
    children = newChildren;
}

///move object

Eigen::Vector3f SceneObject::getBizPosition(float time) {
    for (std::size_t i = 0; i < this->movement.size(); i++) {
        if (movement[i].start_time <= time && movement[i].end_time >= time) {
            return movement[i].getPosition(time);
        }
    }
    return Eigen::Vector3f(0, 0, 0);
}
void SceneObject::Scale(double shiftSize, directions d) {
    switch (d)
    {
    case x:
        viewer->data_list[getIndex()]->MyScale(Eigen::Vector3d(shiftSize, 1, 1));
        break;
    case y:
        viewer->data_list[getIndex()]->MyScale(Eigen::Vector3d(1, shiftSize, 1));
        break;

    case z:
        viewer->data_list[getIndex()]->MyScale(Eigen::Vector3d(1, 1, shiftSize));
        break;

    case w:
        viewer->data_list[getIndex()]->MyScale(Eigen::Vector3d(shiftSize, shiftSize, shiftSize));
        break;
    }
}

void SceneObject::move(double shiftSize,directions d) {
    switch (d) {
        case x:
            viewer->data_list[getIndex()]->MyTranslate(Eigen::Vector3d(shiftSize, 0 , 0) , false);
            designPosition += Eigen::Vector3f(shiftSize, 0 , 0);
            break;

        case y:
            viewer->data_list[getIndex()]->MyTranslate(Eigen::Vector3d(0, shiftSize, 0), false);
            designPosition += Eigen::Vector3f(0, shiftSize, 0);
            break;

        case z:
            viewer->data_list[getIndex()]->MyTranslate(Eigen::Vector3d(0 , 0 , shiftSize) , false);
            designPosition += Eigen::Vector3f(0 , 0 , shiftSize);
            break;
    }
    
    for (SceneObject* child : children) {
        child->move(shiftSize, d);
    }
    
}
