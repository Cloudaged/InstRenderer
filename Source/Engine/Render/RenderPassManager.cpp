
#include "RenderPassManager.h"

RenderPassManager::RenderPassManager()
{

}

void RenderPassManager::ExecuteAllPass()
{
    compositionPass->Execute(view);

}

void RenderPassManager::Build()
{
    geoPass->Build();
    compositionPass->Build();
}

void RenderPassManager::Setup()
{

    this->compositionPass = new CompositionPass(globalDescriptorData);
    compositionPass->SetupAttachments();

    this->geoPass = new GeoPass(globalDescriptorData);
    geoPass->SetupAttachments();
}
