#include "widgets.h"

static int callback(ImGuiInputTextCallbackData* data)
{
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
    {        
        std::string* str = (std::string*)data->UserData;
        IM_ASSERT(data->Buf == str->c_str());
        str->resize(data->BufTextLen);
        data->Buf = (char*)str->c_str();
    }

    return 0;
}
   
bool ImGui::InputTextWithHintR(std::string label, std::string& value, const ImVec2& size, ImGuiInputTextFlags flags)
{
    return ImGui::InputTextWithHint("##", label.c_str(), (char*)value.data(), value.capacity() + 1, ImGuiInputTextFlags_CallbackResize, callback, &value);
}