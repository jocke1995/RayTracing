#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

// Misc Walnut
#include "Walnut/Timer.h"

// Raytracing specific
#include "Renderer.h"
#include "Camera.h"
using namespace Walnut;
class ExampleLayer : public Walnut::Layer
{
public:

   ExampleLayer() 
      : m_Camera(45.0f, 0.1f, 100.0f) {};

   virtual void OnUpdate(float ts) override
   {
      m_Camera.Update(ts);
   }

   virtual void OnUIRender() override
   {
      ImGui::Begin("Settings");
      ImGui::Text("Last render: %.3fms", m_LastRenderTime);
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

      Render();
   }

   void Render()
   {
      Timer timer;

      // Render
      {
         // Resize if needed
         {
            m_Renderer.Resize(m_ViewportWidth, m_ViewportHeight);
            m_Camera.Resize(m_ViewportWidth, m_ViewportHeight);
         }

         m_Renderer.Render(m_Camera);
      }

      m_LastRenderTime = timer.ElapsedMillis();
   }

private:
   Renderer m_Renderer;
   Camera m_Camera;

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