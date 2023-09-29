#include "gui.h"
#include "BT.h"

// Data
static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

// tempData but shifted by in order of most to least recent and shifted by tempIndex
float finalTempData[300] = { -127 };

const int MAX_SECONDS = 300;

// Populate finalTempData using tempData
void setupTempData()
{
    // Iterate until each value of the data array is reached 
    // populates finalTempData from newest data to oldest
    // Converts from Celsius to Farenheit if needed
    int x = 0;
    while (x < g_globals.tempData.size())
    {
        if (g_globals.faren == true)
        {
            if (isnan(g_globals.tempData[x]))
            {
                finalTempData[g_globals.tempData.size() - x] = g_globals.tempData[x];
            }
            else
            {
                finalTempData[g_globals.tempData.size() - x] = (g_globals.tempData[x] * 9.0 / 5.0) + 32;
            }
        }
        else
        {
            finalTempData[g_globals.tempData.size() - x] = g_globals.tempData[x];
        }
        x++;
    }
}

int GUI() {
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("TEAM 0xC"), NULL };
    ::RegisterClassEx(&wc);

    DWORD winStyle = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU; // | WS_BORDER; //WS_OVERLAPPED | WS_THICKFRAME;

    HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("TEAM 0xC"), winStyle, 400, 150, 814, 680, NULL, NULL, wc.hInstance, NULL);

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

    // Buffer to store user text message
    char messageBuf[sizeof(char) * 140]{};

    //for (int i = 0; i < (sizeof(char) * 140); i++)
    //{
    //    if (i % 25 == 0)
    //    {
    //        messageBuf[i] = '\n';
    //    }
    //}

    // buffer to store threshold vals
    float upperThres[MAX_SECONDS + 2]{}; // plus 2 to ensure line goes all the way through graph
    float lowerThres[MAX_SECONDS + 2]{};

    // user entered phone number
    char areaCode[4] = {};
    char phoneNum1[4] = {};
    char phoneNum2[5] = {};

    int phoneCount = 0;
    bool enableTextMessage = true;

    // used to limit to one text message per going over/under threshold
    bool alreadySent = false;

    // Carrier options
    const char* carrierNames[] = { "AT&T", "Boost Mobile", "Consumer Cellular", "Cricket", "C-Spire", "Google Fi", "Mint Mobile",
                "Sprint", "T-Mobile", "US Cellular", "US Mobile", "Verizon", "Virgin Mobile", "Xfinity Mobile" };
    int numOfCarriers = end(carrierNames) - begin(carrierNames);

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
            ImGui::SetNextWindowSize(ImVec2(800, 680));

            ImGui::Begin("SD Lab1!", NULL, window_flags);

            // Display current temperature, unless NaN meaning unplugged sensor
            if (isnan(finalTempData[1]))
            {
                // red
                ImGui::TextColored(ImVec4(255, 0, 0, 255), "Temperature Sensor is Currently Unplugged");
            }
            else if (!g_globals.faren)
            {
                // green
                ImGui::TextColored(ImVec4(0, 255, 0, 255), "Current Temperature in degrees Celsius: %.2f", finalTempData[1]);

            }
            else
            {
                // green
                ImGui::TextColored(ImVec4(0, 255, 0, 255), "Current Temperature in degrees Fahrenheit: %.2f", finalTempData[1]);
            }

            // LED power checkbox
            const char* ledBox;
            if (g_globals.enableLED)
            {
                ledBox = "On";
            }
            else
            {
                ledBox = "Off";
            }

            ImGui::Text("Toggle LED Screen Power:"); ImGui::SameLine();

            if (ImGui::Checkbox(ledBox, &g_globals.enableLED)) {
                g_globals.currentID = globals::MessageID::setDisplay;
                if (!writeData()) {
                    cout << "Failed to write Enable LED command" << endl;
                }
            }

            // Temperature mode checkbox
            const char* tempBox;
            double yMin; double yMax;
            if (g_globals.faren)
            {
                tempBox = "Fahrenheit";
                yMin = 50; yMax = 122;
            }
            else
            {
                tempBox = "Celsius";
                yMin = 10; yMax = 50;
            }

            ImGui::Text("Temperature Mode:"); ImGui::SameLine();
            ImGui::Checkbox(tempBox, &g_globals.faren); ImGui::SameLine();
            ImGui::Text("(Double-Click on Graph After Changing Mode)");

            // Text message attributes:
            ImGui::Text("\nText Message Information:");

            ImGui::Text("Phone Number: +1 ("); ImGui::SameLine();

            // Phone number entry box width
            ImGui::PushItemWidth(50); // only affects labelled and framed widgets!!

            // Area code
            if (ImGui::InputText("  ", areaCode, sizeof(areaCode), ImGuiInputTextFlags_NoHorizontalScroll | ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsNoBlank))
            {
                // update string array and verify proper entry
                for (int i = 0; i < 3; i++)
                {
                    // if .+-*/ entered. OR if entered < 3 digits since chars aren't digits by default
                    if (!isdigit(areaCode[i]))
                    {       
                        areaCode[i] = '0';
                    }
                }
            }
            ImGui::SameLine(); ImGui::Text(")"); ImGui::SameLine();

            // First three digits
            if (ImGui::InputText("   ", phoneNum1, sizeof(phoneNum1), ImGuiInputTextFlags_NoHorizontalScroll | ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsNoBlank))
            {
                // update string array and verify proper entry
                for (int i = 0; i < 3; i++)
                {
                    // if .+-*/ entered. OR if entered < 3 digits since chars aren't digits by default
                    if (!isdigit(phoneNum1[i]))
                    {
                        phoneNum1[i] = '0';
                    }
                }
            }
            ImGui::SameLine(); ImGui::Text("-"); ImGui::SameLine();
            
            // last 4 digits
            if (ImGui::InputText("    ", phoneNum2, sizeof(phoneNum2), ImGuiInputTextFlags_NoHorizontalScroll | ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsNoBlank))
            {
                // update string array and verify proper entry
                for (int i = 0; i < 4; i++)
                {
                    // if .+-*/ entered. OR if entered < 4 digits since chars aren't digits by default
                    if (!isdigit(phoneNum2[i]))
                    {
                        phoneNum2[i] = '0';
                    }
                }
            }

            // Carrier dropdown
            ImGui::SameLine();
            ImGui::PushItemWidth(150);
            if (ImGui::Combo("Cell Carrier", (int*)&g_globals.selectedCarrier, carrierNames, numOfCarriers)) {
                switch (g_globals.selectedCarrier) {
                case CellCarrier::AT_T:
                    g_globals.carrier = "txt.att.net";
                    break;
                case CellCarrier::Boost_Mobile:
                    g_globals.carrier = "sms.myboostmobile.com";
                    break;
                case CellCarrier::Consumer_Cellular:
                    g_globals.carrier = "mailmymobile.net";
                    break;
                case CellCarrier::Cricket:
                    g_globals.carrier = "sms.cricketwireless.net";
                    break;
                case CellCarrier::C_Spire:
                    g_globals.carrier = "cspire1.com";
                    break;
                case CellCarrier::G_Fi:
                    g_globals.carrier = "msg.fi.google.com";
                    break;
                case CellCarrier::M_Mobile:
                    g_globals.carrier = "mailmymobile.net";
                    break;
                case CellCarrier::Sprint:
                    g_globals.carrier = "pm.sprint.com";
                    break;
                case CellCarrier::TMobile:
                    g_globals.carrier = "tmomail.net";
                    break;
                case CellCarrier::US_Cellular:
                    g_globals.carrier = "email.uscc.net";
                    break;
                case CellCarrier::US_Mobile:
                    g_globals.carrier = "vtext.com";
                    break;
                case CellCarrier::Verizon:
                    g_globals.carrier = "vtext.com";
                    break;
                case CellCarrier::V_Mobile:
                    g_globals.carrier = "vmobl.com";
                    break;
                case CellCarrier::X_Mobile:
                    g_globals.carrier = "vtext.com";
                    break;
                default:
                    g_globals.carrier = "txt.att.net";
                    break;
                };
            }




            ImGui::PopItemWidth();
            ImGui::PushItemWidth(75);

            static float lowerThreshold = 0.0f;
            static float upperThreshold = 0.0f;
            bool inLowerThres = false;
            bool inUpperThres = true;


            // ImGui::Text("Threshold Values:");
            if (g_globals.faren)
            {
                ImGui::Text("Lower Threshold in Fahrenheit:"); ImGui::SameLine();
                if (ImGui::InputFloat("      ", &lowerThreshold, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_CharsDecimal))
                {
                    inLowerThres = true;
                    for (int i = 0; i < (MAX_SECONDS + 2); i++)
                    {
                        lowerThres[i] = lowerThreshold;
                    }
                }
                ImGui::Text("Upper Threshold in Fahrenheit:"); ImGui::SameLine();
                if (ImGui::InputFloat("       ", &upperThreshold, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_CharsDecimal))
                {
                    inUpperThres = true;
                    for (int i = 0; i < (MAX_SECONDS + 2); i++)
                    {
                        upperThres[i] = upperThreshold;
                    }
                }
            }
            else
            {
                ImGui::Text("Lower Threshold in Celsius:"); ImGui::SameLine();
                if (ImGui::InputFloat("      ", &lowerThreshold, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_CharsDecimal))
                {
                    inLowerThres = true;
                    for (int i = 0; i < (MAX_SECONDS + 2); i++)
                    {
                        lowerThres[i] = lowerThreshold;
                    }
                }
                ImGui::Text("Upper Threshold in Celsius:"); ImGui::SameLine();
                if (ImGui::InputFloat("       ", &upperThreshold, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_CharsDecimal))
                {
                    inUpperThres = true;
                    for (int i = 0; i < (MAX_SECONDS + 2); i++)
                    {
                        upperThres[i] = upperThreshold;
                    }
                }
            }

            // this monstrosity checks if that the lower and upper thres are valid. If not, reset them
            // ignores 0 since that's the initial value
            // note that if an upper bound is entered that's less than the lower bound, the lower bound changes! We can make it the other way around
            if (lowerThres[0] != 0 && upperThres[0] != 0 && // inLowerThres &&
                lowerThres[0] > upperThres[0])
            {
                for (int i = 0; i < (MAX_SECONDS + 2); i++)
                {
                    upperThres[i] = lowerThres[i] + 1;
                }
                upperThreshold = lowerThres[0] + 1;
            }
            else if (lowerThres[0] != 0 && upperThres[0] != 0 && inUpperThres &&
                upperThres[0] < lowerThres[0] &&
                ((int)(upperThreshold/10) >= ((int)(lowerThreshold/10)))) // don't ask, but this covers the case where 50 is your lower, you want to type 90 so you type 9, then lower resets to 8
            {
                for (int i = 0; i < (MAX_SECONDS + 2); i++)
                {
                    lowerThres[i] = upperThres[i] - 1;
                }
                lowerThreshold = upperThres[0] - 1;
            }

            ImGui::PopItemWidth();


            ImGui::Text("Text Message Content:"); ImGui::SameLine();
            //ImGui::PushItemWidth(-1.0f);
            //ImGui::PushTextWrapPos();
            if (ImGui::InputTextMultiline(" ", messageBuf, sizeof(messageBuf), ImVec2(600, ImGui::GetTextLineHeightWithSpacing() * 2)))
            {
                g_globals.message = messageBuf;
            }
            //ImGui::PopTextWrapPos();
            //ImGui::PopItemWidth();


            ImGui::Checkbox("Enable Text Message?", &enableTextMessage);

            // if text message is enabled AND temperature isn't already outside either threshold
            if (enableTextMessage && !alreadySent)
            {
                // if outside thresholds, send message
                // TODO can split up statement if we want a unique message for under lower threshold vs above upper threshold
                if (finalTempData[1] < lowerThreshold || finalTempData[1] > upperThreshold)
                {
                    // update phone number
                    g_globals.phoneNumber = "";
                    for (int i = 0; i < 10; i++)
                    {
                        if (i <= 2) // area code
                        {
                            g_globals.phoneNumber.append(std::to_string(areaCode[i]));
                        }
                        else if (i > 2 && i <= 5) // middle 3 digits
                        {
                            g_globals.phoneNumber.append(std::to_string(phoneNum1[i - 3]));
                        }
                        else // last 4 digits
                        {
                            g_globals.phoneNumber.append(std::to_string(phoneNum2[i - 6]));
                        }
                    }

                    alreadySent = true;
                    // TODO: SendSMS();
                }
            }
            // if was outside of threshold range (alreadySent == true) AND is now inside threshold range, update alreadySent so text can be sent again
            else if (alreadySent && 
                (finalTempData[1] > lowerThreshold) && 
                (finalTempData[1] < upperThreshold))
            {
                alreadySent = false;
            }

            ImGui::NewLine();
            ImGui::Text("LED Brightness and Contrast:");

            ImGui::PushItemWidth(300);

            //Update brightness
            ImGui::SliderInt("Brightness", &g_globals.brightness, 0, 254);
            ImGui::SameLine();
            if (ImGui::Button("Update")) {
                g_globals.currentID = globals::MessageID::setBrightness;
                if (!writeData()) {
                    cout << "Failed to update brightness" << endl;
                }
            }

            // Update contrast
            ImGui::SliderInt("Contrast", &g_globals.contrast, 0, 100);
            ImGui::SameLine();
            if (ImGui::Button("Update ")) {
                g_globals.currentID = globals::MessageID::setDisplayGamma;
                if (!writeData()) {
                    cout << "Failed to update contrast" << endl;
                }
            }

            ImGui::NewLine();

            ImGui::PopItemWidth();



            //ImPlotStyleVar_FitPadding,         // ImVec2, additional fit padding as a percentage of the fit extents (e.g. ImVec2(0.1f,0.1f) adds 10% to the fit extents of X and Y)
            //ImPlotStyleVar_PlotDefaultSize,    // ImVec2, default size used when ImVec2(0,0) is passed to BeginPlot
            //ImPlotStyleVar_PlotMinSize,        // ImVec2, minimum size plot frame can be when shrunk

            // To change plot size:
            // ImPlot::PushStyleVar(ImPlotStyleVar_PlotDefaultSize, ImVec2(-100, 100));



            ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 2.0f);  // Set line thickness to 2.0
            ImPlot::PushStyleVar(ImPlotStyleVar_Marker, 1);  // type of point/marker
            ImPlot::PushStyleVar(ImPlotStyleVar_MarkerSize, 2.0f);  // size of point/marker




            if (ImPlot::BeginPlot("Temperature Data (Right-Click for Options)", ImVec2(-1, 0), ImPlotFlags_Crosshairs)) {
                if (g_globals.faren)
                {
                    ImPlot::SetupAxes("Seconds ago from current time", "Temperature in Degrees Fahrenheit",
                        ImPlotAxisFlags_Invert, // make x axis go from 300-0 instead of 0-300
                        ImPlotAxisFlags_Opposite // visually move y axis to the right side of the graph
                    );
                }
                else
                {
                    ImPlot::SetupAxes("Seconds ago from current time", "Temperature in Degrees Celsius",
                        ImPlotAxisFlags_Invert, // make x axis go from 300-0 instead of 0-300
                        ImPlotAxisFlags_Opposite // visually move y axis to the right side of the graph
                    );
                }


                ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, 0, MAX_SECONDS);
                ImPlot::SetupAxisLimitsConstraints(ImAxis_Y1, yMin, yMax);

                if (empty(finalTempData) == false)
                {
                    ImPlot::PlotLine("Recorded Temperature", &finalTempData[0], MAX_SECONDS + 1,
                        1.0,
                        0,
                        ImPlotLineFlags_NoClip // makes points/markers on border of constraints visible
                    );
                }

                // This empty check doesn't work lmao, by defaul lower and upper thres are filled with 0's
                // This means if threshold values aren't set, it'll still plot a line at temp = 0
                // So that means there are two offscreen lines at temp = 0, but those are outside the graph limits so...
                if (empty(lowerThres) == false)
                {
                    ImPlot::PlotLine("Lower Threshold", &lowerThres[0], MAX_SECONDS + 1,
                        1.0,
                        0,
                        ImPlotLineFlags_NoClip // makes points/markers on border of constraints visible
                    );
                }

                if (empty(upperThres) == false)
                {
                    ImPlot::PlotLine("Upper Threshold", &upperThres[0], MAX_SECONDS + 1,
                        1.0,
                        0,
                        ImPlotLineFlags_NoClip // makes points/markers on border of constraints visible
                    );
                }

                ImPlot::EndPlot();
            }

            ImGui::End();


            ImPlot::PopStyleVar();

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