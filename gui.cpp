#include "gui.h"
#include "BT.h"

// Data
static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

// Const
const int MAX_SECONDS = 300;
//const int DATA_LIMIT = g_globals.tempData.size();



void setupTempData()
{
    for (int i = 0; i < MAX_SECONDS; i++) // TODO tempData initialization
    {
        if (i < 250 || i > 275)
        {
            //g_globals.tempData.push_back(25 + (rand() % 30));
            g_globals.tempData.push_back(20 + (i / 200.0));

        }
        else
        {
            g_globals.tempData.push_back(-200);
        }
    }

    // added for saftey, should never reach unless inputs are messed up
    if (g_globals.tempData.empty() ||
        g_globals.tempIndex >= g_globals.tempData.size())
    {
        return;
    }

    // Iterate until the queue is at max length or each value of the data array is reached

    int x = 0;
    while (x < g_globals.tempData.size())
    {
        //g_globals.tempDataQ.push(g_globals.tempData[g_globals.tempIndex]);
        //if (g_globals.tempDataQ.size() > DATA_LIMIT)
        //{
        //    g_globals.tempDataQ.pop();
        //}
        if (g_globals.tempData[(g_globals.tempData.size() - g_globals.tempIndex) - 1] > -127)
        {
            if (g_globals.faren == false)
            {
                g_globals.finalTempData[x] = g_globals.tempData[(g_globals.tempData.size() - g_globals.tempIndex) - 1];
            }
            else
            {
                g_globals.finalTempData[x] = (g_globals.tempData[(g_globals.tempData.size() - g_globals.tempIndex) - 1] * 9.0 / 5.0) + 32;
            }
        }
        else
        {
            g_globals.finalTempData[x] = std::sqrt(-1); // make missing data NaN so it is skipped by ImPlot()
        }


        g_globals.tempIndex++;

        if (g_globals.tempIndex >= g_globals.tempData.size())
        {
            g_globals.tempIndex = 0;
        }

        x++;
    }
}

void destroyTempData()
{
    g_globals.tempData = vector<float>();
    //g_globals.tempDataQ = queue<float>();
    // finalTempData = new float[300] {};
}

int GUI() {
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("TEAM 0xC"), NULL };
    ::RegisterClassEx(&wc);

    // TODO background window styles
    DWORD winStyle = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU; // | WS_BORDER; //WS_OVERLAPPED | WS_THICKFRAME;

    HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("TEAM 0xC"), winStyle, 400, 150, 814, 600, NULL, NULL, wc.hInstance, NULL);

    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    // TODO: using demo window??
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    setupTempData();

    // Main loop
    bool done = false;
    while (!done) {
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Handle window resize (we don't resize directly in the WM_SIZE handler)
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            g_d3dpp.BackBufferWidth = g_ResizeWidth;
            g_d3dpp.BackBufferHeight = g_ResizeHeight;
            g_ResizeWidth = g_ResizeHeight = 0;
            ResetDevice();
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
            ImGui::SetNextWindowSize(ImVec2(800, 600));

            ImGui::Begin("SD Lab1!", NULL, window_flags);

            if (isnan(g_globals.finalTempData[0]))
            {
                ImGui::Text("Temperature Sensor is Currently Unplugged");
            }
            else if (g_globals.faren)
            {
                ImGui::Text("Current Temperature in degrees celsius: %.2f", g_globals.finalTempData[0]);
            }
            else
            {
                ImGui::Text("Current Temperature in degrees farenheit: %.2f", g_globals.finalTempData[0]);
            }

            const char* ledBox;
            if (g_globals.enableLED)
            {
                ledBox = "On";
            }
            else
            {
                ledBox = "Off";
            }

            if (ImGui::Checkbox("Enable LED", &g_globals.enableLED)) {
                g_globals.currentID = globals::MessageID::setDisplay;
                if (!writeData()) {
                    cout << "Failed to write Enable LED command" << endl;
                }
            }


            const char* tempBox;
            if (g_globals.faren)
            {
                tempBox = "Farenheit";
                //yMin = 50; yMax = 122;
            }
            else
            {
                tempBox = "Celcius";
                //yMin = 10; yMax = 50;
            }

            ImGui::Text("Temperature Mode:"); ImGui::SameLine();
            ImGui::Checkbox(tempBox, &g_globals.faren);

            // TODO: move this or smthn
            char MessageBuf[250]{};
            char PhoneBuf[10]{};
            char TempBuf[5]{};
            ImGuiInputTextCallback TextEditCallbackStub = nullptr;
            bool reclaim_focus = false;
            ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
            // TODO: add this flag "ImGuiInputTextFlags_EscapeClearsAll", current imgui ver is outdated :D


            // imgui::inputtextwithhint TODO

            static std::string textMessage = "";

            ImGui::Text("\nText Message information:");
            ImGui::Text("Text Message Content:"); ImGui::SameLine();
            ImGui::InputTextMultiline(" ", MessageBuf, sizeof(MessageBuf), ImVec2(500, ImGui::GetTextLineHeight() * 3), ImGuiInputTextFlags_EnterReturnsTrue);

            static float phoneNum = 0;
            ImGui::Text("Phone Number:"); ImGui::SameLine();
            ImGui::InputFloat("  ", &phoneNum, 0.0f, 0.0f, "%.1f", ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsDecimal);

            static float lowerThreshold = 0.0f;
            static float upperThreshold = 0.0f;

            // ImGui::Text("Threshold Values:");
            ImGui::Text("Lower Threshold:"); ImGui::SameLine();
            ImGui::InputFloat("   ", &lowerThreshold, 0.0f, 0.0f, "%.1f", ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsDecimal); //, input_text_flags);
            ImGui::Text("Upper Threshold:"); ImGui::SameLine();
            ImGui::InputFloat("    ", &upperThreshold, 0.0f, 0.0f, "%.1f", ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsDecimal); //, input_text_flags);

            ImGui::NewLine();

            //ImPlotStyleVar_FitPadding,         // ImVec2, additional fit padding as a percentage of the fit extents (e.g. ImVec2(0.1f,0.1f) adds 10% to the fit extents of X and Y)
            //ImPlotStyleVar_PlotDefaultSize,    // ImVec2, default size used when ImVec2(0,0) is passed to BeginPlot
            //ImPlotStyleVar_PlotMinSize,        // ImVec2, minimum size plot frame can be when shrunk

            // To change plot size:
            // ImPlot::PushStyleVar(ImPlotStyleVar_PlotDefaultSize, ImVec2(-100, 100));



            ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 2.0f);  // Set line thickness to 2.0
            ImPlot::PushStyleVar(ImPlotStyleVar_Marker, 1);  // type of point/marker
            ImPlot::PushStyleVar(ImPlotStyleVar_MarkerSize, 2.0f);  // size of point/marker


            // ImPlotFlags_NoMouseText to remove text that shows when moving mouse on graph
            // ImPlotFlags_Equal to change x and y axis pairs
            // ImFlipFlag(plot.Flags, ImPlotFlags_NoLegend);


            if (!empty(g_globals.finalTempData)) {
                if (ImPlot::BeginPlot("Temperature Data (Right-Click for Options)", ImVec2(-1, 0), ImPlotFlags_Crosshairs | ImPlotFlags_NoLegend)) {
                    if (g_globals.faren)
                    {
                        ImPlot::SetupAxes("Seconds ago from current time", "Temperature in Degrees Farenheit",
                            ImPlotAxisFlags_Invert, // make x axis go from 300-0 instead of 0-300
                            ImPlotAxisFlags_Opposite // visually move y axis to the right side of the graph
                        );
                    }
                    else
                    {
                        ImPlot::SetupAxes("Seconds ago from current time", "Temperature in Degrees Celcius",
                            ImPlotAxisFlags_Invert, // make x axis go from 300-0 instead of 0-300
                            ImPlotAxisFlags_Opposite // visually move y axis to the right side of the graph
                        );
                    }

                    double yMin; double yMax;
                    if (g_globals.faren)
                    {
                        tempBox = "Farenheit";
                        yMin = 50; yMax = 122;
                    }
                    else
                    {
                        tempBox = "Celcius";
                        yMin = 10; yMax = 50;
                    }


                    ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, 0, MAX_SECONDS);
                    ImPlot::SetupAxisLimitsConstraints(ImAxis_Y1, yMin, yMax);

                    // TODO: ImPlotLineFlags_SkipNaN, when enabled a line is drawn from where NaN sections start/end
                    ImPlot::PlotLine("Recorded Temperature", &g_globals.finalTempData[0], MAX_SECONDS + 1,
                        1.0,
                        0,
                        ImPlotLineFlags_NoClip // makes points/markers on border of constraints visible
                    );

                    //ImPlot::AddTextCentered()

                    ImPlot::EndPlot();
                }
            }

            ImPlot::PopStyleVar();
            ImGui::End();
        }

        destroyTempData();
        setupTempData();


        // Rendering
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();

        Sleep(20);
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    ImPlot::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}