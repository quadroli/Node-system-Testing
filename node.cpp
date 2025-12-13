#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <sstream>
#include <map>
#include <set>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <utility>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <variant>
#include <fstream>
#include <unordered_map>
using namespace std;
using parameter = variant<int, float, double, long long, string, bool, vector<char>>;
#define PI 3.14159265358979323846

#include "lib/glew-2.1.0/include/GL/glew.h"
#include "lib/glfw-3.4/include/GLFW/glfw3.h"
#include "lib/imgui-master/imgui.h"
#include "lib/imgui-master/backends/imgui_impl_glfw.h"
#include "lib/imgui-master/backends/imgui_impl_opengl3.h"

enum class Class {
    Dummy,
    Input,
    Effect,
    Text,
    Grade,
    Auditory,
    Complex,
    SpecificComplex,
    Merge,
    Output,
    Compose,
    _3D,
    Camera,
    GreenScreen,
    MotionTracking,
    Rotoscope,
    ProceduralGeneration,
    AI,
    MotionGraphics,
    FaceMesh,
    BodyTracking,
    StylisedEngines,
};

enum class Type {
    Integer,
    String,
    Boolean,
    Float,
    File,
    Hidden,
};

map<Class, string> classNames{
    {Class::Dummy, "Dummy"},
    {Class::Input, "Input"},
    {Class::Effect, "Effect"},
    {Class::Text, "Text"},
    {Class::Grade, "Grade"},
    {Class::Auditory, "Auditory"},
    {Class::Complex, "Complex"},
    {Class::SpecificComplex, "Specific Complex"},
    {Class::Merge, "Merge"},
    {Class::Output, "Output"},
    {Class::Compose, "Compose"},
    {Class::_3D, "3D"},
    {Class::Camera, "Camera"},
};  

map<Class, vector<tuple<string, Type, parameter>>> classParameters{
    {Class::Dummy, vector<tuple<string, Type, parameter>>{}},
    {Class::Input, vector<tuple<string, Type, parameter>>{{"File", Type::File, vector<char>{}}}},
    {Class::Effect, vector<tuple<string, Type, parameter>>{}},
    {Class::Text, vector<tuple<string, Type, parameter>>{{"Text", Type::String, "Text"}}},
    {Class::Grade, vector<tuple<string, Type, parameter>>{}},
    {Class::Auditory, vector<tuple<string, Type, parameter>>{}},
    {Class::Complex, vector<tuple<string, Type, parameter>>{}},
    {Class::SpecificComplex, vector<tuple<string, Type, parameter>>{}},
    {Class::Merge, vector<tuple<string, Type, parameter>>{}},
    {Class::Output, vector<tuple<string, Type, parameter>>{}},
    {Class::Compose, vector<tuple<string, Type, parameter>>{}},
    {Class::_3D, vector<tuple<string, Type, parameter>>{}},
    {Class::Camera, vector<tuple<string, Type, parameter>>{}},
};

vector<tuple<string, Type, parameter>> globalParameters {
    {"X", Type::Integer, 0},
    {"Y", Type::Integer, 0},
};

class Color {
    public:
    float red, green, blue, alpha;
    Color merge(Color b, float amount = 0.5f) {
        Color a = *this;
        return Color(a.red * (1-amount) + b.red * amount,
        a.green * (1-amount) + b.green * amount,
        a.blue * (1-amount) + b.blue * amount,
        a.alpha * (1-amount) + b.alpha * amount);
    }
    explicit Color(float r, float g, float b, float a = 1) {
        red = r, green = g, blue = b, alpha = a;
    }
    explicit Color(float a = 1) {
        red = 0, green = 0, blue = 0, alpha = a;
    }
    static Color FromHex(string hex) {
        if (hex[0] == '#') {
            hex = hex.substr(1);
        }
        if (hex.size() != 6 && hex.size() != 8) {
            return Color(1);
        }
        transform(hex.begin(), hex.end(), hex.begin(), ::toupper);
        try {
            unsigned long full_color = stoul(hex, nullptr, 16);
            if (hex.size() == 6) {
                return Color::FromRGB((full_color >> 16) & 0xFF, (full_color >> 8) & 0xFF, full_color & 0xFF);
            } else {
                return Color::FromRGB((full_color >> 24) & 0xFF, (full_color >> 16) & 0xFF, (full_color >> 8) & 0xFF, full_color & 0xFF);
            }
            
        } catch (const std::invalid_argument& e) {
            return Color(1);
        } catch (const std::out_of_range& e) {
            return Color(1);
        }
    }
    static Color FromRGB(int r, int g, int b, int a = 255) {
        return Color(
            static_cast<float>(r) / 255.0f,
            static_cast<float>(g) / 255.0f,
            static_cast<float>(b) / 255.0f,
            static_cast<float>(a) / 255.0f
        );
    }
};

map<Class, Color> colors = {
    {Class::Dummy, Color::FromHex("#808080")},
    {Class::Input, Color::FromHex("#3CB4FF")},
    {Class::Effect, Color::FromHex("#6A3CFF")},
    {Class::Text, Color::FromHex("#C67DFF")},
    {Class::Grade, Color::FromHex("#FFE14A")},
    {Class::Auditory, Color::FromHex("#8A0000")},
    {Class::Complex, Color::FromHex("#2D2D2D")},
    {Class::SpecificComplex, Color::FromHex("#1F0F2E")},
    {Class::Merge, Color::FromHex("#FF7A3C")},
    {Class::Output, Color::FromHex("#FF4A4A")},
    {Class::Compose, Color::FromHex("#1A8A7A")},
    {Class::_3D, Color::FromHex("#FFB14A")},
    {Class::Camera, Color::FromHex("#4A9FFF")},
};

class Node;

using NodePtr = unique_ptr<Node>;

class Graph {
    public:
    vector<NodePtr> nodes;
    map<int, ImVec2> nodepos;
    map<int, ImVec2> centres;
    int id = 0;
    int next_id = 0;
    void OnDelete(int n, int deleted);
    void Delete(int id);
    void Link(int id, int id2);
    void Unlink(int id, int id2);
    void Package(int id, int id2);
    void Unpackage(int id);
    Node& Create(Class _class, int branch_id, bool after);
    Node& CreateUnlinked(Class _class);
    Node& GetNode(int id);
};

class Node;

class Parameter {
    public:
    int id;
    string name;
    Type type;
    parameter val;
    Node* parent;
};

enum class FileType {
    UNKNOWN,
    // Images
    JPEG,
    PNG,
    GIF,
    // Video
    AVI,
    MP4,
    WEBM,
    // Audio
    MP3,
    WAV,
    FLAC,
    OGG
};
string fileTypeToString(FileType type) {
    switch (type) {
        case FileType::JPEG: return "JPEG Image (.jpg/.jpeg)";
        case FileType::PNG: return "PNG Image (.png)";
        case FileType::GIF: return "GIF Image (.gif)";
        case FileType::MP3: return "MP3 Audio (.mp3)";
        case FileType::MP4: return "MP4/MOV Container (.mp4/.mov)";
        case FileType::AVI: return "AVI Video (.avi)";
        case FileType::WEBM: return "WEBM Video (.webm)";
        case FileType::WAV: return "WAV Audio (.wav)";
        case FileType::FLAC: return "FLAC Audio (.flac)";
        case FileType::OGG: return "OGG Audio (.ogg)";
        case FileType::UNKNOWN:
        default: return "UNKNOWN or UNSUPPORTED FORMAT";
    }
}

std::map<std::vector<unsigned char>, FileType> magicNumbers = {
    //0x00 represents an arbitrary bit.

    //Images
    {{0xFF, 0xD8, 0xFF}, FileType::JPEG}, 
    {{0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A}, FileType::PNG},
    {{0x47, 0x49, 0x46, 0x38, 0x39, 0x61}, FileType::GIF}, // GIF89a
    {{0x47, 0x49, 0x46, 0x38, 0x37, 0x61}, FileType::GIF}, // GIF87a
    //Videos
    {{0x52, 0x49, 0x46, 0x46, 0x00, 0x00, 0x00, 0x00, 0x41, 0x56, 0x49, 0x20}, FileType::AVI}, // 'RIFF'; Check bytes 8-11 for AVI
    {{0x00, 0x00, 0x00, 0x00, 0x66, 0x74, 0x79, 0x70}, FileType::MP4},
    {{0x1A, 0x45, 0xDF, 0xA3}, FileType::WEBM}, 
    
    //Audio
    {{0x49, 0x44, 0x33}, FileType::MP3}, //ID2
    {{0x66, 0x4C, 0x61, 0x43}, FileType::FLAC}, // 'fLaC'
    {{0x52, 0x49, 0x46, 0x46, 0x00, 0x00, 0x00, 0x00, 0x57, 0x41, 0x56, 0x45}, FileType::WAV},
    {{0x4F, 0x67, 0x67, 0x53}, FileType::OGG} // 'OggS'
};

const int HEADER_SIZE = 12;

class File {
    public:
    string filename;
    ifstream file;
    void openFile(string nam) {
        filename = nam;
        file.open(filename, ios::binary);
    }
    FileType getType() {
        if (!file.is_open()) {
            return FileType::UNKNOWN;
        }
        vector<char> buffer(HEADER_SIZE);
        file.read(buffer.data(), HEADER_SIZE);
        streamsize bytesRead = file.gcount();
        for (auto kvp = magicNumbers.begin(); kvp != magicNumbers.end(); ++kvp) {
            bool yay = true;
            for (int i = 0; i < bytesRead; ++i) {
                if (i >= kvp->first.size()) break;
                if (kvp->first[i] == (unsigned char)buffer[i] || kvp->first[i] == 0x00) continue;
                yay = false;
                break;
            }
            if (yay) {
                return kvp->second;
            }
        }
        return FileType::UNKNOWN;
    }
};

class Node {
    public:
    int id = 0, p_id_c = 0;
    ImVec2 pos;
    Class _class;
    Graph* myGraph;
    string name;
    bool deleted = false;
    vector<int> in, out, pkg;
    vector<Parameter> params;
    bool operator==(Node b) {
        return this->id == b.id;
    }
    Node(Graph* g, Class c) {
        myGraph = g;
        id = g->next_id;
        ++g->next_id;
        SetClass(c);
    }
    Node(Graph* g, Class c, int branch, bool after) {
        myGraph = g;
        id = g->next_id;
        ++g->next_id;
        if (after) {
            in.push_back(branch);
            g->nodes[branch]->out.push_back(id);
        } else {
            out.push_back(branch);
            g->nodes[branch]->in.push_back(id);
        }
        SetClass(c);
    }
    void SetClass (Class c) {
        _class = c;
        while (params.size() > globalParameters.size())
        {
            params.pop_back();
        }
        for (tuple<string, Type, parameter> param : classParameters[c]) {
            Parameter p;
            p.id = p_id_c;
            ++p_id_c;
            p.name = get<0>(param);
            p.type = get<1>(param);
            p.val = get<2>(param);
            params.push_back(p);
        }
    }
    void Rename (string nam) {
        name = nam;
    }
};

Node& Graph::GetNode(int id) {
    for (const auto& node_ptr : nodes) {
        if (node_ptr->id == id) {
            return *node_ptr;
        }
    }
    throw runtime_error("Node ID not found.");
}

void Graph::Delete(int id) {
    Node& node = *nodes[id];
    for (int i = 0; i < node.in.size(); ++i) {
        OnDelete(node.in[i], id);
    } 
    for (int i = 0; i < node.out.size(); ++i) {
        OnDelete(node.out[i], id);
    } 
    for (int i = 0; i < node.pkg.size(); ++i) {
        OnDelete(node.pkg[i], id);
    } 
    node.in.clear();
    node.out.clear();
    node.pkg.clear();
    node.deleted = true;
}

Node& Graph::Create(Class _class, int branch_id, bool after) {
    unique_ptr<Node> new_node_ptr(new Node(this, _class, branch_id, after));
    Node* new_node_raw = new_node_ptr.get();
    nodes.emplace_back(move(new_node_ptr));
    return *new_node_raw; 
}

Node& Graph::CreateUnlinked(Class _class) {
    unique_ptr<Node> new_node_ptr(new Node(this, _class));
    Node* new_node_raw = new_node_ptr.get();
    nodes.emplace_back(move(new_node_ptr));
    return *new_node_raw; 
}

void Graph::OnDelete(int n, int deleted_id) {
    Node& node = *nodes[n];
    node.in.erase(std::remove(node.in.begin(), node.in.end(), deleted_id), node.in.end());
    node.out.erase(std::remove(node.out.begin(), node.out.end(), deleted_id), node.out.end());
    node.pkg.erase(std::remove(node.pkg.begin(), node.pkg.end(), deleted_id), node.pkg.end());
}

void Graph::Link(int id, int id2) {
    nodes[id]->out.push_back(id2);
    nodes[id2]->in.push_back(id);
}

void Graph::Unlink(int id, int id2) {
    nodes[id]->in.erase(remove(nodes[id]->in.begin(), nodes[id]->in.end(), id2), nodes[id]->in.end());
    nodes[id]->out.erase(remove(nodes[id]->out.begin(), nodes[id]->out.end(), id2), nodes[id]->out.end());
    nodes[id2]->in.erase(remove(nodes[id2]->in.begin(), nodes[id2]->in.end(), id), nodes[id2]->in.end());
    nodes[id2]->out.erase(remove(nodes[id2]->out.begin(), nodes[id2]->out.end(), id), nodes[id2]->out.end());
}

void Graph::Package(int id, int id2) {
    nodes[id]->pkg.push_back(id2);
    nodes[id2]->in.push_back(id);
}

void Graph::Unpackage(int id) {
    if (nodes[id]->pkg.size() == 0) return;
    int id2 = nodes[id]->pkg[0];
    nodes[id]->pkg.erase(remove(nodes[id]->pkg.begin(), nodes[id]->pkg.end(), id2), nodes[id]->pkg.end());
}


void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

char name_buffer[128];

ImVec2 rotate(ImVec2 pt, float angle, ImVec2 offset = {0, 0}) {
    ImVec2 ret = {pt.x * cos(angle) - pt.y * sin(angle), pt.x * sin(angle) + pt.y * cos(angle)};
    ret = {ret.x + offset.x, ret.y + offset.y};
    return ret;
}

void drawLine(ImVec2 startpos, ImVec2 endpos, ImDrawList* drawList, ImU32 colour, bool directed) {
    drawList->AddLine(startpos, endpos, colour, 3.0f);
    if (directed) {
        ImVec2 midway = {startpos.x/2 + endpos.x/2, startpos.y/2 + endpos.y/2};
        float slope, angle;
        if (startpos.x == endpos.x) {
            if (endpos.y > startpos.y) {
                angle = 90;
            } else {
                angle = 270;
            }
        } else if (startpos.y == endpos.y) {
            if (endpos.x > startpos.x) {
                angle = 0;
            } else {
                angle = 180;
            }
        } else {
            slope = (endpos.y - startpos.y)/(endpos.x - startpos.x);
            angle = atan(slope);
        }
        if (startpos.x > endpos.x) {
            angle += PI;
        }
        ImVec2 p1, p2, p3;
        p1 = {0, 10}, p2 = {20, 0}, p3 = {0, -10};
        p1 = rotate(p1, angle, midway), p2 = rotate(p2, angle, midway), p3 = rotate(p3, angle, midway);
        drawList->AddTriangleFilled(
            p1, 
            p2, 
            p3, 
            colour
        );
    }
}

int main(int, char**) {
    Graph graph;
    graph.CreateUnlinked(Class::Input).Rename("Input");
    graph.Create(Class::Effect, 0, true).Rename("Effect");
    graph.Create(Class::Text, 1, true).Rename("Text");
    graph.Create(Class::Auditory, 1, true).Rename("Music");
    graph.Create(Class::Camera, 3, false).Rename("Cam");

    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    const char* glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui Graph Editor", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); 
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW\n"; //doesn't print
        return 1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    int drawing = 0, startNode = 0;
    bool inputShield = true;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            int selected = -1;
            string action;
            for (auto& ptr : graph.nodes) {
                int id = ptr->id;
                if (ptr->deleted) continue;
                ImVec2& pos = graph.nodepos[id];

                string window_name = "Node " + to_string(id) + " (" + classNames[ptr->_class] + ")";
                Color c;
                if (colors.count(ptr->_class)) {
                    c = colors[ptr->_class];
                } else {
                    c = colors[Class::Dummy];
                }

                ImGui::PushStyleColor(ImGuiCol_TitleBg, 
                ImVec4(c.red, c.green, c.blue, 1.0f));

                ImGui::SetNextWindowPos(pos, ImGuiCond_Once);
                ImGui::Begin(window_name.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize);
                ImGui::Text("Name: %s", ptr->name.c_str());
                ImGui::Text("ID: %d", id);
                ImGui::Text("Class: %s", classNames[ptr->_class].c_str());
                for (Parameter param : ptr->params) {
                    switch(param.type) {
                        case Type::String: {
                            ImGui::Text(param.name.c_str());
                            ImGui::SameLine();
                            strncpy(name_buffer, get<string>(param.val).c_str(), sizeof(name_buffer) - 1);
                            name_buffer[sizeof(name_buffer) - 1] = '\0';
                            ImGui::InputText(("##TextInput"+to_string(graph.id)+"."+to_string(ptr->id)+"."+to_string(param.id)).c_str(), name_buffer, sizeof(name_buffer));
                            if (ImGui::IsItemDeactivatedAfterEdit() || ImGui::IsItemDeactivated()) {
                                param.val = name_buffer;
                            }
                        }
                    }
                }

                if (ImGui::Button("Create New Linked Node")) {
                    selected = id;
                    action = "CreateLinkedNode";
                }
                if (ImGui::Button("Delete")) {
                    selected = id;
                    action = "Delete";
                }
                if (ImGui::Button("Add Link")) {
                    startNode = id;
                    drawing = 1;
                }
                if (ImGui::Button("Package")) {
                    startNode = id;
                    drawing = 2;
                }

                if (ImGui::IsWindowHovered() && (ImGui::IsMouseClicked(ImGuiMouseButton_Left)||ImGui::IsMouseReleased(ImGuiMouseButton_Left))) {
                    if (id == startNode) {
                        inputShield = true;
                    } else {
                        if (drawing == 1) {
                            graph.Link(startNode, id);
                        } else if (drawing == 2) {
                            graph.Package(startNode, id);
                        }
                        if (drawing > 0) {
                            drawing = 0;
                            startNode = 0;
                        }
                    }
                }
                
                if (ImGui::BeginPopupContextWindow()) {
                    selected = id;
                    if (ImGui::IsWindowAppearing()) { 
                        strncpy(name_buffer, ptr->name.c_str(), sizeof(name_buffer) - 1);
                        name_buffer[sizeof(name_buffer) - 1] = '\0';
                    }
                    
                    ImGui::Text("Rename:");
                    ImGui::InputText("##NodeNameEdit", name_buffer, sizeof(name_buffer));
                    
                    if (ImGui::IsItemDeactivatedAfterEdit() || ImGui::IsItemDeactivated()) {
                        ptr->Rename(name_buffer);
                    }

                    ImGui::Separator();

                    // --- Change Class Combo Box ---
                    ImGui::Text("Change Class:");
                    const char* class_items[] = { 
                        "Dummy", "Input", "Effect", "Text", "Grade", "Auditory", "Complex", "SpecificComplex",
                        "Merge", "Output", "Compose", "3D", "Camera", "GreenScreen",
                    };
                    int current_class_idx = static_cast<int>(ptr->_class);

                    // Update index if the node's class changed externally
                    if (current_class_idx != static_cast<int>(ptr->_class)) {
                        current_class_idx = static_cast<int>(ptr->_class);
                    }

                    if (ImGui::BeginCombo("##NodeClassCombo", class_items[current_class_idx])) {
                        for (int i = 0; i < IM_ARRAYSIZE(class_items); i++) {
                            bool is_selected = (current_class_idx == i);
                            if (ImGui::Selectable(class_items[i], is_selected)) {
                                current_class_idx = i;
                                // Apply the new class
                                ptr->SetClass(static_cast<Class>(i)); 
                            }
                            if (is_selected) {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }
                    ImGui::EndPopup();
                }
                ImVec2 size = ImGui::GetWindowSize();
                graph.centres[id] = ImVec2(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f);

                graph.nodepos[id] = ImGui::GetWindowPos();
                ImGui::PopStyleColor();
                ImGui::End();
            }
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)||ImGui::IsMouseReleased(ImGuiMouseButton_Left) && !inputShield) {
                drawing = 0;
                startNode = 0;
            }
            inputShield = false;
            ImGui::Begin("Graph Canvas", 
             NULL, 
             ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | 
             ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | 
             ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground);
            ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
            for (const auto& node_ptr : graph.nodes) {
                int start_id = node_ptr->id;
                if (graph.centres.count(start_id)) {
                    ImVec2 start_pos = graph.centres[start_id];
                    for (int end_id : node_ptr->out) {
                        if (graph.centres.count(end_id)) {
                            drawLine(start_pos, graph.centres[end_id], draw_list, IM_COL32(0, 192, 255, 255), true);
                        }
                    }
                    for (int end_id : node_ptr->pkg) {
                        if (graph.centres.count(end_id)) {
                            drawLine(start_pos, graph.centres[end_id], draw_list, IM_COL32(252, 123, 219, 255), true);
                        }
                    }
                    if (drawing > 0 && startNode == node_ptr->id) {
                        ImVec2 end_pos = ImGui::GetMousePos();
                        ImU32 line_color;
                        if (drawing == 2) {
                            line_color = IM_COL32(252, 123, 219, 128);
                        } else if (drawing == 1) {
                            line_color = IM_COL32(0, 192, 255, 128);
                        }
                        draw_list->AddLine(start_pos, end_pos, line_color, 3.0f);
                    }
                }                
            }
            ImGui::End();
            
            
            ImGui::Begin("Graph Node Editor");
            ImGui::Text("Hello from ImGui!");
            if (ImGui::Button("Create New Node")) {
                Node& newNode = graph.CreateUnlinked(Class::Input);
                newNode.Rename("New Input");
                ImVec2 initial_position = ImVec2(50.0f, 50.0f);
                graph.nodepos[newNode.id] = initial_position;
            }
            ImGui::End();

            if (selected >= 0) {
                if (action == "CreateLinkedNode") {
                    Node& newNode = graph.Create(Class::Input, selected, true);
                    newNode.Rename("New Input");
                    ImVec2 initial_position = ImVec2(50.0f, 50.0f);
                    graph.nodepos[newNode.id] = initial_position;
                } else if (action == "Delete") {
                    graph.Delete(selected);
                }

            }
        }
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}