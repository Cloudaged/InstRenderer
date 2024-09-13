
#include "RenderPassManager.h"

RenderPassManager::RenderPassManager()
{

}

void RenderPassManager::ExecuteAllPass()
{
    geometryPass->Execute(view);

}

void RenderPassManager::Build()
{
    geometryPass->Build();

}

void RenderPassManager::Setup()
{

    this->geometryPass = new GeometryPass(globalDescriptorData);
    geometryPass->SetupAttachments();
}
