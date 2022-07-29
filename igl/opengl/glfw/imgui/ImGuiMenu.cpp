// This file is part of libigl, a simple c++ geometry processing library.
//
// Copyright (C) 2018 Jérémie Dumas <jeremie.dumas@ens-lyon.org>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.
////////////////////////////////////////////////////////////////////////////////
//#include "ImGuiMenu.h"
//#include "ImGuiHelpers.h"
#include <igl/project.h>
#include "ImGuiHelpers.h"

#include "ImGuiMenu.h"
#include "../imgui.h"
#include "igl/opengl/glfw/imgui/imgui_impl_glfw.h"
#include "igl/opengl/glfw/imgui/imgui_impl_opengl3.h"

//#include <imgui_fonts_droid_sans.h>
//#include <GLFW/glfw3.h>
#include <iostream>
////////////////////////////////////////////////////////////////////////////////

#include "../../../../tutorial/Project/GuiHandler.h"
#include "../../../../tutorial/Project/Project.h"
#include "../../ViewerData.h"

namespace igl
{
    namespace opengl
    {
        namespace glfw
        {
            namespace imgui
            {

                IGL_INLINE void ImGuiMenu::init(Display *disp)
                {
                    // Setup ImGui binding
                    if (disp->window)
                    {
                        IMGUI_CHECKVERSION();
                        if (!context_)
                        {
                            // Single global context by default, but can be overridden by the user
                            static ImGuiContext *__global_context = ImGui::CreateContext();
                            context_ = __global_context;
                        }
                        const char *glsl_version = "#version 150";

                        ImGui_ImplGlfw_InitForOpenGL(disp->window, true);
                        ImGui_ImplOpenGL3_Init(glsl_version);
                        ImGui::GetIO().IniFilename = nullptr;
                        ImGui::StyleColorsDark();
                        ImGuiStyle &style = ImGui::GetStyle();
                        style.FrameRounding = 5.0f;
                        reload_font();
                    }
                }

                IGL_INLINE void ImGuiMenu::reload_font(int font_size)
                {
                    hidpi_scaling_ = hidpi_scaling();
                    pixel_ratio_ = pixel_ratio();
                    ImGuiIO &io = ImGui::GetIO();
                    io.Fonts->Clear();
                    // io.Fonts->AddFontFromMemoryCompressedTTF(droid_sans_compressed_data,
                    //   droid_sans_compressed_size, font_size * hidpi_scaling_);
                    io.FontGlobalScale = 1.0 / pixel_ratio_;
                }

                IGL_INLINE void ImGuiMenu::shutdown()
                {
                    // Cleanup
                    ImGui_ImplOpenGL3_Shutdown();
                    ImGui_ImplGlfw_Shutdown();
                    // User is responsible for destroying context if a custom context is given
                    // ImGui::DestroyContext(*context_);
                }

                IGL_INLINE bool ImGuiMenu::pre_draw()
                {
                    glfwPollEvents();

                    // Check whether window dpi has changed
                    float scaling = hidpi_scaling();
                    if (std::abs(scaling - hidpi_scaling_) > 1e-5)
                    {
                        reload_font();
                        ImGui_ImplOpenGL3_DestroyDeviceObjects();
                    }

                    ImGui_ImplOpenGL3_NewFrame();
                    ImGui_ImplGlfw_NewFrame();
                    ImGui::NewFrame();
                    return false;
                }

                IGL_INLINE bool ImGuiMenu::post_draw()
                {
                    // draw_menu(viewer,core);
                    ImGui::Render();
                    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                    return false;
                }

                IGL_INLINE void ImGuiMenu::post_resize(int width, int height)
                {
                    if (context_)
                    {
                        ImGui::GetIO().DisplaySize.x = float(width);
                        ImGui::GetIO().DisplaySize.y = float(height);
                    }
                }

                // Mouse IO
                IGL_INLINE bool ImGuiMenu::mouse_down(GLFWwindow *window, int button, int modifier)
                {
                    ImGui_ImplGlfw_MouseButtonCallback(window, button, GLFW_PRESS, modifier);
                    return ImGui::GetIO().WantCaptureMouse;
                }

                IGL_INLINE bool ImGuiMenu::mouse_up(GLFWwindow *window, int button, int modifier)
                {
                    // return ImGui::GetIO().WantCaptureMouse;
                    //  !! Should not steal mouse up
                    return false;
                }

                IGL_INLINE bool ImGuiMenu::mouse_move(GLFWwindow *window, int mouse_x, int mouse_y)
                {
                    return ImGui::GetIO().WantCaptureMouse;
                }

                IGL_INLINE bool ImGuiMenu::mouse_scroll(GLFWwindow *window, float delta_y)
                {
                    ImGui_ImplGlfw_ScrollCallback(window, 0.f, delta_y);
                    return ImGui::GetIO().WantCaptureMouse;
                }

                // Keyboard IO
                IGL_INLINE bool ImGuiMenu::key_pressed(GLFWwindow *window, unsigned int key, int modifiers)
                {
                    ImGui_ImplGlfw_CharCallback(nullptr, key);
                    return ImGui::GetIO().WantCaptureKeyboard;
                }

                IGL_INLINE bool ImGuiMenu::key_down(GLFWwindow *window, int key, int modifiers)
                {
                    ImGui_ImplGlfw_KeyCallback(window, key, 0, GLFW_PRESS, modifiers);
                    return ImGui::GetIO().WantCaptureKeyboard;
                }

                IGL_INLINE bool ImGuiMenu::key_up(GLFWwindow *window, int key, int modifiers)
                {
                    ImGui_ImplGlfw_KeyCallback(window, key, 0, GLFW_RELEASE, modifiers);
                    return ImGui::GetIO().WantCaptureKeyboard;
                }

                IGL_INLINE void ImGuiMenu::draw_viewer_menu(igl::opengl::glfw::Viewer *viewer, std::vector<igl::opengl::Camera *> &camera, Eigen::Vector4i &viewWindow, std::vector<DrawInfo *> drawInfos)
                {
                    Project *scn = ((Project *)viewer);
                    bool *p_open = NULL;
                    static bool no_titlebar = false;
                    static bool no_scrollbar = false;
                    static bool no_menu = true;
                    static bool no_move = false;
                    static bool no_resize = false;
                    static bool no_collapse = false;
                    static bool no_close = false;
                    static bool no_nav = false;
                    static bool no_background = false;
                    static bool no_bring_to_front = false;

                    ImGuiWindowFlags window_flags = 0;
                    if (no_titlebar)
                        window_flags |= ImGuiWindowFlags_NoTitleBar;
                    if (no_scrollbar)
                        window_flags |= ImGuiWindowFlags_NoScrollbar;
                    if (!no_menu)
                        window_flags |= ImGuiWindowFlags_MenuBar;
                    if (no_move)
                        window_flags |= ImGuiWindowFlags_NoMove;
                    if (no_resize)
                        window_flags |= ImGuiWindowFlags_NoResize;
                    if (no_collapse)
                        window_flags |= ImGuiWindowFlags_NoCollapse;
                    if (no_nav)
                        window_flags |= ImGuiWindowFlags_NoNav;
                    if (no_background)
                        window_flags |= ImGuiWindowFlags_NoBackground;
                    if (no_bring_to_front)
                        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
                    ImGui::Begin(
                        "Viewer", p_open,
                        window_flags);

                    ImGui::SetWindowPos(ImVec2((float)0, (float)0), ImGuiCond_Always);
                    ImGui::SetWindowSize(ImVec2((float)0, (float)0), ImGuiCond_Always);
                    ImGui::End();
                    no_move = true;
                    no_resize = true;
                    ImGui::Begin(
                        "Viewer", p_open,
                        window_flags);

                    // Mesh
                    if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        float w = ImGui::GetContentRegionAvailWidth();
                        float p = ImGui::GetStyle().FramePadding.x;
                        if (ImGui::Button("Load##Mesh", ImVec2((w - p), 0)))
                        {
                            // int savedIndx = viewer->selected_data_index;
                            // viewer->open_dialog_load_mesh();
                            (scn)->my_open_dialog_load_mesh();
                            // if (viewer->data_list.size() > viewer->parents.size())
                            // {
                            //     viewer->parents.push_back(-1);
                            //     viewer->data_list.back()->set_visible(false, 1);
                            //     viewer->data_list.back()->set_visible(true, 2);
                            //     viewer->data_list.back()->show_faces = 3;
                            //     viewer->selected_data_index = savedIndx;
                            // }
                        }
                    }

                    // Viewing options
                    if (ImGui::CollapsingHeader("Viewing Options", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        // Zoom
                        ImGui::PushItemWidth(80 * menu_scaling());
                        if (camera[0]->_ortho)
                            ImGui::DragFloat("Zoom", &(camera[0]->_length), 0.05f, 0.1f, 20.0f);
                        else
                            ImGui::DragFloat("Fov", &(camera[0]->_fov), 0.05f, 30.0f, 90.0f);

                        // Select rotation type
                        static Eigen::Quaternionf trackball_angle = Eigen::Quaternionf::Identity();
                        static bool orthographic = true;

                        // Orthographic view
                        ImGui::Checkbox("Orthographic view", &(camera[0]->_ortho));
                        if (camera[0]->_ortho)
                        {
                            camera[0]->SetProjection(0, camera[0]->_relationWH);
                        }
                        else
                        {
                            camera[0]->SetProjection(camera[0]->_fov > 0 ? camera[0]->_fov : 45, camera[0]->_relationWH);
                        }

                        ImGui::PopItemWidth();
                    }

                    // Helper for setting viewport specific mesh options
                    auto make_checkbox = [&](const char *label, unsigned int &option)
                    {
                        return ImGui::Checkbox(
                            label,
                            [&]()
                            { return drawInfos[1]->is_set(option); },
                            [&](bool value)
                            { return drawInfos[1]->set(option, value); });
                    };

                    if (ImGui::CollapsingHeader("Layers", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        for (size_t i = 0; i < (scn)->layers.size(); i++)
                        {
                            bool isVisible = ((scn)->layers.at(i)->getIsVisible());
                            if (ImGui::Checkbox(((scn)->layers.at(i)->getName()).c_str(),
                                                &isVisible))
                            {
                                std::cout << "layer changed:" << ((scn)->layers.at(i))->getIsVisible() << std::endl;
                                GuiHandler::OnLayerChange(((scn)->layers.at(i)->getName()), isVisible, scn);
                            }
                        }
                        ImGui::InputText("", viewer->data()->layer_name);
                        if (ImGui::Button("Add Layer"))
                        {
                            GuiHandler::OnAddLayer(viewer->data()->layer_name, true, scn);
                        }
                    }

                    if (ImGui::CollapsingHeader("Picked Objects", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        // Material
                        std::string tempStr = "material" + (std::to_string(viewer->data()->material_indx));
                        const char *current_material_item = tempStr.c_str();
                        if (ImGui::BeginCombo("##materials combo", current_material_item))
                        {
                            for (size_t n = 0; n < viewer->materials.size(); n++)
                            {
                                bool is_selected = strcmp(current_material_item, ("material" + (std::to_string(n))).c_str()) == 0;
                                if (ImGui::Selectable(("material" + (std::to_string(n))).c_str(), is_selected))
                                {
                                    current_material_item = ("material" + (std::to_string(n))).c_str();
                                    viewer->data()->material_indx = n;
                                    GuiHandler::OnSelectMaterial(n, scn);
                                }
                                if (is_selected)
                                {
                                    ImGui::SetItemDefaultFocus();
                                }
                            }

                            ImGui::EndCombo();
                        }

                        // Transparency
                        bool isAllTransparent = true;

                        for (size_t i = 0; i < scn->pShapes.size(); i++)
                        {
                            int pickedIndx = scn->pShapes[i];
                            if (scn->sceneObjects.at(pickedIndx)->isTransparent == false)
                            {
                                isAllTransparent = false;
                                break;
                            }
                        }

                        if (ImGui::Checkbox("Set Transparent", &isAllTransparent))
                        {
                            GuiHandler::OnTransparentToggled(isAllTransparent, scn);
                        }

                        // Far blur

                        ImGui::SliderFloat("Far Value", &scn->farCoeff, scn->GetRenderer()->GetNear(0), scn->GetRenderer()->GetFar(0));

                        // Set blur
                        if (ImGui::Checkbox("Set Blur", &viewer->data()->isBlur))
                        {
                            GuiHandler::OnBlurToggled(viewer->data()->isBlur, scn);
                        }

                        // Layers
                        ImGui::Text("Set a layer:");

                        std::string tempLayerSetStr = (scn)->layers.at(viewer->data()->layerSetIndex)->getName();
                        const char *current_layer_set_item = tempLayerSetStr.c_str();
                        if (ImGui::BeginCombo("##layers set combo", current_layer_set_item))
                        {
                            for (size_t n = 0; n < (scn)->layers.size(); n++)
                            {
                                bool is_selected = strcmp(current_layer_set_item, (scn)->layers.at(n)->getName().c_str()) == 0;
                                if (ImGui::Selectable((scn)->layers.at(n)->getName().c_str(), is_selected))
                                {
                                    current_layer_set_item = (scn)->layers.at(n)->getName().c_str();
                                    viewer->data()->layerSetIndex = n;
                                    GuiHandler::OnSetLayer((scn)->layers.at(n)->getName(), scn);
                                }
                                if (is_selected)
                                {
                                    ImGui::SetItemDefaultFocus();
                                }
                            }

                            ImGui::EndCombo();
                        }

                        // Parents

                        if (ImGui::Button("Choose childs"))
                        {
                            viewer->data()->selectedTempChilds.clear();
                            for (int i : scn->pShapes)
                            {
                                viewer->data()->selectedTempChilds.push_back(i);
                            }
                        }

                        if (ImGui::Button("Set parent"))
                        {
                            GuiHandler::onSetChilds(viewer->data()->selectedTempChilds, scn);
                        }

                        // selectedTempChilds

                        // End picked
                    }

                    if (ImGui::CollapsingHeader("Camera Split", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        if (ImGui::RadioButton("No Split", &(viewer->data()->camera_split), 0))
                        {
                            GuiHandler::OnSplitChange(0, scn);
                        }
                        if (ImGui::RadioButton("Split x", &(viewer->data()->camera_split), 1))
                        {
                            GuiHandler::OnSplitChange(1, scn);
                        }
                        if (ImGui::RadioButton("Split y", &(viewer->data()->camera_split), 2))
                        {
                            GuiHandler::OnSplitChange(2, scn);
                        }

                        ImGui::Text("Select Camera for screen 1:");
                        const char *current_item = viewer->data()->cameras.at(viewer->data()->cameraScreen1Indx).c_str();
                        if (ImGui::BeginCombo("##cameras combo 1", current_item))
                        {
                            for (size_t n = 0; n < viewer->data()->cameras.size(); n++)
                            {
                                bool is_selected = (current_item == viewer->data()->cameras.at(n).c_str());
                                if (ImGui::Selectable(viewer->data()->cameras.at(n).c_str(), is_selected))
                                {
                                    current_item = viewer->data()->cameras.at(n).c_str();
                                    viewer->data()->cameraScreen1Indx = n;
                                    GuiHandler::OnSetCamera(0, n, scn);
                                }
                                if (is_selected)
                                {
                                    ImGui::SetItemDefaultFocus();
                                }
                            }

                            ImGui::EndCombo();
                        }
                        ImGui::Text("Select Camera for screen 2:");
                        const char *current_item2 = (viewer->data()->cameras.at(viewer->data()->cameraScreen2Indx)).c_str();

                        if (ImGui::BeginCombo("##cameras combo 2", current_item2))
                        {
                            for (size_t n = 0; n < viewer->data()->cameras.size(); n++)
                            {
                                bool is_selected = (current_item2 == viewer->data()->cameras.at(n).c_str());
                                if (ImGui::Selectable(viewer->data()->cameras.at(n).c_str(), is_selected))
                                {
                                    current_item2 = viewer->data()->cameras.at(n).c_str();
                                    viewer->data()->cameraScreen2Indx = n;
                                    GuiHandler::OnSetCamera(1, n, scn);
                                }
                                if (is_selected)
                                {
                                    ImGui::SetItemDefaultFocus();
                                }
                            }
                            ImGui::EndCombo();
                        }
                        ImGui::Text("Camera name:");
                        ImGui::InputText(".", (viewer->data()->camera_name));
                        if (ImGui::Button("Add Camera"))
                        {
                            viewer->data()->cameras.push_back(viewer->data()->camera_name);
                            GuiHandler::OnAddCamera(viewer->data()->camera_name, scn);
                        }
                    }

                    if (ImGui::CollapsingHeader("Animation", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        if (ImGui::Checkbox("Play Animation", &(viewer->data()->play_is_active)))
                        {
                            viewer->isActive = viewer->data()->play_is_active;
                            GuiHandler::OnPlayChanged(viewer->isActive, scn);
                            if (viewer->isActive)
                                viewer->Activate();
                            else
                                viewer->Deactivate();
                        }
                        
                        if (ImGui::SliderFloat("Time Scale", &(scn->time), 0, (scn)->max_time, "%.1f"))
                        {
                            
                        }

                        // Adding to viewer time param
                        if (ImGui::CollapsingHeader("Add movment to selected shapes", ImGuiTreeNodeFlags_DefaultOpen))
                        {
                            ImGui::InputFloat3("point", scn->bizPoint);
                            if (ImGui::Button("Add point"))
                                GuiHandler::onAddPoint(scn->bizPoint, scn);
                            ImGui::InputFloat2("time", scn->start_end_time);
                            if (ImGui::Button("Add Bézier movment"))
                                GuiHandler::onAddBiz(scn->start_end_time, scn);
                            if (ImGui::Button("Remove Bézier movment"))
                                GuiHandler::onRemoveBiz(scn);
                        }
                    }

                    ImGui::End();
                }

                IGL_INLINE float ImGuiMenu::pixel_ratio()
                {
                    // Computes pixel ratio for hidpi devices
                    int buf_size[2];
                    int win_size[2];
                    GLFWwindow *window = glfwGetCurrentContext();
                    glfwGetFramebufferSize(window, &buf_size[0], &buf_size[1]);
                    glfwGetWindowSize(window, &win_size[0], &win_size[1]);
                    return (float)buf_size[0] / (float)win_size[0];
                }

                IGL_INLINE float ImGuiMenu::hidpi_scaling()
                {
                    // Computes scaling factor for hidpi devices
                    float xscale, yscale;
                    GLFWwindow *window = glfwGetCurrentContext();
                    glfwGetWindowContentScale(window, &xscale, &yscale);
                    return 0.5 * (xscale + yscale);
                }

            } // end namespace
        }     // end namespace
    }         // end namespace
} // end namespace
