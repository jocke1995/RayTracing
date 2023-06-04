#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

// Misc Walnut
#include "Walnut/Timer.h"

// Raytracing specific
#include "Renderer.h"

static bool RENDER_EVERY_FRAME = false;

using namespace Walnut;
class ExampleLayer : public Walnut::Layer
{
public:
   virtual void OnUIRender() override
   {
      ImGui::Begin("Settings");
      ImGui::Text("Last render: %.3fms", m_LastRenderTime);
      if (RENDER_EVERY_FRAME == true)
      {
         Render();
      }
      else
      {
         if (ImGui::Button("Render"))
         {
            Render();
         }
      }

      ImGui::End();

      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
      ImGui::Begin("Viewport");

      m_ViewportWidth  = ImGui::GetContentRegionAvail().x;
      m_ViewportHeight = ImGui::GetContentRegionAvail().y;

      if (auto finalImage = m_Renderer.GetFinalImage())
      {
         ImGui::Image(  finalImage->GetDescriptorSet(),
                        { (float)finalImage->GetWidth(), (float)finalImage->GetHeight() },
                        ImVec2(0, 1), ImVec2(1, 0)); // Flip UVs
      }

      ImGui::End();
      ImGui::PopStyleVar();
   }

   void Render()
   {
      Timer timer;

      // Render
      {
         m_Renderer.Resize(m_ViewportWidth, m_ViewportHeight);

         m_Renderer.Render();
      }

      m_LastRenderTime = timer.ElapsedMillis();
   }

private:
   Renderer m_Renderer;

   uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

   float m_LastRenderTime = 0.0f;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
   Walnut::ApplicationSpecification spec;
   spec.Name = "CPU RayTracing";

   Walnut::Application* app = new Walnut::Application(spec);
   app->PushLayer<ExampleLayer>();
   app->SetMenubarCallback([app]()
   {
      if (ImGui::BeginMenu("File"))
      {
         if (ImGui::MenuItem("Exit"))
         {
            app->Close();
         }
         ImGui::EndMenu();
      }
   });
   return app;
}