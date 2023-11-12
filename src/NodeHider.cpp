#include "NodeHider.h"

SINGLETONBODY(DeviousDevices::NodeHider)

#ifdef NH_IMPARMHIDER

void DeviousDevices::NodeHider::HideArms(RE::Actor* a_actor)
{
    if (a_actor == nullptr) return;

    RE::NiNode* thirdpersonNode = a_actor->Get3D(0)->AsNode();

    if (thirdpersonNode == nullptr) return;

    RE::NiNode* leftarmNode     = thirdpersonNode->GetObjectByName("NPC L UpperArm [LUar]")->AsNode();
    if (leftarmNode == nullptr) return;

    RE::NiNode* rightarmNode    = thirdpersonNode->GetObjectByName("NPC R UpperArm [RUar]")->AsNode();
    if (rightarmNode == nullptr) return;

    leftarmNode->local.scale  = 0.002f;
    rightarmNode->local.scale = 0.002f;
}

void DeviousDevices::NodeHider::ShowArms(RE::Actor* a_actor)
{
    if (a_actor == nullptr) return;

    RE::NiNode* thirdpersonNode = a_actor->Get3D(0)->AsNode();

    if (thirdpersonNode == nullptr) return;

    RE::NiNode* leftarmNode     = thirdpersonNode->GetObjectByName("NPC L UpperArm [LUar]")->AsNode();
    if (leftarmNode == nullptr) return;

    RE::NiNode* rightarmNode    = thirdpersonNode->GetObjectByName("NPC R UpperArm [RUar]")->AsNode();
    if (rightarmNode == nullptr) return;

    leftarmNode->local.scale  = 1.000f;
    rightarmNode->local.scale = 1.000f;
}

#endif

void DeviousDevices::NodeHider::HideWeapons(RE::Actor* a_actor)
{
    if (a_actor == nullptr) return;

    //LOG("HideWeapons called for {}",a_actor->GetName());

    for (auto&& it : WeaponNodes)
    {
        AddHideNode(a_actor,it);
    }
}



void DeviousDevices::NodeHider::ShowWeapons(RE::Actor* a_actor)
{
    if (a_actor == nullptr) return;

    //LOG("ShowWeapons called for {}",a_actor->GetName());

    if (_slots.find(a_actor->GetHandle().native_handle()) == _slots.end())
    {
        //LOG("Actor {} have no hiden nodes",a_actor->GetName());
        return;
    }

    for (auto&& it : WeaponNodes)
    {
        RemoveHideNode(a_actor,it);
    }
}

void DeviousDevices::NodeHider::Setup()
{ 
}

void DeviousDevices::NodeHider::Update()
{
    for (auto&& it : _slots)
    {

        RE::Actor* loc_actor = RE::Actor::LookupByHandle(it.first).get();
        if (ValidateActor(loc_actor))
        {
            if (loc_actor->Is3DLoaded())
            {
                NodeHiderSlot* loc_slot = &it.second;
                if (loc_slot->enabled)
                {
                    if (loc_slot->nodes.size() > 0)
                    {
                        RE::NiNode* loc_thirdpersonNode = loc_actor->Get3D(false)->AsNode();
                        if (loc_thirdpersonNode == nullptr) 
                        {
                            WARN("Cant hide nodes on actor {} because its 3D is none",loc_actor->GetName())
                            continue;
                        }
                        for (auto&& it2 : loc_slot->nodes)
                        {
                            RE::NiAVObject* loc_node = loc_thirdpersonNode->GetObjectByName(it2);
                            if (loc_node != nullptr) loc_node->local.scale = 0.002f;
                            else WARN("Cant hide node {} on actor {} because its none",it2,loc_actor->GetName())
                        }
                    }
                    else
                    {
                        //no nodes, unregister npc to save resources
                        _slots.erase(it.first);
                        //LOG("Update({},{}) - Actor have no more nodes to hide, unregistering, new size={})",loc_actor->GetName(),a_delta,_slots.size())
                    }
                }
            }
        }
    }
}

bool DeviousDevices::NodeHider::ValidateActor(RE::Actor* a_actor)
{
    if (a_actor == nullptr) return false;
    
    if (a_actor->IsDead())
    {
        _slots.erase(a_actor->GetHandle().native_handle());
        return false;
    }
    return true;
}

bool DeviousDevices::NodeHider::AddHideNode(RE::Actor* a_actor, std::string a_nodename)
{
    if (a_actor == nullptr) return false;
    
    RE::NiNode* loc_thirdpersonNode = a_actor->Get3D(false)->AsNode();
    
    if (loc_thirdpersonNode == nullptr) return false;
    
    RE::NiAVObject* loc_node = loc_thirdpersonNode->GetObjectByName(a_nodename);
    if (loc_node != nullptr)
    {
        auto loc_handle = a_actor->GetHandle().native_handle();
        auto loc_nodes = _slots[loc_handle].nodes;
        if (std::find(loc_nodes.begin(),loc_nodes.end(), a_nodename) == loc_nodes.end())
        {
            loc_nodes.push_back(a_nodename);
            //LOG("AddHideNode({},{})",a_actor->GetName(),a_nodename);
            return true;
        }
        else
        {
            //LOG("AddHideNode({},{}) - Node already present, skipping",a_actor->GetName(),a_nodename);
            return false;
        }
    }
    return false;
}

bool DeviousDevices::NodeHider::RemoveHideNode(RE::Actor* a_actor, std::string a_nodename)
{
    if (a_actor == nullptr) return false;
    
    auto loc_handle = a_actor->GetHandle().native_handle();
    if (_slots.find(loc_handle) == _slots.end())
    {
        LOG("Actor {} have no hiden nodes",a_actor->GetName());
        return false;
    }
    
    RE::NiNode* loc_thirdpersonNode = a_actor->Get3D(false)->AsNode();
    
    if (loc_thirdpersonNode == nullptr) return false;
    
    RE::NiAVObject* loc_node = loc_thirdpersonNode->GetObjectByName(a_nodename);
    if (loc_node != nullptr)
    {
        if (std::find(_slots[loc_handle].nodes.begin(),_slots[loc_handle].nodes.end(), a_nodename) != _slots[loc_handle].nodes.end())
        {
            _slots[loc_handle].nodes.erase(std::find(_slots[loc_handle].nodes.begin(),_slots[loc_handle].nodes.end(), a_nodename));
            loc_node->AsNode()->local.scale = 1.00f;
            return true;
        }
    }
    return false;
}

void DeviousDevices::HideWeapons(PAPYRUSFUNCHANDLE, RE::Actor* a_actor)
{
    NodeHider::GetSingleton()->HideWeapons(a_actor);
}

void DeviousDevices::ShowWeapons(PAPYRUSFUNCHANDLE, RE::Actor* a_actor)
{
    NodeHider::GetSingleton()->ShowWeapons(a_actor);
}